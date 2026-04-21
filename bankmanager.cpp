#include "bankmanager.h"

// [생성자]
BankManager::BankManager(QObject *parent)
    : QObject(parent)
{
    m_accountModel     = new AccountModel(this);
    m_transactionModel = new TransactionModel(this);
}

// 계좌 추가
bool BankManager::addAccount(const QString &accNum,
                             const QString &password,
                             const QString &bank,
                             qint64 initial)
{
    // 중복 계좌번호 체크
    for (const auto &acc : m_accountModel->accounts()) {
        if (acc.accountNumber == accNum) return false;
    }

    Account acc;
    acc.id             = m_nextAccountId++;
    acc.accountNumber  = accNum;
    acc.password       = password; // [보안] 비밀번호 저장 추가
    acc.bankName       = bank;
    acc.initialBalance = initial;
    acc.currentBalance = initial;

    m_accountModel->addAccount(acc);
    return true;
}

// 입금 처리
void BankManager::deposit(int accountId,
                          qint64 amount,
                          const QString &memo)
{
    Transaction tx;
    tx.id = m_nextTransactionId++;
    tx.accountId  = accountId;
    tx.amount     = amount;
    tx.type       = TransactionType::Deposit;
    tx.status     = TransactionStatus::Posted;
    tx.memo       = memo;
    tx.occurredAt = QDateTime::currentDateTime();

    m_transactionModel->addTransaction(tx);
    recalcAllBalances();
}

//출금 처리
// 잔고 부족 시 false 반환
bool BankManager::withdraw(int accountId,
                           qint64 amount,
                           const QString &memo)
{
    // 잔고 부족 체크
    for (const auto &acc : m_accountModel->accounts()) {
        if (acc.id == accountId) {
            if (acc.currentBalance < amount) return false;
            break;
        }
    }

    Transaction tx;
    tx.id         = m_nextTransactionId++;
    tx.accountId  = accountId;
    tx.amount     = amount;
    tx.type       = TransactionType::Withdraw;
    tx.status     = TransactionStatus::Posted;
    tx.memo       = memo;
    tx.occurredAt = QDateTime::currentDateTime();

    m_transactionModel->addTransaction(tx);
    recalcAllBalances();
    return true;
}

// 송금 처리 추가
// 원자성 보장 : TransferOut / TransferIn 두 거래를 동시에 생성
// transferGroupId 로 두 거래를 묶어서 관리
bool BankManager::transfer(int fromAccountId,
                           int toAccountId,
                           qint64 amount,
                           const QString &memo)
{
    // 동일 계좌 송금 방지
    if (fromAccountId == toAccountId) return false;

    // 출금 계좌 잔고 부족 체크
    bool fromExists = false;
    bool toExists   = false;
    
    QString fromAccNum = "";
    QString toAccNum = "";

    for (const auto &acc : m_accountModel->accounts()) {
        if (acc.id == fromAccountId) {
            if (!acc.allowOverdraft && acc.currentBalance < amount) return false;
            fromExists = true;
            fromAccNum = acc.accountNumber;
        }
        if (acc.id == toAccountId) {
            toExists = true;
            toAccNum = acc.accountNumber;
        }
    }

    // 존재하지 않는 계좌 체크
    if (!fromExists || !toExists) return false;

    // 송금 쌍 묶음 ID 발급
    int groupId = m_nextTransferGroupId++;

    // TransferOut : 출금 계좌에서 나가는 거래
    Transaction txOut;
    txOut.id              = m_nextTransactionId++;
    txOut.accountId       = fromAccountId;
    txOut.amount          = amount;
    txOut.type            = TransactionType::TransferOut;
    txOut.status          = TransactionStatus::Posted;
    txOut.memo            = memo;
    txOut.occurredAt      = QDateTime::currentDateTime();
    txOut.transferGroupId = groupId;
    txOut.counterpartyAccount = toAccNum; // 거래 상대(받는 계좌) 추가

    // TransferIn : 입금 계좌로 들어오는 거래
    Transaction txIn;
    txIn.id              = m_nextTransactionId++;
    txIn.accountId       = toAccountId;
    txIn.amount          = amount;
    txIn.type            = TransactionType::TransferIn;
    txIn.status          = TransactionStatus::Posted;
    txIn.memo            = memo;
    txIn.occurredAt      = QDateTime::currentDateTime();
    txIn.transferGroupId = groupId;
    txIn.counterpartyAccount = fromAccNum; // 거래 상대(보내는 계좌) 추가

    m_transactionModel->addTransaction(txOut);
    m_transactionModel->addTransaction(txIn);
    recalcAllBalances();
    return true;
}

// 전체 잔고 재계산
void BankManager::recalcAllBalances()
{
    auto &accounts = m_accountModel->accounts();

    // 모든 계좌 잔고를 초기 잔고로 리셋
    for (auto &acc : accounts) {
        acc.currentBalance = acc.initialBalance;
    }

    // 정상 거래만 잔고에 반영
    const auto &transactions = m_transactionModel->transactions();
    for (const auto &tx : transactions) {
        // 취소된 거래는 잔고 계산에서 제외
        if (tx.status != TransactionStatus::Posted)
            continue;
        for (auto &acc : accounts) {
            if (acc.id == tx.accountId) {
                if (tx.type == TransactionType::Deposit || tx.type == TransactionType::TransferIn) {
                    acc.currentBalance += tx.amount;
                } else if (tx.type == TransactionType::Withdraw || tx.type == TransactionType::TransferOut) {
                    acc.currentBalance -= tx.amount;
                }
                break;
            }
        }
    }

    m_accountModel->updateAll();
}

// 계좌 삭제
bool BankManager::removeAccount(int accountId)
{
    // ──────────────────────────────────────────────────────────
    // 1. accountId 로 행 번호(row) 찾기
    //    AccountModel::removeAccount() 가 row 를 받으니까
    //    id → row 변환이 필요해요
    // ──────────────────────────────────────────────────────────
    const auto &accounts = m_accountModel->accounts();
    int row = -1;
    for (int i = 0; i < accounts.size(); i++) {
        if (accounts[i].id == accountId) {
            row = i;
            break;
        }
    }

    if (row == -1) return false;

    // ──────────────────────────────────────────────────────────
    // 2. 해당 계좌의 거래 내역 전부 Canceled 처리
    // ──────────────────────────────────────────────────────────
    QList<Transaction> transactions = m_transactionModel->transactions();
    for (auto &tx : transactions) {
        if (tx.accountId == accountId) {
            tx.status = TransactionStatus::Canceled;
        }
    }
    m_transactionModel->setTransactions(transactions);

    // ──────────────────────────────────────────────────────────
    // 3. 계좌 삭제 (row 로 전달)
    // ──────────────────────────────────────────────────────────
    m_accountModel->removeAccount(row);

    // ──────────────────────────────────────────────────────────
    // 4. 잔고 재계산
    // ──────────────────────────────────────────────────────────
    recalcAllBalances();

    return true;
}

// JSON 데이터 복원용
void BankManager::restoreAccount(const Account &acc)
{
    m_accountModel->addAccount(acc);
    if(acc.id >= m_nextAccountId) {
        m_nextAccountId = acc.id + 1;
    }
}

void BankManager::restoreTransaction(const Transaction &tx)
{
    m_transactionModel->addTransaction(tx);
    if(tx.id >= m_nextTransactionId) {
        m_nextTransactionId = tx.id + 1;
    }
    if(tx.transferGroupId >= m_nextTransferGroupId) {
        m_nextTransferGroupId = tx.transferGroupId + 1;
    }
}
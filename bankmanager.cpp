#include "bankmanager.h"

// [생성자]
BankManager::BankManager(QObject *parent)
    : QObject(parent)
{
    m_accountModel     = new AccountModel(this);
    m_transactionModel = new TransactionModel(this);
}

// 계좌 추가
bool BankManager::addAccount(const QString &name,
                             const QString &accNum,
                             const QString &bank,
                             qint64 initial)
{
    // 중복 계좌번호 체크
    for (const auto &acc : m_accountModel->accounts()) {
        if (acc.accountNumber == accNum) return false;
    }

    Account acc;
    acc.id             = m_nextAccountId++;
    acc.name           = name;
    acc.accountNumber  = accNum;
    acc.bankName       = bank;
    acc.initialBalance = initial;
    acc.currentBalance = initial;

    m_accountModel->addAccount(acc);
    return true;
}

// 거래 추가
void BankManager::addTransaction(int accountId,
                                 qint64 amount,
                                 TransactionType type,
                                 const QString &memo)
{
    Transaction tx;
    tx.id         = m_nextTransactionId++;
    tx.accountId  = accountId;
    tx.amount     = amount;
    tx.type       = type;
    tx.memo       = memo;
    tx.status     = TransactionStatus::Posted;
    tx.occurredAt = QDateTime::currentDateTime();

    m_transactionModel->addTransaction(tx);
    recalcAllBalances();
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
                if (tx.type == TransactionType::Deposit) {
                    acc.currentBalance += tx.amount;
                } else if (tx.type == TransactionType::Withdraw) {
                    acc.currentBalance -= tx.amount;
                }
                break;
            }
        }
    }

    m_accountModel->updateAll();
}
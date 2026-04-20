#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QMessageBox>
#include <algorithm>

// =======================================================================
// AccountManager
// =======================================================================

// ── 계좌 CRUD ─────────────────────────────────────────────────────────────

int AccountManager::createAccount(const Account &accountData, QString *outError)
{
    // TODO: name 비어있으면 오류
    // TODO: accountData 복사 → id = m_nextAccountId++ 세팅
    // TODO: m_accounts.append(newAccount)
    // TODO: recalcBalance(newAccount)
    Q_UNUSED(accountData)
    if (outError) *outError = "Not implemented";
    return -1;
}

bool AccountManager::updateAccount(int accountId, const Account &updated, QString *outError)
{
    // TODO: findAccount(accountId) — 없으면 오류
    // TODO: name, accountNumber, bankName, overdraftPolicy 만 덮어쓰기
    // TODO: initialBalance 변경 시도 감지 → 오류
    Q_UNUSED(accountId) Q_UNUSED(updated)
    if (outError) *outError = "Not implemented";
    return false;
}

bool AccountManager::deactivateAccount(int accountId, QString *outError)
{
    // TODO: findAccount(accountId) — 없으면 오류
    // TODO: 이미 Inactive 면 오류
    // TODO: status = Inactive
    Q_UNUSED(accountId)
    if (outError) *outError = "Not implemented";
    return false;
}

bool AccountManager::reactivateAccount(int accountId, QString *outError)
{
    // TODO: m_accounts 전체 순회 (Inactive 포함)
    // TODO: status = Active
    Q_UNUSED(accountId)
    if (outError) *outError = "Not implemented";
    return false;
}

// ── 계좌 조회 ─────────────────────────────────────────────────────────────

std::optional<Account> AccountManager::findById(int accountId) const
{
    // TODO: m_accounts 순회 → id 일치 시 해당 Account 반환
    // TODO: 없으면 std::nullopt
    Q_UNUSED(accountId)
    return std::nullopt;
}

QList<Account> AccountManager::activeAccounts() const
{
    // TODO: m_accounts 에서 status == Active 만 필터
    // TODO: 각 항목에 currentBalance 채워서 반환
    return {};
}

QList<Account> AccountManager::allAccounts() const
{
    // TODO: m_accounts 전체 복사 (currentBalance 포함)
    return {};
}

// ── 잔고 계산 ─────────────────────────────────────────────────────────────

qint64 AccountManager::currentBalanceOf(int accountId) const
{
    // TODO: findAccount → currentBalance 반환
    // TODO: 없으면 0
    Q_UNUSED(accountId)
    return 0;
}

qint64 AccountManager::totalActiveBalance() const
{
    // TODO: activeAccounts() 순회하며 currentBalance 합산
    return 0;
}

// ── 내부 데이터 접근 ──────────────────────────────────────────────────────

QList<Transaction> &AccountManager::transactions()             { return m_transactions; }
const QList<Transaction> &AccountManager::transactions() const { return m_transactions; }
int AccountManager::nextAccountId()     const { return m_nextAccountId; }
int AccountManager::nextTransactionId() const { return m_nextTransactionId; }

// ── private 헬퍼 ──────────────────────────────────────────────────────────

Account *AccountManager::findAccount(int accountId)
{
    // TODO: m_accounts 순회 → id 일치 시 포인터 반환, 없으면 nullptr
    Q_UNUSED(accountId)
    return nullptr;
}

const Account *AccountManager::findAccount(int accountId) const
{
    // TODO: const 버전 동일 로직
    Q_UNUSED(accountId)
    return nullptr;
}

void AccountManager::recalcBalance(Account &account) const
{
    // TODO: account.currentBalance = account.initialBalance
    // TODO: m_transactions 순회 (accountId 일치 + Posted 만)
    //         Deposit / TransferIn  → + amount
    //         Withdraw / TransferOut → - amount
    Q_UNUSED(account)
}

void AccountManager::recalcAllBalances()
{
    // TODO: for (auto &acc : m_accounts) recalcBalance(acc);
}

// =======================================================================
// DepositModule
// =======================================================================

DepositModule::DepositModule(AccountManager *manager) : m_manager(manager) {}

int DepositModule::deposit(int accountId, qint64 amount,
                           const QDateTime &occurredAt,
                           const QString &memo, const QString &category,
                           QString *outError)
{
    // TODO: validateAmount(amount, outError)
    // TODO: m_manager->findAccount(accountId) — 없으면 오류
    // TODO: Transaction 생성 (type=Deposit, status=Posted) → id = m_manager->m_nextTransactionId++
    // TODO: m_manager->m_transactions.append(tx)
    // TODO: m_manager->recalcBalance(*account)
    // TODO: 생성된 tx.id 반환
    Q_UNUSED(accountId) Q_UNUSED(amount) Q_UNUSED(occurredAt)
    Q_UNUSED(memo) Q_UNUSED(category)
    if (outError) *outError = "Not implemented";
    return -1;
}

bool DepositModule::updateDeposit(int transactionId, qint64 newAmount,
                                  const QDateTime &newOccurredAt,
                                  const QString &newMemo, const QString &newCategory,
                                  QString *outError)
{
    // TODO: m_manager->transactions() 에서 transactionId 탐색
    // TODO: Canceled 이면 오류
    // TODO: type != Deposit 이면 오류
    // TODO: validateAmount(newAmount)
    // TODO: 필드 수정 후 recalcBalance 호출
    Q_UNUSED(transactionId) Q_UNUSED(newAmount) Q_UNUSED(newOccurredAt)
    Q_UNUSED(newMemo) Q_UNUSED(newCategory)
    if (outError) *outError = "Not implemented";
    return false;
}

bool DepositModule::validateAmount(qint64 amount, QString *outError) const
{
    // TODO: amount <= 0 이면 오류 메시지 세팅 후 false
    Q_UNUSED(amount)
    if (outError) *outError = "Not implemented";
    return false;
}

// =======================================================================
// WithdrawModule
// =======================================================================

WithdrawModule::WithdrawModule(AccountManager *manager) : m_manager(manager) {}

int WithdrawModule::withdraw(int accountId, qint64 amount,
                             const QDateTime &occurredAt,
                             const QString &memo, QString *outError)
{
    // TODO: validateAmount(amount)
    // TODO: checkOverdraft(accountId, amount) — Deny 정책이면 잔고 확인
    // TODO: Transaction 생성 (type=Withdraw) → 추가 및 잔고 재계산
    // TODO: tx.id 반환
    Q_UNUSED(accountId) Q_UNUSED(amount) Q_UNUSED(occurredAt) Q_UNUSED(memo)
    if (outError) *outError = "Not implemented";
    return -1;
}

bool WithdrawModule::updateWithdraw(int transactionId, qint64 newAmount,
                                    const QDateTime &newOccurredAt,
                                    const QString &newMemo, QString *outError)
{
    // TODO: 거래 탐색 (Withdraw 타입, Posted 상태 확인)
    // TODO: 수정 후 잔고 정책 위반 여부 재확인
    // TODO: 필드 수정 + recalcBalance
    Q_UNUSED(transactionId) Q_UNUSED(newAmount) Q_UNUSED(newOccurredAt) Q_UNUSED(newMemo)
    if (outError) *outError = "Not implemented";
    return false;
}

bool WithdrawModule::validateAmount(qint64 amount, QString *outError) const
{
    // TODO: amount <= 0 이면 오류
    Q_UNUSED(amount)
    if (outError) *outError = "Not implemented";
    return false;
}

bool WithdrawModule::checkOverdraft(int accountId, qint64 amount, QString *outError) const
{
    // TODO: m_manager->currentBalanceOf(accountId) - amount < 0 이고
    //         policy == Deny 이면 오류 반환
    Q_UNUSED(accountId) Q_UNUSED(amount)
    if (outError) *outError = "Not implemented";
    return false;
}

// =======================================================================
// TransferModule
// =======================================================================

TransferModule::TransferModule(AccountManager *manager) : m_manager(manager) {}

int TransferModule::transfer(int fromAccountId, int toAccountId, qint64 amount,
                             const QDateTime &occurredAt, const QString &memo,
                             QString *outError)
{
    // TODO: validateTransfer(from, to, amount)
    // TODO: 출금 측 잔고 정책 확인 (Deny 이면 잔고 체크)
    // TODO: groupId = m_manager->m_nextTransferGroupId++
    // TODO: TransferOut 거래 생성 (from 계좌)
    // TODO: TransferIn  거래 생성 (to 계좌)
    // TODO: 둘 다 m_transactions 에 추가
    // TODO: 두 계좌 recalcBalance 호출
    // TODO: groupId 반환
    Q_UNUSED(fromAccountId) Q_UNUSED(toAccountId) Q_UNUSED(amount)
    Q_UNUSED(occurredAt) Q_UNUSED(memo)
    if (outError) *outError = "Not implemented";
    return -1;
}

bool TransferModule::validateTransfer(int fromAccountId, int toAccountId,
                                      qint64 amount, QString *outError) const
{
    // TODO: from == to 이면 오류
    // TODO: amount <= 0 이면 오류
    // TODO: 두 계좌 모두 존재하고 Active 인지 확인
    Q_UNUSED(fromAccountId) Q_UNUSED(toAccountId) Q_UNUSED(amount)
    if (outError) *outError = "Not implemented";
    return false;
}

// =======================================================================
// CorrectionModule
// =======================================================================

CorrectionModule::CorrectionModule(AccountManager *manager) : m_manager(manager) {}

bool CorrectionModule::cancelTransaction(int transactionId, QString *outError)
{
    // TODO: 거래 탐색 — 없으면 오류
    // TODO: 이미 Canceled 이면 오류
    // TODO: TransferOut/In 이면 findTransferPair 로 쌍을 찾아 함께 취소 (원자성)
    // TODO: markCanceled 호출 후 recalcAllBalances
    Q_UNUSED(transactionId)
    if (outError) *outError = "Not implemented";
    return false;
}

bool CorrectionModule::correctTransaction(int transactionId,
                                          const CorrectionRequest &req,
                                          QString *outError)
{
    // TODO: Canceled 이면 오류
    // TODO: 송금 거래고 amount 변경이면 쌍 모두 수정
    // TODO: applyCorrection 호출
    // TODO: recalcAllBalances 후 정책 위반 확인 → 위반 시 원복
    Q_UNUSED(transactionId) Q_UNUSED(req)
    if (outError) *outError = "Not implemented";
    return false;
}

bool CorrectionModule::isCanceled(int transactionId) const
{
    // TODO: 거래 탐색 → status == Canceled 반환
    Q_UNUSED(transactionId)
    return false;
}

int CorrectionModule::findTransferPair(int transactionId) const
{
    // TODO: transactionId 의 transferGroupId 로 같은 그룹 내 반대 타입 거래 탐색
    Q_UNUSED(transactionId)
    return -1;
}

bool CorrectionModule::markCanceled(int transactionId, QString *outError)
{
    // TODO: 거래 탐색 → status = Canceled
    Q_UNUSED(transactionId)
    if (outError) *outError = "Not implemented";
    return false;
}

bool CorrectionModule::applyCorrection(int transactionId,
                                       const CorrectionRequest &req,
                                       QString *outError)
{
    // TODO: req 의 change* 플래그를 보고 해당 필드만 수정
    Q_UNUSED(transactionId) Q_UNUSED(req)
    if (outError) *outError = "Not implemented";
    return false;
}

// =======================================================================
// TransactionFilter
// =======================================================================

TransactionFilter::TransactionFilter(const AccountManager *manager) : m_manager(manager) {}

QList<Transaction> TransactionFilter::query(const FilterOptions &options) const
{
    // TODO: resolveDateRange(options) 로 [from, to] 구하기
    // TODO: m_manager->transactions() 순회
    //         matchesType, matchesStatus, matchesDate, matchesMemo 모두 통과한 것만 수집
    // TODO: newestFirst 기준으로 정렬
    Q_UNUSED(options)
    return {};
}

qint64 TransactionFilter::sumOf(const QList<Transaction> &txs) const
{
    // TODO: Deposit/TransferIn → +amount, Withdraw/TransferOut → -amount 합산
    Q_UNUSED(txs)
    return 0;
}

qint64 TransactionFilter::totalDeposit(const QList<Transaction> &txs) const
{
    // TODO: type == Deposit || TransferIn 인 것의 amount 합산
    Q_UNUSED(txs)
    return 0;
}

qint64 TransactionFilter::totalWithdraw(const QList<Transaction> &txs) const
{
    // TODO: type == Withdraw || TransferOut 인 것의 amount 합산
    Q_UNUSED(txs)
    return 0;
}

std::pair<QDateTime, QDateTime>
TransactionFilter::resolveDateRange(const FilterOptions &options) const
{
    // TODO: ThisMonth  → 이번 달 1일 00:00 ~ 말일 23:59
    // TODO: LastMonth  → 지난 달 1일 00:00 ~ 말일 23:59
    // TODO: Custom     → options.customFrom / customTo 그대로 사용
    Q_UNUSED(options)
    return { QDateTime(), QDateTime() };
}

bool TransactionFilter::matchesType(const Transaction &tx,
                                    const QList<TransactionType> &types) const
{
    // TODO: types 비어있으면 true (전체), 아니면 포함 여부
    Q_UNUSED(tx) Q_UNUSED(types)
    return true;
}

bool TransactionFilter::matchesStatus(const Transaction &tx,
                                      const QList<TransactionStatus> &statuses) const
{
    // TODO: statuses 비어있으면 true, 아니면 포함 여부
    Q_UNUSED(tx) Q_UNUSED(statuses)
    return true;
}

bool TransactionFilter::matchesDate(const Transaction &tx,
                                    const QDateTime &from, const QDateTime &to) const
{
    // TODO: tx.occurredAt 이 [from, to] 범위 내이면 true
    Q_UNUSED(tx) Q_UNUSED(from) Q_UNUSED(to)
    return true;
}

bool TransactionFilter::matchesMemo(const Transaction &tx, const QString &keyword) const
{
    // TODO: keyword 비어있으면 true
    // TODO: tx.memo.contains(keyword, Qt::CaseInsensitive)
    Q_UNUSED(tx) Q_UNUSED(keyword)
    return true;
}

// =======================================================================
// BalanceQuery
// =======================================================================

BalanceQuery::BalanceQuery(const AccountManager *manager) : m_manager(manager) {}

qint64 BalanceQuery::currentBalance(int accountId) const
{
    // TODO: m_manager->currentBalanceOf(accountId)
    Q_UNUSED(accountId)
    return 0;
}

qint64 BalanceQuery::totalBalance() const
{
    // TODO: m_manager->totalActiveBalance()
    return 0;
}

QList<BalanceSnapshot> BalanceQuery::balanceHistory(int accountId,
                                                    const QDateTime &from,
                                                    const QDateTime &to) const
{
    // TODO: 기간 내 해당 계좌 거래를 오래된 순 정렬
    // TODO: 초기잔고부터 시작해서 거래마다 누적 BalanceSnapshot 생성
    Q_UNUSED(accountId) Q_UNUSED(from) Q_UNUSED(to)
    return {};
}

QList<Account> BalanceQuery::accountSummaries() const
{
    // TODO: m_manager->activeAccounts() 반환
    return {};
}

// =======================================================================
// MainWindow
// =======================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_deposit(&m_accountManager)
    , m_withdraw(&m_accountManager)
    , m_transfer(&m_accountManager)
    , m_correction(&m_accountManager)
    , m_filter(&m_accountManager)
    , m_balanceQuery(&m_accountManager)
{
    ui->setupUi(this);

    // TODO: 초기 계좌 목록을 탭 위젯에 로드
    // TODO: refreshTransactionList(), refreshSummary() 호출
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ── 버튼 슬롯 ─────────────────────────────────────────────────────────────

void MainWindow::on_pushButton_calc_clicked()   // 입금
{
    // TODO: currentAccountId() 확인
    // TODO: UI에서 금액, 일시, 메모, 카테고리 읽기
    // TODO: m_deposit.deposit(...) 호출
    // TODO: 성공 → refreshTransactionList(), refreshSummary()
    // TODO: 실패 → showError(...)
}

void MainWindow::on_pushButton_save_clicked()   // 출금
{
    // TODO: currentAccountId() 확인
    // TODO: UI에서 금액, 일시, 메모 읽기
    // TODO: m_withdraw.withdraw(...) 호출
    // TODO: 성공 → refreshTransactionList(), refreshSummary()
    // TODO: 실패 → showError(...)
}

void MainWindow::on_pushButton_login_clicked()  // 송금
{
    // TODO: readTransferInput(toAccountId, amount, err) 호출
    // TODO: m_transfer.transfer(currentAccountId(), toAccountId, amount, ...) 호출
    // TODO: 성공 → refreshTransactionList(), refreshSummary()
    // TODO: 실패 → showError(...)
}

void MainWindow::on_pushButton_reset_clicked()  // 정정
{
    // TODO: 테이블에서 선택된 거래 ID 가져오기
    // TODO: CorrectionRequest 구성
    // TODO: m_correction.correctTransaction(txId, req) 호출
    // TODO: 성공 → refreshTransactionList(), refreshSummary()
    // TODO: 실패 → showError(...)
}

void MainWindow::on_pushButton_help_clicked()   // 내역 저장
{
    // TODO: QFileDialog 로 저장 경로 선택
    // TODO: 현재 거래 목록 CSV 등으로 내보내기
}

// ── 메뉴 슬롯 ─────────────────────────────────────────────────────────────

void MainWindow::on_actionAccount_View_triggered()  // 계좌 조회
{
    // TODO: m_balanceQuery.accountSummaries() 가져오기
    // TODO: 다이얼로그 또는 패널에 목록 표시
}

void MainWindow::on_actionAdd_triggered()           // 계좌 생성
{
    // TODO: 계좌 정보 입력 다이얼로그 (이름, 은행명, 초기 잔고, 정책)
    // TODO: m_accountManager.createAccount(...) 호출
    // TODO: 성공 → 탭 위젯에 새 탭 추가, refreshSummary()
    // TODO: 실패 → showError(...)
}

void MainWindow::on_actionDelete_triggered()        // 계좌 탈퇴
{
    // TODO: 현재 탭의 계좌 확인 + 취소 확인 다이얼로그
    // TODO: m_accountManager.deactivateAccount(currentAccountId()) 호출
    // TODO: 성공 → 탭 제거, refreshSummary()
    // TODO: 실패 → showError(...)
}

// ── 탭 변경 슬롯 ──────────────────────────────────────────────────────────

void MainWindow::on_tabWidget_semester_currentChanged(int index)
{
    // TODO: index → accountId 매핑 (탭-계좌 매핑 자료구조 필요)
    // TODO: refreshTransactionList(), refreshSummary()
    Q_UNUSED(index)
}

// ── UI 헬퍼 ───────────────────────────────────────────────────────────────

int MainWindow::currentAccountId() const
{
    // TODO: 현재 탭 index → accountId 변환
    // TODO: 탭 없으면 -1
    return -1;
}

void MainWindow::refreshTransactionList()
{
    // TODO: FilterOptions 구성 (현재 계좌 ID 등)
    // TODO: m_filter.query(options) 로 거래 목록 가져오기
    // TODO: 결과를 UI 행(QDateEdit, QComboBox 등)에 채워 넣기
}

void MainWindow::refreshSummary()
{
    // TODO: lineEdit_total_gpa    ← currentBalance(현재 계좌)
    // TODO: lineEdit_major_gpa   ← 전체 활성 잔고 합산 등 원하는 값
    // TODO: lineEdit_credits_earned, lineEdit_major_credits 업데이트
}

bool MainWindow::readTransferInput(int &toAccountId, qint64 &amount, QString &outError)
{
    // TODO: lineEdit_studentId → 계좌번호 읽기 → accountId 변환
    // TODO: lineEdit_password  → 금액 파싱
    // TODO: 유효하지 않으면 outError 세팅 후 false
    Q_UNUSED(toAccountId) Q_UNUSED(amount)
    outError = "Not implemented";
    return false;
}

void MainWindow::showError(const QString &msg)
{
    QMessageBox::warning(this, tr("오류"), msg);
}

void MainWindow::showInfo(const QString &msg)
{
    QMessageBox::information(this, tr("완료"), msg);
}

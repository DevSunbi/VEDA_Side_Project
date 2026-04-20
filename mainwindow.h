#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDateTime>
#include <QList>
#include <optional>
#include <utility>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// =======================================================================
// SECTION 1 : 모델 (Account, Transaction)
// =======================================================================

enum class AccountStatus {
    Active,     // 활성
    Inactive    // 비활성(보관)
};

enum class OverdraftPolicy {
    Deny,       // 잔고 부족 시 거래 불가
    Allow       // 마이너스 허용
};

struct Account {
    int             id;
    QString         name;
    QString         accountNumber;
    QString         bankName;
    qint64          initialBalance;
    QDateTime       createdAt;
    AccountStatus   status;
    OverdraftPolicy overdraftPolicy;
    qint64          currentBalance;  // AccountManager 가 채워줌

    Account()
        : id(0), initialBalance(0)
        , createdAt(QDateTime::currentDateTime())
        , status(AccountStatus::Active)
        , overdraftPolicy(OverdraftPolicy::Deny)
        , currentBalance(0)
    {}
};

enum class TransactionType {
    Deposit,      // 입금
    Withdraw,     // 출금
    TransferOut,  // 송금 출금 측
    TransferIn    // 송금 입금 측
};

enum class TransactionStatus {
    Posted,       // 정상
    Canceled      // 취소
};

struct Transaction {
    int               id;
    int               accountId;
    TransactionType   type;
    TransactionStatus status;
    qint64            amount;
    QDateTime         occurredAt;
    QString           memo;
    QString           category;
    int               transferGroupId;  // 0 이면 단독 거래

    Transaction()
        : id(0), accountId(0)
        , type(TransactionType::Deposit)
        , status(TransactionStatus::Posted)
        , amount(0)
        , occurredAt(QDateTime::currentDateTime())
        , transferGroupId(0)
    {}
};

// =======================================================================
// SECTION 2 : 헬퍼 구조체 (모듈 공용)
// =======================================================================

enum class DateRangePreset { ThisMonth, LastMonth, Custom };

struct FilterOptions {
    int                     accountId  = 0;
    QList<TransactionType>  types;
    QList<TransactionStatus> statuses  = { TransactionStatus::Posted };
    DateRangePreset         datePreset = DateRangePreset::ThisMonth;
    QDateTime               customFrom;
    QDateTime               customTo;
    QString                 memoKeyword;
    bool                    newestFirst = true;
};

struct BalanceSnapshot {
    QDateTime at;
    qint64    balance;
};

struct CorrectionRequest {
    qint64    newAmount     = 0;
    QDateTime newOccurredAt;
    QString   newMemo;
    QString   newCategory;
    bool changeAmount      = false;
    bool changeOccurredAt  = false;
    bool changeMemo        = false;
    bool changeCategory    = false;
};

// =======================================================================
// SECTION 3 : AccountManager
// =======================================================================

class AccountManager
{
public:
    // ── 계좌 CRUD ────────────────────────────────────────────────────────
    int  createAccount   (const Account &accountData, QString *outError = nullptr);
    bool updateAccount   (int accountId, const Account &updated, QString *outError = nullptr);
    bool deactivateAccount(int accountId, QString *outError = nullptr);
    bool reactivateAccount(int accountId, QString *outError = nullptr);

    // ── 계좌 조회 ────────────────────────────────────────────────────────
    std::optional<Account> findById       (int accountId) const;
    QList<Account>         activeAccounts () const;
    QList<Account>         allAccounts    () const;

    // ── 잔고 계산 ────────────────────────────────────────────────────────
    qint64 currentBalanceOf  (int accountId) const;
    qint64 totalActiveBalance() const;

    // ── 내부 데이터 접근 (모듈 공유) ─────────────────────────────────────
    QList<Transaction>       &transactions();
    const QList<Transaction> &transactions() const;
    int nextAccountId()     const;
    int nextTransactionId() const;

private:
    QList<Account>     m_accounts;
    QList<Transaction> m_transactions;
    int m_nextAccountId       = 1;
    int m_nextTransactionId   = 1;
    int m_nextTransferGroupId = 1;

    Account       *findAccount(int accountId);
    const Account *findAccount(int accountId) const;
    void recalcBalance   (Account &account) const;
    void recalcAllBalances();

    friend class DepositModule;
    friend class WithdrawModule;
    friend class TransferModule;
    friend class CorrectionModule;
};

// =======================================================================
// SECTION 4 : DepositModule
// =======================================================================

class DepositModule
{
public:
    explicit DepositModule(AccountManager *manager);

    int  deposit      (int accountId, qint64 amount,
                       const QDateTime &occurredAt,
                       const QString &memo     = QString(),
                       const QString &category = QString(),
                       QString *outError = nullptr);

    bool updateDeposit(int transactionId, qint64 newAmount,
                       const QDateTime &newOccurredAt,
                       const QString &newMemo, const QString &newCategory,
                       QString *outError = nullptr);

private:
    AccountManager *m_manager;
    bool validateAmount(qint64 amount, QString *outError) const;
};

// =======================================================================
// SECTION 5 : WithdrawModule
// =======================================================================

class WithdrawModule
{
public:
    explicit WithdrawModule(AccountManager *manager);

    int  withdraw      (int accountId, qint64 amount,
                        const QDateTime &occurredAt,
                        const QString &memo = QString(),
                        QString *outError = nullptr);

    bool updateWithdraw(int transactionId, qint64 newAmount,
                        const QDateTime &newOccurredAt,
                        const QString &newMemo,
                        QString *outError = nullptr);

private:
    AccountManager *m_manager;
    bool validateAmount  (qint64 amount, QString *outError) const;
    bool checkOverdraft  (int accountId, qint64 amount, QString *outError) const;
};

// =======================================================================
// SECTION 6 : TransferModule
// =======================================================================

class TransferModule
{
public:
    explicit TransferModule(AccountManager *manager);

    int transfer(int fromAccountId, int toAccountId, qint64 amount,
                 const QDateTime &occurredAt, const QString &memo = QString(),
                 QString *outError = nullptr);

private:
    AccountManager *m_manager;
    bool validateTransfer(int fromAccountId, int toAccountId, qint64 amount,
                          QString *outError) const;
};

// =======================================================================
// SECTION 7 : CorrectionModule
// =======================================================================

class CorrectionModule
{
public:
    explicit CorrectionModule(AccountManager *manager);

    bool cancelTransaction  (int transactionId, QString *outError = nullptr);
    bool correctTransaction (int transactionId, const CorrectionRequest &req,
                             QString *outError = nullptr);
    bool isCanceled         (int transactionId) const;

private:
    AccountManager *m_manager;
    int  findTransferPair  (int transactionId) const;
    bool markCanceled      (int transactionId, QString *outError);
    bool applyCorrection   (int transactionId, const CorrectionRequest &req,
                            QString *outError);
};

// =======================================================================
// SECTION 8 : TransactionFilter
// =======================================================================

class TransactionFilter
{
public:
    explicit TransactionFilter(const AccountManager *manager);

    QList<Transaction> query        (const FilterOptions &options) const;
    qint64             sumOf        (const QList<Transaction> &txs) const;
    qint64             totalDeposit (const QList<Transaction> &txs) const;
    qint64             totalWithdraw(const QList<Transaction> &txs) const;

private:
    const AccountManager *m_manager;
    std::pair<QDateTime, QDateTime> resolveDateRange(const FilterOptions &options) const;
    bool matchesType  (const Transaction &tx, const QList<TransactionType>   &types)    const;
    bool matchesStatus(const Transaction &tx, const QList<TransactionStatus> &statuses) const;
    bool matchesDate  (const Transaction &tx, const QDateTime &from, const QDateTime &to) const;
    bool matchesMemo  (const Transaction &tx, const QString &keyword) const;
};

// =======================================================================
// SECTION 9 : BalanceQuery
// =======================================================================

class BalanceQuery
{
public:
    explicit BalanceQuery(const AccountManager *manager);

    qint64               currentBalance  (int accountId) const;
    qint64               totalBalance    () const;
    QList<BalanceSnapshot> balanceHistory(int accountId,
                                          const QDateTime &from,
                                          const QDateTime &to) const;
    QList<Account>       accountSummaries() const;

private:
    const AccountManager *m_manager;
};

// =======================================================================
// SECTION 10 : MainWindow
// =======================================================================

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // ── 버튼 슬롯 ────────────────────────────────────────────────────────
    void on_pushButton_calc_clicked();      // 입금
    void on_pushButton_save_clicked();      // 출금
    void on_pushButton_login_clicked();     // 송금
    void on_pushButton_reset_clicked();     // 정정
    void on_pushButton_help_clicked();      // 내역 저장

    // ── 메뉴 슬롯 ────────────────────────────────────────────────────────
    void on_actionAccount_View_triggered(); // 계좌 조회
    void on_actionAdd_triggered();          // 계좌 생성
    void on_actionDelete_triggered();       // 계좌 탈퇴

    // ── 탭 변경 슬롯 ─────────────────────────────────────────────────────
    void on_tabWidget_semester_currentChanged(int index);

private:
    Ui::MainWindow *ui;

    // ── 핵심 매니저 ──────────────────────────────────────────────────────
    AccountManager   m_accountManager;

    // ── 기능 모듈 ────────────────────────────────────────────────────────
    DepositModule    m_deposit;
    WithdrawModule   m_withdraw;
    TransferModule   m_transfer;
    CorrectionModule m_correction;
    TransactionFilter m_filter;
    BalanceQuery     m_balanceQuery;

    // ── UI 헬퍼 ──────────────────────────────────────────────────────────
    int  currentAccountId() const;
    void refreshTransactionList();
    void refreshSummary();
    bool readTransferInput(int &toAccountId, qint64 &amount, QString &outError);
    void showError(const QString &msg);
    void showInfo (const QString &msg);
};

#endif // MAINWINDOW_H

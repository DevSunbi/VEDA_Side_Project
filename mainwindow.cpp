#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include <algorithm>

// =======================================================================
// AccountManager
// =======================================================================

int AccountManager::createAccount(const Account &accountData, QString *outError)
{
    Q_UNUSED(accountData)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return -1;
}

bool AccountManager::updateAccount(int accountId, const Account &updated, QString *outError)
{
    Q_UNUSED(accountId)
    Q_UNUSED(updated)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

bool AccountManager::deactivateAccount(int accountId, QString *outError)
{
    Q_UNUSED(accountId)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

bool AccountManager::reactivateAccount(int accountId, QString *outError)
{
    Q_UNUSED(accountId)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

std::optional<Account> AccountManager::findById(int accountId) const
{
    Q_UNUSED(accountId)
    return std::nullopt;
}

QList<Account> AccountManager::activeAccounts() const
{
    return {};
}

QList<Account> AccountManager::allAccounts() const
{
    return {};
}

qint64 AccountManager::currentBalanceOf(int accountId) const
{
    Q_UNUSED(accountId)
    return 0;
}

qint64 AccountManager::totalActiveBalance() const
{
    return 0;
}

QList<Transaction> &AccountManager::transactions()
{
    return m_transactions;
}

const QList<Transaction> &AccountManager::transactions() const
{
    return m_transactions;
}

int AccountManager::nextAccountId() const
{
    return m_nextAccountId;
}

int AccountManager::nextTransactionId() const
{
    return m_nextTransactionId;
}

Account *AccountManager::findAccount(int accountId)
{
    Q_UNUSED(accountId)
    return nullptr;
}

const Account *AccountManager::findAccount(int accountId) const
{
    Q_UNUSED(accountId)
    return nullptr;
}

void AccountManager::recalcBalance(Account &account)
{
    Q_UNUSED(account)
}

void AccountManager::recalcAllBalances()
{
}

QList<BalanceSnapshot> AccountManager::balanceHistory(int accountId, const QDateTime &from, const QDateTime &to) const
{
    Q_UNUSED(accountId)
    Q_UNUSED(from)
    Q_UNUSED(to)
    return {};
}

bool AccountManager::validateAmount(qint64 amount, QString *outError)
{
    Q_UNUSED(amount)
    if (outError)
        *outError = "Not implemented";
    return false;
}

// =======================================================================
// DepositModule
// =======================================================================

DepositModule::DepositModule(AccountManager *manager)
    : m_manager(manager)
{
}

int DepositModule::deposit(int accountId, qint64 amount, const QDateTime &occurredAt,
                           const QString &memo, const QString &category, QString *outError)
{
    Q_UNUSED(accountId)
    Q_UNUSED(amount)
    Q_UNUSED(occurredAt)
    Q_UNUSED(memo)
    Q_UNUSED(category)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return -1;
}



// =======================================================================
// WithdrawModule
// =======================================================================

WithdrawModule::WithdrawModule(AccountManager *manager)
    : m_manager(manager)
{
}

int WithdrawModule::withdraw(int accountId, qint64 amount, const QDateTime &occurredAt,
                             const QString &memo, QString *outError)
{
    Q_UNUSED(accountId)
    Q_UNUSED(amount)
    Q_UNUSED(occurredAt)
    Q_UNUSED(memo)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return -1;
}



bool WithdrawModule::checkOverdraft(int accountId, qint64 amount, QString *outError) const
{
    Q_UNUSED(accountId)
    Q_UNUSED(amount)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

// =======================================================================
// TransferModule
// =======================================================================

TransferModule::TransferModule(AccountManager *manager)
    : m_manager(manager)
{
}

int TransferModule::transfer(int fromAccountId, int toAccountId, qint64 amount,
                             const QDateTime &occurredAt, const QString &memo, QString *outError)
{
    Q_UNUSED(fromAccountId)
    Q_UNUSED(toAccountId)
    Q_UNUSED(amount)
    Q_UNUSED(occurredAt)
    Q_UNUSED(memo)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return -1;
}

bool TransferModule::validateTransfer(int fromAccountId, int toAccountId, qint64 amount, QString *outError) const
{
    Q_UNUSED(fromAccountId)
    Q_UNUSED(toAccountId)
    Q_UNUSED(amount)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

// =======================================================================
// CorrectionModule
// =======================================================================

CorrectionModule::CorrectionModule(AccountManager *manager)
    : m_manager(manager)
{
}

bool CorrectionModule::cancelTransaction(int transactionId, QString *outError)
{
    Q_UNUSED(transactionId)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

bool CorrectionModule::correctTransaction(int transactionId, const CorrectionRequest &req, QString *outError)
{
    Q_UNUSED(transactionId)
    Q_UNUSED(req)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

bool CorrectionModule::isCanceled(int transactionId) const
{
    Q_UNUSED(transactionId)
    return false;
}

int CorrectionModule::findTransferPair(int transactionId) const
{
    Q_UNUSED(transactionId)
    return -1;
}

bool CorrectionModule::markCanceled(int transactionId, QString *outError)
{
    Q_UNUSED(transactionId)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

bool CorrectionModule::applyCorrection(int transactionId, const CorrectionRequest &req, QString *outError)
{
    Q_UNUSED(transactionId)
    Q_UNUSED(req)
    if (outError)
    {
        *outError = "Not implemented";
    }
    return false;
}

// =======================================================================
// TransactionFilter
// =======================================================================

TransactionFilter::TransactionFilter(const AccountManager *manager)
    : m_manager(manager)
{
}

QList<Transaction> TransactionFilter::query(const FilterOptions &options) const
{
    Q_UNUSED(options)
    return {};
}

qint64 TransactionFilter::sumOf(const QList<Transaction> &txs) const
{
    Q_UNUSED(txs)
    return 0;
}

qint64 TransactionFilter::totalDeposit(const QList<Transaction> &txs) const
{
    Q_UNUSED(txs)
    return 0;
}

qint64 TransactionFilter::totalWithdraw(const QList<Transaction> &txs) const
{
    Q_UNUSED(txs)
    return 0;
}

std::pair<QDateTime, QDateTime> TransactionFilter::resolveDateRange(const FilterOptions &options) const
{
    Q_UNUSED(options)
    return {QDateTime(), QDateTime()};
}

bool TransactionFilter::matchesType(const Transaction &tx, const QList<TransactionType> &types) const
{
    Q_UNUSED(tx)
    Q_UNUSED(types)
    return true;
}

bool TransactionFilter::matchesStatus(const Transaction &tx, const QList<TransactionStatus> &statuses) const
{
    Q_UNUSED(tx)
    Q_UNUSED(statuses)
    return true;
}

bool TransactionFilter::matchesDate(const Transaction &tx, const QDateTime &from, const QDateTime &to) const
{
    Q_UNUSED(tx)
    Q_UNUSED(from)
    Q_UNUSED(to)
    return true;
}

bool TransactionFilter::matchesMemo(const Transaction &tx, const QString &keyword) const
{
    Q_UNUSED(tx)
    Q_UNUSED(keyword)
    return true;
}

// =======================================================================
// MainWindow
// =======================================================================

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_accountManager()
    , m_deposit(&m_accountManager)
    , m_withdraw(&m_accountManager)
    , m_transfer(&m_accountManager)
    , m_correction(&m_accountManager)
    , m_filter(&m_accountManager)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_calc_clicked()
{
}

void MainWindow::on_pushButton_save_clicked()
{
}

void MainWindow::on_pushButton_login_clicked()
{
}

void MainWindow::on_pushButton_reset_clicked()
{
}

void MainWindow::on_pushButton_help_clicked()
{
}

void MainWindow::on_actionAccount_View_triggered()
{
}

void MainWindow::on_actionAdd_triggered()
{
}

void MainWindow::on_actionDelete_triggered()
{
}

void MainWindow::on_tabWidget_semester_currentChanged(int index)
{
    Q_UNUSED(index)
}

int MainWindow::currentAccountId() const
{
    return -1;
}

void MainWindow::refreshTransactionList()
{
}

void MainWindow::refreshSummary()
{
}

bool MainWindow::readTransferInput(int &toAccountId, qint64 &amount, QString &outError)
{
    Q_UNUSED(toAccountId)
    Q_UNUSED(amount)
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

#include "account.h"
#include <QLocale>

// =======================================================================
// Account 구조체 구현 (생성자)
// =======================================================================
Account::Account() 
    : id(0), initialBalance(0), status("활성"), allowOverdraft(false), currentBalance(0), password("")
{
    createdAt = QDateTime::currentDateTime();
}

// =======================================================================
// AccountModel 클래스 구현
// =======================================================================
AccountModel::AccountModel(QObject *parent) : QAbstractTableModel(parent) {}

int AccountModel::rowCount(const QModelIndex &parent) const { 
    Q_UNUSED(parent)
    return m_accounts.size(); 
}

int AccountModel::columnCount(const QModelIndex &parent) const { 
    Q_UNUSED(parent)
    return ColumnCount; // 계좌명, 계좌번호, 은행, 잔고
}

QVariant AccountModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_accounts.size()) return QVariant();
    const auto &acc = m_accounts[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case NameColumn: return acc.name;
            case NumberColumn: return acc.accountNumber;
            case BankColumn: return acc.bankName;
            case BalanceColumn: return acc.currentBalance;
        }
    }
    return QVariant();
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case NameColumn: return "계좌명";
        case NumberColumn: return "계좌번호";
        case BankColumn: return "은행";
        case BalanceColumn: return "잔고";
    }
    return QVariant();
}

void AccountModel::addAccount(const Account &acc) {
    // 모델에 새로운 행이 추가됨을 알림
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(acc);
    endInsertRows();
}

QList<Account>& AccountModel::accounts(){
    return m_accounts;
}

void AccountModel::updateAll() {
    if (m_accounts.isEmpty()) return;
    emit dataChanged(index(0, 0), index(m_accounts.size()-1, columnCount()-1));
}

void AccountModel::removeAccount(int row)
{
    if(row<0||row>=m_accounts.size()) return;
    beginRemoveRows(QModelIndex(), row, row);
    m_accounts.removeAt(row);
    endRemoveRows();
}
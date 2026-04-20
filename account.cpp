#include "account.h"

Account::Account() 
    : id(0), initialBalance(0), status("활성"), allowOverdraft(false), currentBalance(0) 
{
    createdAt = QDateTime::currentDateTime();
}

AccountModel::AccountModel(QObject *parent) : QAbstractTableModel(parent) {}

int AccountModel::rowCount(const QModelIndex &parent) const { return m_accounts.size(); }
int AccountModel::columnCount(const QModelIndex &parent) const { return 4; }

QVariant AccountModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_accounts.size()) return QVariant();
    const auto &acc = m_accounts[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return acc.name;
            case 1: return acc.accountNumber;
            case 2: return acc.bankName;
            case 3: return acc.currentBalance;
        }
    }
    return QVariant();
}

QVariant AccountModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case 0: return "계좌명";
        case 1: return "계좌번호";
        case 2: return "은행";
        case 3: return "잔고";
    }
    return QVariant();
}

void AccountModel::addAccount(const Account &acc) {
    beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
    m_accounts.append(acc);
    endInsertRows();
}

QList<Account>& AccountModel::accounts() { return m_accounts; }
const QList<Account>& AccountModel::accounts() const { return m_accounts; }

void AccountModel::updateAll() {
    if (m_accounts.isEmpty()) return;
    emit dataChanged(index(0, 0), index(m_accounts.size()-1, columnCount()-1));
}

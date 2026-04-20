#include "transaction.h"

Transaction::Transaction() 
    : id(0), accountId(0), amount(0), type("입금"), status("정상") 
{
    occurredAt = QDateTime::currentDateTime();
}

TransactionModel::TransactionModel(QObject *parent) : QAbstractTableModel(parent) {}

int TransactionModel::rowCount(const QModelIndex &parent) const { return m_transactions.size(); }
int TransactionModel::columnCount(const QModelIndex &parent) const { return 6; }

QVariant TransactionModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= m_transactions.size()) return QVariant();
    const auto &tx = m_transactions[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case 0: return tx.occurredAt.toString("yyyy-MM-dd HH:mm");
            case 1: return tx.type;
            case 2: return tx.amount;
            case 3: return tx.category;
            case 4: return tx.memo;
            case 5: return tx.status;
        }
    }
    if (role == Qt::ForegroundRole && tx.status == "취소") return QColor(Qt::red);
    return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
    switch (section) {
        case 0: return "일시";
        case 1: return "구분";
        case 2: return "금액";
        case 3: return "카테고리";
        case 4: return "메모";
        case 5: return "상태";
    }
    return QVariant();
}

void TransactionModel::addTransaction(const Transaction &tx) {
    beginInsertRows(QModelIndex(), m_transactions.size(), m_transactions.size());
    m_transactions.append(tx);
    endInsertRows();
}

const QList<Transaction>& TransactionModel::transactions() const { return m_transactions; }

void TransactionModel::setTransactions(const QList<Transaction> &list) {
    beginResetModel();
    m_transactions = list;
    endResetModel();
}

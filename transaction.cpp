#include "transaction.h"

// [생성자] Transaction::Transaction()

Transaction::Transaction()
    : id(-1)
    , accountId(-1)
    , amount(0)
    , occurredAt(QDateTime::currentDateTime())
    , memo("")
    , type(TransactionType::Deposit)
    , status(TransactionStatus::Posted)
, transferGroupId(-1)
{
}
// 생성자

TransactionModel::TransactionModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

// 행 개수 반환 : 거래 내역 리스트 크기
int TransactionModel::rowCount(const QModelIndex &parent) const
{
    return m_transactions.size();
}

// 열 개수 반환
//   0: 일시 / 1: 구분 / 2: 금액 / 3: 메모 / 4: 상태
int TransactionModel::columnCount(const QModelIndex &parent) const
{
    return 5;
}

// 각 셀의 데이터 반환
QVariant TransactionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_transactions.size())
        return QVariant();

    const auto &tx = m_transactions[index.row()];

    if (role == Qt::DisplayRole) {
        switch (index.column()) {
        case 0: return tx.occurredAt.toString("yyyy-MM-dd HH:mm");
        case 1:
            switch (tx.type) {
            case TransactionType::Deposit:     return "입금";
            case TransactionType::Withdraw:    return "출금";
            case TransactionType::TransferOut: return "송금(출)";
            case TransactionType::TransferIn:  return "송금(입)";
            }
        case 2: return tx.amount;
        case 3: return tx.memo;
        case 4: return tx.status == TransactionStatus::Posted ? "정상" : "취소";
        }
    }

    // 취소된 거래는 빨간색으로 표시
    if (role == Qt::ForegroundRole && tx.status == TransactionStatus::Canceled)
        return QColor(Qt::red);

    return QVariant();
}

// 테이블 헤더 텍스트 반환
// category 제거로 헤더도 5개로 변경
QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
    case 0: return "일시";
    case 1: return "구분";
    case 2: return "금액";
    case 3: return "메모";
    case 4: return "상태";
    }

    return QVariant();
}

// 거래 추가
void TransactionModel::addTransaction(const Transaction &tx)
{
    beginInsertRows(QModelIndex(), m_transactions.size(), m_transactions.size());
    m_transactions.append(tx);
    endInsertRows();
}

// 전체 거래 목록 반환
const QList<Transaction>& TransactionModel::transactions() const
{
    return m_transactions;
}

// 거래 목록 전체 교체
void TransactionModel::setTransactions(const QList<Transaction> &list)
{
    beginResetModel();
    m_transactions = list;
    endResetModel();
}
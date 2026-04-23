#include "transaction.h"

Transaction::Transaction()
    :id(-1)
    ,accountId(-1)
    ,amount(0)
    ,occurredAt(QDateTime::currentDateTime())
    ,memo("")
    ,type(TransactionType::Deposit)
    ,status(TransactionStatus::Posted)
    ,transferGroupId(-1)
    ,counterpartyAccount("")
{
}
// 생성자
TransactionModel::TransactionModel(QObject *parent)
    : QAbstractTableModel(parent){}

// 행 개수 반환 : 거래 내역 리스트 크기
int TransactionModel::rowCount(const QModelIndex &parent) const
{
    return m_transactions.size();
}

// 열 개수 반환
// 0: 일시 / 1: 구분 / 2: 거래 계좌 / 3: 금액 / 4: 메모 / 5: 상태
int TransactionModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 6;
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
        case 2: return tx.counterpartyAccount.isEmpty() ? "-" : tx.counterpartyAccount;
        case 3: return tx.amount;
        case 4: return tx.memo;
        case 5: return tx.status == TransactionStatus::Posted ? "정상" : "취소";
        }
    }

    if (role == Qt::ForegroundRole) {
        // 1. 취소된 거래는 전체 회색 처리
        if (tx.status == TransactionStatus::Canceled) return QColor("#ADB5BD");

        // 2. '금액' 컬럼(3번)만 포인트 색상 적용
        if (index.column() == 3) {
            if (tx.type == TransactionType::Deposit || tx.type == TransactionType::TransferIn)
                return QColor("#3182F6"); // Blue (입금)
            else
                return QColor("#F04452"); // Red (출금)
        }
        
        // 3. Default : Black
        return QColor("#191F28");
    }

    return QVariant();
}

// 테이블 헤더 텍스트 반환
QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
        return QVariant();

    switch (section) {
    case 0: return "일시";
    case 1: return "구분";
    case 2: return "거래 대상";
    case 3: return "금액";
    case 4: return "메모";
    case 5: return "상태";
    }

    return QVariant();
}

// 거래 내역 테이블에서 메모(4번 열)만 편집 가능하도록 허용
Qt::ItemFlags TransactionModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) return Qt::NoItemFlags;
    Qt::ItemFlags defaultFlags = QAbstractTableModel::flags(index);
    if (index.column() == 4) return defaultFlags | Qt::ItemIsEditable; // 메모 편집
    return defaultFlags;
}

// 메모(4번 열) 편집 시 실제 데이터에 반영하고 뷰에 변경을 알림
bool TransactionModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.isValid() && role == Qt::EditRole) {
        if (index.column() == 4) {
            m_transactions[index.row()].memo = value.toString();
            emit dataChanged(index, index);
            return true;
        }
    }
    return false;
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

// 대상 계좌의 거래 이력 일괄 삭제 (좀비 데이터 방지용)
void TransactionModel::removeTransactionsByAccountId(int accountId)
{
    for (int i = m_transactions.size() - 1; i >= 0; --i) {
        if (m_transactions[i].accountId == accountId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_transactions.removeAt(i);
            endRemoveRows();
        }
    }
}
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDateTime>
#include <QAbstractTableModel>
#include <QList>
#include <QColor>

// =======================================================================
// 거래 내역 데이터 구조체 (초보자형 직관적 변수명)
// =======================================================================
struct Transaction {
    int         id;
    int         accountId;
    qint64      amount;
    QDateTime   occurredAt;
    QString     memo;
    QString     category;

    // 상태값들 (enum 대신 글자로 직접 관리)
    QString     type;   // "입금", "출금", "송금"
    QString     status; // "정상", "취소"

    Transaction() : id(0), accountId(0), amount(0), type("입금"), status("정상") {
        occurredAt = QDateTime::currentDateTime();
    }
};

// =======================================================================
// 거래 내역 모델 (표/리스트 뷰에 데이터를 그려주는 역할)
// =======================================================================
class TransactionModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit TransactionModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 모델에 데이터를 추가하거나 바꾸는 함수들
    void addTransaction(const Transaction &tx);
    const QList<Transaction>& transactions() const;
    void setTransactions(const QList<Transaction> &list);

private:
    QList<Transaction> m_transactions; // 모든 거래 내역이 저장되는 리스트
};

#endif // TRANSACTION_H

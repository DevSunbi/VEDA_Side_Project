#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <QString>
#include <QDateTime>
#include <QAbstractTableModel>
#include <QList>
#include <QColor>

// 거래 유형을 나타내는 열거형
enum class TransactionType {
    Deposit,    // 입금
    Withdraw,   // 출금
    TransferOut,    // 송금 출금 (내 계좌에서 나가는 쪽)
    TransferIn      // 송금 입금 (상대 계좌로 들어오는 쪽)
};

// 거래 상태를 나타내는 열거형
// 거래를 삭제하지 않고 상태로 무효화하기 위해 사용
enum class TransactionStatus {
    Posted,     // 정상 처리된 거래
    Canceled    // 취소된 거래 (잔고 롤백 대상)
};

// 하나의 거래(입금/출금)를 표현하는 데이터 구조체

struct Transaction {
    int               id;           // 거래 고유 ID
    int               accountId;    // 연결된 계좌 ID
    qint64            amount;       // 금액 (항상 양수, 단위: 원)
    QDateTime         occurredAt;   // 거래 발생 일시
    QString           memo;         // 메모 (선택)
    TransactionType   type;         // 거래 유형 (입금/출금)
    TransactionStatus status;       // 거래 상태 (정상/취소)
    int transferGroupId;  // 송금 쌍 묶음 ID (-1이면 해당 없음)
    QString counterpartyAccount;    // 거래 상대방 계좌 (송금 시)

    // 기본 생성자 : 안전한 초기값으로 설정
    Transaction();
};


// 거래 내역을 테이블 뷰에 표시하기 위한 모델 클래스

class TransactionModel : public QAbstractTableModel {
    Q_OBJECT

public:
    explicit TransactionModel(QObject *parent = nullptr);

    // QAbstractTableModel 구현
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // 테이블 수정용 오버라이드
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // 모델 데이터 조작 함수

    void addTransaction(const Transaction &tx); // 거래 추가
    const QList<Transaction>& transactions() const;  // 전체 거래 목록 반환
    void setTransactions(const QList<Transaction> &list); // 거래 목록 교체
    void removeTransactionsByAccountId(int accountId); // 대상 계좌 거래내역 일괄 삭제

private:
    QList<Transaction> m_transactions;  // 거래 내역 리스트
};

#endif // TRANSACTION_H
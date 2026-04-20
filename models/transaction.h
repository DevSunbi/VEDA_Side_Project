#pragma once

#include <QString>
#include <QDateTime>

// 거래 유형
enum class TransactionType {
    Deposit,    // 입금
    Withdraw,   // 출금
    TransferOut,// 송금 출금 측
    TransferIn  // 송금 입금 측
};

// 거래 상태
enum class TransactionStatus {
    Posted,     // 정상
    Canceled    // 취소
};

struct Transaction {
    int         id;             // 내부 고유 ID (자동 증가)
    int         accountId;      // 연결된 계좌 ID
    TransactionType   type;     // 입금 / 출금 / 송금(출) / 송금(입)
    TransactionStatus status;   // posted / canceled
    qint64      amount;         // 금액 (항상 양수, 방향은 type 으로 결정)
    QDateTime   occurredAt;     // 거래 일시
    QString     memo;           // 메모 (옵션)
    QString     category;       // 입금 사유/구분 (옵션, Deposit 전용)

    // 송금 연결 — TransferOut/TransferIn 쌍을 묶는 그룹 ID
    // 0 이면 단독 거래
    int         transferGroupId;

    Transaction()
        : id(0)
        , accountId(0)
        , type(TransactionType::Deposit)
        , status(TransactionStatus::Posted)
        , amount(0)
        , occurredAt(QDateTime::currentDateTime())
        , transferGroupId(0)
    {}
};

#pragma once

#include "../models/transaction.h"
#include <QString>

class AccountManager;

// -----------------------------------------------------------------------
// DepositModule
// 역할 : 입금 거래 등록 / 수정 (취소는 CorrectionModule)
// -----------------------------------------------------------------------
class DepositModule
{
public:
    explicit DepositModule(AccountManager *manager);

    // 입금 등록
    // - amount    : 양의 정수, 0이면 오류
    // - occurredAt: 거래 일시 (기본값 현재)
    // - memo      : 메모 (옵션)
    // - category  : 입금 사유/구분 (옵션)
    // 성공 시 생성된 Transaction.id 반환, 실패 시 -1
    int deposit(int accountId,
                qint64 amount,
                const QDateTime &occurredAt,
                const QString &memo = QString(),
                const QString &category = QString(),
                QString *outError = nullptr);

    // 입금 거래 수정 (amount, occurredAt, memo, category 변경)
    // 취소된 거래는 수정 불가
    bool updateDeposit(int transactionId,
                       qint64 newAmount,
                       const QDateTime &newOccurredAt,
                       const QString &newMemo,
                       const QString &newCategory,
                       QString *outError = nullptr);

private:
    AccountManager *m_manager;

    // 금액 유효성 검증 (> 0, 정수)
    bool validateAmount(qint64 amount, QString *outError) const;
};

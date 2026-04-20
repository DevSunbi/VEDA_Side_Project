#pragma once

#include "../models/transaction.h"
#include <QString>
#include <QDateTime>

class AccountManager;

// -----------------------------------------------------------------------
// WithdrawModule
// 역할 : 출금 거래 등록 / 수정, 잔고 부족 정책 적용
// -----------------------------------------------------------------------
class WithdrawModule
{
public:
    explicit WithdrawModule(AccountManager *manager);

    // 출금 등록
    // - amount     : 양의 정수
    // - occurredAt : 거래 일시
    // - memo       : 메모 (옵션)
    // 잔고 부족 정책은 account.overdraftPolicy 를 따름:
    //   Deny  → 잔고 부족 시 오류 반환
    //   Allow → 마이너스 허용
    // 성공 시 Transaction.id 반환, 실패 시 -1
    int withdraw(int accountId,
                 qint64 amount,
                 const QDateTime &occurredAt,
                 const QString &memo = QString(),
                 QString *outError = nullptr);

    // 출금 거래 수정
    // 취소된 거래는 수정 불가
    // 수정 후에도 정책 위반이면 실패
    bool updateWithdraw(int transactionId,
                        qint64 newAmount,
                        const QDateTime &newOccurredAt,
                        const QString &newMemo,
                        QString *outError = nullptr);

private:
    AccountManager *m_manager;

    bool validateAmount(qint64 amount, QString *outError) const;

    // 출금 후 예상 잔고 계산 후 정책 확인
    // currentBalance - amount 가 음수이고 policy == Deny 면 false
    bool checkOverdraft(int accountId,
                        qint64 amount,
                        QString *outError) const;
};

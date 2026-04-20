#pragma once

#include "../models/account.h"
#include <QList>
#include <QString>

class AccountManager;

// -----------------------------------------------------------------------
// BalanceQuery
// 역할 : 계좌별/전체 잔고 조회, 잔고 변동 흐름 제공
// -----------------------------------------------------------------------

// 잔고 스냅샷 (특정 시점 이후의 흐름용)
struct BalanceSnapshot {
    QDateTime   at;
    qint64      balance;    // 해당 시점 이후 누적 잔고
};

class BalanceQuery
{
public:
    explicit BalanceQuery(const AccountManager *manager);

    // 특정 계좌 현재 잔고
    qint64 currentBalance(int accountId) const;

    // 전체 활성 계좌 잔고 합산
    qint64 totalBalance() const;

    // 특정 계좌의 잔고 시계열 (기간 내 거래 순서대로 누적)
    // 반환값: 각 거래 직후 잔고 스냅샷 리스트 (오래된 순)
    QList<BalanceSnapshot> balanceHistory(int accountId,
                                          const QDateTime &from,
                                          const QDateTime &to) const;

    // 모든 활성 계좌 요약 정보 반환
    QList<Account> accountSummaries() const;

private:
    const AccountManager *m_manager;
};

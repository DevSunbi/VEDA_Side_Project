#pragma once

#include "../models/transaction.h"
#include <QList>
#include <QDateTime>
#include <QString>
#include <optional>

class AccountManager;

// -----------------------------------------------------------------------
// TransactionFilter
// 역할 : 거래 내역 조회 / 필터 / 정렬
// -----------------------------------------------------------------------

// 기간 프리셋
enum class DateRangePreset {
    ThisMonth,      // 이번 달
    LastMonth,      // 지난 달
    Custom          // 사용자 지정
};

struct FilterOptions {
    // 계좌 필터 (0이면 전체)
    int accountId = 0;

    // 유형 필터 (비어 있으면 전체)
    QList<TransactionType> types;

    // 상태 필터 (비어 있으면 posted 만)
    QList<TransactionStatus> statuses = { TransactionStatus::Posted };

    // 기간 필터
    DateRangePreset datePreset = DateRangePreset::ThisMonth;
    QDateTime customFrom;   // preset == Custom 일 때만 사용
    QDateTime customTo;     // preset == Custom 일 때만 사용

    // 메모 검색어 (대소문자 무시)
    QString memoKeyword;

    // 정렬: true = 최신순(기본), false = 오래된순
    bool newestFirst = true;
};

class TransactionFilter
{
public:
    explicit TransactionFilter(const AccountManager *manager);

    // 필터 조건에 맞는 거래 목록 반환
    QList<Transaction> query(const FilterOptions &options) const;

    // 조회 결과 합계 (입금 - 출금 기준 순합)
    qint64 sumOf(const QList<Transaction> &transactions) const;

    // 조회 결과 총 입금액
    qint64 totalDeposit(const QList<Transaction> &transactions) const;

    // 조회 결과 총 출금액
    qint64 totalWithdraw(const QList<Transaction> &transactions) const;

private:
    const AccountManager *m_manager;

    // 기간 프리셋을 [from, to] 범위로 변환
    std::pair<QDateTime, QDateTime> resolveDateRange(const FilterOptions &options) const;

    bool matchesType(const Transaction &tx, const QList<TransactionType> &types) const;
    bool matchesStatus(const Transaction &tx, const QList<TransactionStatus> &statuses) const;
    bool matchesDate(const Transaction &tx, const QDateTime &from, const QDateTime &to) const;
    bool matchesMemo(const Transaction &tx, const QString &keyword) const;
};

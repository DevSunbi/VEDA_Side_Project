#pragma once

#include <QString>
#include <QDateTime>

// 계좌 상태
enum class AccountStatus {
    Active,     // 활성
    Inactive    // 비활성(보관)
};

// 잔고 부족 정책
enum class OverdraftPolicy {
    Deny,       // A: 잔고 부족 시 저장 불가
    Allow       // B: 마이너스 허용
};

struct Account {
    int         id;             // 내부 고유 ID (자동 증가)
    QString     name;           // 계좌명
    QString     accountNumber;  // 계좌 번호 (옵션)
    QString     bankName;       // 은행명 (옵션)
    qint64      initialBalance; // 초기 잔고 (원 단위 정수)
    QDateTime   createdAt;      // 생성일
    AccountStatus   status;     // 활성 / 비활성
    OverdraftPolicy overdraftPolicy; // 잔고 부족 정책

    // 계산된 현재 잔고 (AccountManager 가 채워준다)
    qint64      currentBalance; // 초기잔고 + 입금합 - 출금합

    Account()
        : id(0)
        , initialBalance(0)
        , createdAt(QDateTime::currentDateTime())
        , status(AccountStatus::Active)
        , overdraftPolicy(OverdraftPolicy::Deny)
        , currentBalance(0)
    {}
};

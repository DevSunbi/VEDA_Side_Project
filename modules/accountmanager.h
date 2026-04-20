#pragma once

#include "../models/account.h"
#include "../models/transaction.h"
#include <QList>
#include <QString>
#include <optional>

// -----------------------------------------------------------------------
// AccountManager
// 역할 : 계좌 생성/수정/비활성, 잔고 계산, 계좌 목록 제공
// -----------------------------------------------------------------------
class AccountManager
{
public:
    // --- 계좌 CRUD ---------------------------------------------------

    // 새 계좌 생성. 성공 시 생성된 account.id(>0) 반환, 실패 시 -1
    // 오류 원인은 outError 에 기록
    int createAccount(const Account &accountData, QString *outError = nullptr);

    // 계좌 수정 (name, accountNumber, bankName, overdraftPolicy 변경 가능)
    // initialBalance 변경은 수정 불가 (취소/재생성 필요)
    bool updateAccount(int accountId, const Account &updated, QString *outError = nullptr);

    // 계좌 비활성화 (soft-delete)
    bool deactivateAccount(int accountId, QString *outError = nullptr);

    // 계좌 재활성화
    bool reactivateAccount(int accountId, QString *outError = nullptr);

    // --- 계좌 조회 ---------------------------------------------------

    // ID 로 단일 계좌 조회 (없으면 nullopt)
    std::optional<Account> findById(int accountId) const;

    // 전체 활성 계좌 목록 (currentBalance 포함)
    QList<Account> activeAccounts() const;

    // 비활성 포함 전체 목록
    QList<Account> allAccounts() const;

    // --- 잔고 계산 ---------------------------------------------------

    // 특정 계좌 현재 잔고
    qint64 currentBalanceOf(int accountId) const;

    // 전체 활성 계좌 잔고 합산
    qint64 totalActiveBalance() const;

    // --- 내부 데이터 (모듈들이 공유) ---------------------------------

    // 거래 목록 참조 (TransactionFilter, CorrectionModule 등에서 사용)
    QList<Transaction>& transactions();
    const QList<Transaction>& transactions() const;

    // 다음 자동 증가 ID (내부 사용)
    int nextAccountId() const;
    int nextTransactionId() const;

private:
    QList<Account>     m_accounts;
    QList<Transaction> m_transactions;
    int m_nextAccountId     = 1;
    int m_nextTransactionId = 1;
    int m_nextTransferGroupId = 1;

    // 내부 헬퍼: 계좌 포인터 반환 (없으면 nullptr)
    Account*       findAccount(int accountId);
    const Account* findAccount(int accountId) const;

    // 거래 목록 기준으로 계좌 잔고를 재계산하여 account.currentBalance 갱신
    void recalcBalance(Account &account) const;

    // 잔고 재계산 후 전체 계좌 목록 동기화
    void recalcAllBalances();

    friend class DepositModule;
    friend class WithdrawModule;
    friend class TransferModule;
    friend class CorrectionModule;
};

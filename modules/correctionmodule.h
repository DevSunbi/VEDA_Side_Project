#pragma once

#include "../models/transaction.h"
#include <QString>
#include <QDateTime>

class AccountManager;

// -----------------------------------------------------------------------
// CorrectionModule
// 역할 : 거래 취소(상태 변경 + 잔고 롤백) / 수정(금액·일시·메모 변경 + 잔고 재반영)
// -----------------------------------------------------------------------

// 거래 수정 요청 구조체
struct CorrectionRequest {
    qint64    newAmount      = 0;
    QDateTime newOccurredAt;
    QString   newMemo;
    QString   newCategory;  // Deposit 전용

    bool changeAmount      = false;
    bool changeOccurredAt  = false;
    bool changeMemo        = false;
    bool changeCategory    = false;
};

class CorrectionModule
{
public:
    explicit CorrectionModule(AccountManager *manager);

    // 거래 취소
    // - 이미 취소된 거래는 재취소 불가 (오류)
    // - 송금 거래(TransferOut/In) 취소 시 쌍(transferGroupId)을 함께 취소 (원자성)
    // 성공 시 true, 실패 시 false + outError
    bool cancelTransaction(int transactionId, QString *outError = nullptr);

    // 거래 수정
    // - 취소된 거래는 수정 불가
    // - 송금 거래(amount 변경)는 쌍 모두 수정
    // - 수정 후 잔고 정책 위반이면 실패(롤백)
    bool correctTransaction(int transactionId,
                            const CorrectionRequest &req,
                            QString *outError = nullptr);

    // 취소 거래 상태 확인 헬퍼
    bool isCanceled(int transactionId) const;

private:
    AccountManager *m_manager;

    // 송금 쌍의 상대 거래 ID 반환
    int findTransferPair(int transactionId) const;

    // 단일 거래를 Posted → Canceled 로 전환
    bool markCanceled(int transactionId, QString *outError);

    // 거래 필드 실제 수정 (내부)
    bool applyCorrection(int transactionId, const CorrectionRequest &req, QString *outError);
};

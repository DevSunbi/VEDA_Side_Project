#pragma once

#include <QDateTime>
#include <QString>

class AccountManager;

// -----------------------------------------------------------------------
// TransferModule
// 역할 : 송금(이체) 등록, 원자성 보장
// -----------------------------------------------------------------------
class TransferModule {
public:
  explicit TransferModule(AccountManager *manager);

  // 송금 등록
  // - fromAccountId  : 출금 계좌 ID
  // - toAccountId    : 입금 계좌 ID (fromAccountId != toAccountId 강제)
  // - amount         : 양의 정수
  // - occurredAt     : 거래 일시
  // - memo           : 메모 (옵션)
  //
  // 원자성: 출금 + 입금 둘 다 성공해야 최종 반영.
  //         출금 측 잔고 정책 위반 시 전체 실패.
  //
  // 성공 시 transferGroupId(> 0) 반환, 실패 시 -1
  int transfer(int fromAccountId, int toAccountId, qint64 amount,
               const QDateTime &occurredAt, const QString &memo = QString(),
               QString *outError = nullptr);

private:
  AccountManager *m_manager;

  bool validateTransfer(int fromAccountId, int toAccountId, qint64 amount,
                        QString *outError) const;
};

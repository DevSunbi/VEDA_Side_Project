#ifndef BANKMANAGER_H
#define BANKMANAGER_H

#include <QObject>
#include "account.h"
#include "transaction.h"

// 실제 입출금, 잔고 계산 로직 클래스
class BankManager : public QObject {
    Q_OBJECT

public:
    explicit BankManager(QObject *parent = nullptr);

    // 모델을 외부(MainWindow 등)에서 사용할 수 있게 연다
    AccountModel* accountModel() const
    {
        return m_accountModel;
    }
    TransactionModel* transactionModel() const
    {
        return m_transactionModel;
    }

    // 계좌 추가
    // 반환값 - 성공 true / 중복 계좌번호 false
    bool addAccount(const QString &accNum,
                    const QString &bank,
                    qint64 initial);

    // 거래 추가
    // type : TransactionType::Deposit or TransactionType::Withdraw
    void addTransaction(int accountId,
                        qint64 amount,
                        TransactionType type,
                        const QString &memo = "");

    // 계좌 삭제
    bool removeAccount(int accountId);

    // 입금 처리
    void deposit(int accountId,
                 qint64 amount,
                 const QString &memo = "");

    // 출금 처리
    // 반환값 : 성공 true / 잔고 부족 false
    bool withdraw(int accountId,
                  qint64 amount,
                  const QString &memo = "");

    // [수정] 송금 처리 추가
    // fromAccountId : 출금 계좌 ID (m_selectedAccountId)
    // toAccountId   : 입금 계좌 ID (상대 계좌)
    // amount        : 송금 금액
    // memo          : 메모 (선택)
    // true  : 송금 성공
    // false : 잔고 부족 / 동일 계좌 송금 / 존재하지 않는 계좌
    bool transfer(int fromAccountId,
                  int toAccountId,
                  qint64 amount,
                  const QString &memo = "");

    // 전체 잔고 재계산
    // 모든 거래 내역을 처음부터 다시 계산해서 잔고를 최신화
    void recalcAllBalances();

private:
    // 계좌 목록 관리
    AccountModel     *m_accountModel;
    // 거래 내역 관리
    TransactionModel *m_transactionModel;

    // 계좌 고유 ID 카운터
    int m_nextAccountId     = 1;
    int m_nextTransactionId = 1;

    int m_nextTransferGroupId = 1;
};

#endif // BANKMANAGER_H
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
    bool addAccount(const QString &name,
                    const QString &accNum,
                    const QString &bank,
                    qint64 initial);

    // 거래 추가
    // type : TransactionType::Deposit or TransactionType::Withdraw
    void addTransaction(int accountId,
                        qint64 amount,
                        TransactionType type,
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
};

#endif // BANKMANAGER_H
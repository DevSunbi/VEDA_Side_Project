#ifndef BANKMANAGER_H
#define BANKMANAGER_H

#include <QObject>
#include "account.h"
#include "transaction.h"

// =======================================================================
// 은행 업무 매니저 (실제 입출금, 잔고 계산 로직이 다 들어있는 핵심 심장부)
// =======================================================================
class BankManager : public QObject {
    Q_OBJECT
public:
    explicit BankManager(QObject *parent = nullptr);

    // 모델을 외부(MainWindow 등)에서 사용할 수 있게 열어주는 창구
    AccountModel* accountModel() const { return m_accountModel; }
    TransactionModel* transactionModel() const { return m_transactionModel; }

    // ── 핵심 기능 함수들 (이전 코드 스타일을 유지함) ──
    bool addAccount(const QString &accNum, qint64 initial);
    void addTransaction(int accountId, qint64 amount, const QString &type, const QString &memo = "", const QString &category = "");
    
    // 전체 거래 내역을 처음부터 끝까지 다 더하고 빼서 잔고를 최신으로 맞추는 마법의 함수!
    void recalcAllBalances();

private:
    AccountModel     *m_accountModel;     // 계좌 목록 관리
    TransactionModel *m_transactionModel; // 거래 내역 관리

    // 새 항목을 만들 때 부여할 고유 숫자 (1부터 1씩 더함)
    int m_nextAccountId = 1;
    int m_nextTransactionId = 1;
};

#endif // BANKMANAGER_H

#include "bankmanager.h"


BankManager::BankManager(QObject *parent) : QObject(parent) {
    m_accountModel = new AccountModel(this);
    m_transactionModel = new TransactionModel(this);
}

bool BankManager::addAccount(const QString &accNum, qint64 initial) {
    // 중복 체크
    for(const auto &acc : m_accountModel->accounts()) {
        if(acc.accountNumber == accNum) return false;
    }

    Account acc;
    acc.id = m_nextAccountId++;
    acc.accountNumber = accNum;
    acc.initialBalance = initial;
    acc.currentBalance = initial;

    m_accountModel->addAccount(acc);
    return true;
}

void BankManager::addTransaction(int accountId, qint64 amount, const QString &type, const QString &memo, const QString &category) {
    Transaction tx;
    tx.id = m_nextTransactionId++;
    tx.accountId = accountId;
    tx.amount = amount;
    tx.type = type;
    tx.memo = memo;
    tx.category = category;

    m_transactionModel->addTransaction(tx);
    recalcAllBalances();
}

void BankManager::recalcAllBalances() {
    auto &accounts = m_accountModel->accounts();
    for(auto &acc : accounts) {
        acc.currentBalance = acc.initialBalance;
    }

    const auto &transactions = m_transactionModel->transactions();
    for(const auto &tx : transactions) {
        if(tx.status != "정상") continue;
        for(auto &acc : accounts) {
            if(acc.id == tx.accountId) {
                if(tx.type == "입금") acc.currentBalance += tx.amount;
                else if(tx.type == "출금" || tx.type == "송금") acc.currentBalance -= tx.amount;
                break;
            }
        }
    }
    m_accountModel->updateAll();
}

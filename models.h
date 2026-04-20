#ifndef MODELS_H
#define MODELS_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QList>
#include <QColor>

// --- 데이터 구조체 ---
struct Account {
    int         id;
    QString     name;
    QString     accountNumber;
    QString     bankName;
    qint64      initialBalance;
    QDateTime   createdAt;
    QString     status; // "활성", "비활성"
    bool        allowOverdraft;
    qint64      currentBalance;

    Account() : id(0), initialBalance(0), status("활성"), allowOverdraft(false), currentBalance(0) {
        createdAt = QDateTime::currentDateTime();
    }
};

struct Transaction {
    int         id;
    int         accountId;
    qint64      amount;
    QDateTime   occurredAt;
    QString     memo;
    QString     category;
    QString     type;   // "입금", "출금", "송금"
    QString     status; // "정상", "취소"

    Transaction() : id(0), accountId(0), amount(0), type("입금"), status("정상") {
        occurredAt = QDateTime::currentDateTime();
    }
};

// --- 거래 내역 모델 (Transaction Model) ---
class TransactionModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit TransactionModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    // 필수 구현 함수들
    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_transactions.size(); }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 6; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_transactions.size()) return QVariant();

        const auto &tx = m_transactions[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case 0: return tx.occurredAt.toString("yyyy-MM-dd HH:mm");
                case 1: return tx.type;
                case 2: return tx.amount;
                case 3: return tx.category;
                case 4: return tx.memo;
                case 5: return tx.status;
            }
        }
        
        // 시각적 피드백 (취소된 내역은 빨간색으로 표시 등)
        if (role == Qt::ForegroundRole && tx.status == "취소") {
            return QColor(Qt::red);
        }

        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
        switch (section) {
            case 0: return "일시";
            case 1: return "구분";
            case 2: return "금액";
            case 3: return "카테고리";
            case 4: return "메모";
            case 5: return "상태";
        }
        return QVariant();
    }

    // 데이터 조작 함수
    void addTransaction(const Transaction &tx) {
        beginInsertRows(QModelIndex(), m_transactions.size(), m_transactions.size());
        m_transactions.append(tx);
        endInsertRows();
    }

    void setTransactions(const QList<Transaction> &list) {
        beginResetModel();
        m_transactions = list;
        endResetModel();
    }

    const QList<Transaction>& transactions() const { return m_transactions; }

private:
    QList<Transaction> m_transactions;
};

// --- 계좌 모델 (Account Model) ---
class AccountModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit AccountModel(QObject *parent = nullptr) : QAbstractTableModel(parent) {}

    int rowCount(const QModelIndex &parent = QModelIndex()) const override { return m_accounts.size(); }
    int columnCount(const QModelIndex &parent = QModelIndex()) const override { return 4; }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override {
        if (!index.isValid() || index.row() >= m_accounts.size()) return QVariant();
        const auto &acc = m_accounts[index.row()];

        if (role == Qt::DisplayRole) {
            switch (index.column()) {
                case 0: return acc.name;
                case 1: return acc.accountNumber;
                case 2: return acc.bankName;
                case 3: return acc.currentBalance;
            }
        }
        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override {
        if (role != Qt::DisplayRole || orientation != Qt::Horizontal) return QVariant();
        switch (section) {
            case 0: return "계좌명";
            case 1: return "계좌번호";
            case 2: return "은행";
            case 3: return "잔고";
        }
        return QVariant();
    }

    void addAccount(const Account &acc) {
        beginInsertRows(QModelIndex(), m_accounts.size(), m_accounts.size());
        m_accounts.append(acc);
        endInsertRows();
    }

    QList<Account>& accounts() { return m_accounts; }
    const QList<Account>& accounts() const { return m_accounts; }

    void updateAll() {
        emit dataChanged(index(0, 0), index(m_accounts.size()-1, columnCount()-1));
    }

private:
    QList<Account> m_accounts;
};

#endif // MODELS_H

#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <QString>
#include <QDateTime>
#include <QAbstractTableModel>
#include <QList>

// =======================================================================
// 초보자 친화적 데이터 구조체 (enum 없이 QString과 bool만 사용)
// =======================================================================
struct Account {
    int         id;
    QString     name;
    QString     accountNumber;
    QString     bankName;
    qint64      initialBalance;
    QDateTime   createdAt;
    
    // 상태값들 (enum 대신 그냥 문자열이나 bool로 직관적으로 관리)
    QString     status;         // "활성" 또는 "비활성"
    bool        allowOverdraft; // 거부: false, 허용(마이너스 가능): true
    qint64      currentBalance;

    Account();
};

// =======================================================================
// 계좌 목록 모델 (화면과 데이터를 연결하는 통로)
// =======================================================================
class AccountModel : public QAbstractTableModel {
    Q_OBJECT
public:
    explicit AccountModel(QObject *parent = nullptr);

    enum Column {
        NameColumn = 0,
        NumberColumn,
        BankColumn,
        BalanceColumn,
        ColumnCount
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    // 데이터를 넣고 빼는 직관적인 함수들
    void addAccount(const Account &acc);
    QList<Account>& accounts();
    //const QList<Account>& accounts() const;
    void updateAll(); // 화면 싹 다 새로고침 하라고 신호 보내기
    void removeAccount(int row);


private:
    QList<Account> m_accounts; // 실제 계좌들이 담길 바구니
};

#endif // ACCOUNT_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QDateTime>
#include <QList>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

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
    QString     status; // "활성" 또는 "비활성"
    bool        allowOverdraft; // 거부: false, 허용(마이너스 가능): true
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

    // 상태값들 (enum을 없애고 초보자가 읽기 쉬운 문자열 형태로 고정)
    QString     type;   // "입금", "출금", "송금"
    QString     status; // "정상", "취소"

    Transaction() : id(0), accountId(0), amount(0), type("입금"), status("정상") {
        occurredAt = QDateTime::currentDateTime();
    }
};

// =======================================================================
// 단일 MainWindow (모든 기능을 여기서 처리)
// =======================================================================

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // ── UI 버튼 슬롯 (이 안에서 if / for 문으로 직접 모든 기능 구현) ──
    void on_pushButton_calc_clicked();      // 입금 처리
    void on_pushButton_save_clicked();      // 출금 처리
    void on_pushButton_login_clicked();     // 송금 처리
    void on_pushButton_reset_clicked();     // 내역 정정/취소 처리
    void on_pushButton_help_clicked();      // 파일 등으로 저장

    void on_actionAccount_View_triggered(); // 계좌 목록 조회 창 띄우기
    void on_actionAdd_triggered();          // 새로 계좌를 만들 때 실행 (계좌 생성 로직)
    void on_actionDelete_triggered();       // 계좌 상태 비활성화 로직

    void on_tabWidget_semester_currentChanged(int index);

private:
    Ui::MainWindow *ui;

    // ── 가장 중요한 핵심 데이터 변수 ──
    QList<Account>      m_accounts;       // 생성된 모든 계좌 목록
    QList<Transaction>  m_transactions;   // 일어난 모든 거래 내역

    // ── 새 항목을 만들 때 부여할 고유 숫자 (안전하게 1부터 1씩 더해감) ──
    int m_nextAccountId = 1;
    int m_nextTransactionId = 1;

    // ── 기타 화면 편의 함수 ──
    int currentAccountId() const;    // 현재 화면 탭에 뜬 계좌의 ID가 뭔지 반환
    void refreshTransactionList();   // 화면에 거래내역 테이블 다시 그려주기
    void refreshSummary();           // 화면 하단 잔고 등 다시 계산해서 글자 바꾸기
    void recalcAllBalances();        // 전체 거래 내역을 처음부터 끝까지 다 더하고 빼서 잔고를 최신으로 맞추는 함수!
};

#endif // MAINWINDOW_H

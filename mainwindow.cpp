#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // [뼈대] 매니저 생성
    m_bankManager = new BankManager(this);

    // [힌트] 아래 주석을 풀고 UI의 테이블뷰와 모델을 연결하세요.
    // ui->tableView->setModel(m_bankManager->transactionModel());
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ── 보조 함수 구현 힌트 ──────────────────────────────────────────────────

int MainWindow::currentAccountId() const
{
    // [TODO] 탭 위젯(ui->tabWidget_...)의 현재 index를 구하고,
    // 해당 index의 탭이 어떤 계좌ID를 가졌는지 반환해야 합니다.
    // 예: return ui->tabWidget->currentIndex() + 1; (임시)
    return -1; 
}

void MainWindow::refreshSummary()
{
    // [TODO] ui->lineEdit_total->setText(...) 처럼 
    // m_bankManager->accountModel()에서 잔고 정보를 가져와 화면에 찍어주세요.
}

// ── 버튼 클릭 시 할 일 (직접 구현해보기) ───────────────────────────────────

void MainWindow::on_actionAdd_triggered()
{
    // [TODO] 사용자에게 입력받을 팝업창을 띄우거나, lineEdit에서 텍스트를 읽어오세요.
    QString testName = "새 계좌";
    QString testNum  = "111-222";
    
    // 매니저에게 계좌 추가를 시킵니다. (성공하면 true 반환)
    bool ok = m_bankManager->addAccount(testName, testNum, "기본은행", 0);
    
    if(ok) {
        QMessageBox::information(this, "알림", "계좌가 생겼습니다!");
        // [TODO] ui->tabWidget에 새로운 탭을 추가하는 코드를 작성하세요.
    }
}

void MainWindow::on_actionDelete_triggered()
{
    // [TODO] 현재 선택된 탭의 계좌를 확인하고, 삭제 로직을 구현하세요.
}

void MainWindow::on_pushButton_calc_clicked()   // [입금] 버튼
{
    int id = currentAccountId();
    if(id == -1) return;

    // [TODO] ui->lineEdit_amount 등에서 입금액을 숫자로 가져오세요.
    qint64 amount = 5000; 

    // 매니저에게 거래 추가를 시킵니다. (내부에서 잔고 계산도 자동으로 수행됨)
    m_bankManager->addTransaction(id, amount, "입금", "용돈");
    
    refreshSummary(); // 화면 잔고 글자 갱신
}

void MainWindow::on_pushButton_save_clicked()   // [출금] 버튼
{
    // [TODO] 입금과 비슷하게 구현하되, "출금" 타입을 전달하세요.
    // 힌트: m_bankManager->addTransaction(id, amount, "출금");
}

void MainWindow::on_pushButton_login_clicked()  // [송금] 버튼
{
    // [TODO] 출금 1번, 입금 1번을 연속으로 처리하면 송금이 됩니다!
}

void MainWindow::on_pushButton_reset_clicked()  // [정정/취소] 버튼
{
    // [TODO] ui->tableView에서 현재 선택된 행(row)을 찾고,
    // 해당 거래의 status를 "취소"로 바꾼 뒤 m_bankManager->recalcAllBalances() 하세요.
}

void MainWindow::on_pushButton_help_clicked()   // [저장] 버튼
{
    // [TODO] m_bankManager 내의 데이터를 파일로 저장하는 기능을 구현해보세요.
}

void MainWindow::on_tabWidget_semester_currentChanged(int index)
{
    Q_UNUSED(index)
    refreshSummary();
}

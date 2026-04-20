#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // [뼈대] 매니저 생성
  m_bankManager = new BankManager(this);

  // [힌트] 아래 주석을 풀고 UI의 테이블뷰와 모델을 연결하세요.
  // ui->tableView->setModel(m_bankManager->transactionModel());
}

MainWindow::~MainWindow() { delete ui; }

// ── 보조 함수 구현 힌트 ──────────────────────────────────────────────────

int MainWindow::currentAccountId() const {
  return ui->Sel_Acc_Tab->currentIndex(); // UI 명은 차후 변경 필수
}

void MainWindow::refreshSummary() {
  int id = currentAccountId();
  if (id == -1)
    return;

  const auto &accounts = m_bankManager->accountModel()->accounts();
  if (id < accounts.size()) {
    qint64 balance = accounts[id].currentBalance;

    ui->CBal_f_lbl->setText(QString::number(balance) + "Won");
    // ui->Arr_r_lbl->setText(accounts[id].accountNumber); // 계좌 라벨에도
    // 이름/번호 표시
  }
}

// ── 버튼 클릭 시 할 일 (직접 구현해보기) ───────────────────────────────────

void MainWindow::on_actionAdd_triggered() {
  // [TODO] 사용자에게 입력받을 팝업창을 띄우거나, lineEdit에서 텍스트를
  // 읽어오세요.
  QString testName = "새 계좌";
  QString testNum = "111-222";

  bool ok;

  QString newAccNum =
      QInputDialog::getText(this, "계좌 생성", "추가 할 계좌 번호를 적어주세요",
                            QLineEdit::Normal, "", &ok);

  // 매니저에게 계좌 추가를 시킵니다. (성공하면 true 반환)
  if(ok && !newAccNum.isEmpty())
  {
      bool isSuccess = m_bankManager->addAccount(newAccNum, 0);
      if (isSuccess) {
          ui->Sel_Acc_Tab->addTab(new QWidget(), newAccNum);
          QMessageBox::information(this, "알림", "계좌가 생겼습니다!");
          // [TODO] ui->tabWidget에 새로운 탭을 추가하는 코드를 작성하세요.
      }
  }

}

void MainWindow::on_actionDelete_triggered() {
  // [TODO] 현재 선택된 탭의 계좌를 확인하고, 삭제 로직을 구현하세요.
  int targetId = currentAccountId();
  if (targetId >= 0) {
    ui->Sel_Acc_Tab->removeTab(targetId);
    m_bankManager->accountModel()->removeAccount(targetId);
  }
}

void MainWindow::on_Deposit_Btn_clicked() // [입금] 버튼
{
  int id = currentAccountId();
  if (id == -1)
    return;

  qint64 amount = ui->Amount_lbl->text().toLongLong();
  if (amount <= 0)
    return;

  m_bankManager->addTransaction(id, amount, "입금", "사용자입금", "현금");
  refreshSummary();
  ui->Amount_lbl->clear();
}

void MainWindow::on_Withdraw_Btn_clicked() // [출금] 버튼
{
  // [TODO] 입금과 비슷하게 구현하되, "출금" 타입을 전달하세요.
  // 힌트: m_bankManager->addTransaction(id, amount, "출금");
  int id = currentAccountId();
  if (id == -1)
    return;

  qint64 amount = ui->Amount_lbl->text().toLongLong();
  if (amount <= 0)
    return;

  m_bankManager->addTransaction(id, amount, "출금", "사용자출금", "현금");
  refreshSummary();
  ui->Amount_lbl->clear();
}

void MainWindow::on_Confirm_Btn_clicked() // [송금] 버튼
{
  // [TODO] 출금 1번, 입금 1번을 연속으로 처리하면 송금이 됩니다!
  int myId = currentAccountId();
  QString targetAcc = ui->AccountID_lbl->text();
  qint64 amount = ui->Amount_lbl->text().toLongLong();

  if (myId == -1 || amount <= 0 || targetAcc.isEmpty())
    return;

  const auto &accounts = m_bankManager->accountModel()->accounts();
  int targetId = -1;
  for (int i = 0; i < accounts.size(); i++) {
    if (accounts[i].accountNumber == targetAcc) {
      targetId = i;
      break;
    }
  }

  if (targetId == -1) {
    QMessageBox::critical(this, "Error", "존재하지 않는 계좌번호입니다!");
    return;
  }

  m_bankManager->addTransaction(myId, amount, "송금", "송금 출금", "계좌 이체");
  m_bankManager->addTransaction(targetId, amount, "입금", "이체 입금",
                                "계좌 이체");

  refreshSummary();
  ui->Amount_lbl->clear();
  ui->AccountID_lbl->clear();
}

void MainWindow::on_Re_Btn_clicked() // [정정/취소] 버튼
{
  // [TODO] ui->tableView에서 현재 선택된 행(row)을 찾고,
  // 해당 거래의 status를 "취소"로 바꾼 뒤 m_bankManager->recalcAllBalances()
  // 하세요.
}

void MainWindow::on_Save_Btn_clicked() // [저장] 버튼
{
  // [TODO] m_bankManager 내의 데이터를 파일로 저장하는 기능을 구현해보세요.
}

void MainWindow::on_Sel_Acc_Tab_currentChanged(int index) {
  Q_UNUSED(index)
  refreshSummary();
}

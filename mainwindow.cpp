#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QInputDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QVBoxLayout>
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);

  // [뼈대] 매니저 생성
  m_bankManager = new BankManager(this);

  // 불필요한 기본 탭 제거
  ui->Sel_Acc_Tab->clear();

  // [추가] 저장된 JSON 이 있다면 불러와서 복원합니다.
  loadFromFile();

  // [힌트] 아래 주석을 풀고 UI의 테이블뷰와 모델을 연결하세요.
  // ui->tableView->setModel(m_bankManager->transactionModel());

  // 기능 연결 (ui 이름 임의 지정하여 연결해둠)
  // ui->DummyTransactionTableView->setModel(m_bankManager->transactionModel()); // 차후 수정 필요
}

MainWindow::~MainWindow() { delete ui; }

// ── 보조 함수 구현 힌트 ──────────────────────────────────────────────────

void MainWindow::loadFromFile() {
    QFile file("account_info.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return; // 파일이 없거나 읽을 수 없으면 무시
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) return;

    QJsonObject rootObj = doc.object();
    QJsonArray accountArray = rootObj["accounts"].toArray();

    for (const QJsonValue &val : accountArray) {
        if (!val.isObject()) continue;
        QJsonObject obj = val.toObject();

        Account acc;
        acc.id = obj["id"].toInt();
        acc.name = obj["name"].toString();
        acc.accountNumber = obj["accountNumber"].toString();
        acc.bankName = obj["bankName"].toString();
        acc.initialBalance = obj["initialBalance"].toVariant().toLongLong();
        acc.createdAt = QDateTime::fromString(obj["createdAt"].toString(), Qt::ISODate);
        acc.status = obj["status"].toString();
        acc.allowOverdraft = obj["allowOverdraft"].toBool();
        acc.currentBalance = obj["currentBalance"].toVariant().toLongLong();
        acc.password = obj["password"].toString();

        m_bankManager->restoreAccount(acc);
        ui->Sel_Acc_Tab->addTab(new QWidget(), acc.accountNumber);
    }
}

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
    // ui->Arr_r_lbl->setText(accounts[id].accountNumber); // 계좌 라벨에도 이름/번호 표시

    // 그래프 최신화
    updateGraph(id);
  }
}

void MainWindow::updateGraph(int accountId) {
    const auto &transactions = m_bankManager->transactionModel()->transactions();
    const auto &accounts = m_bankManager->accountModel()->accounts();
    if (accountId < 0 || accountId >= accounts.size()) return;
    
    // 차트 시리즈 데이터 준비
    int realAccId = accounts[accountId].id;
    qint64 initialBal = accounts[accountId].initialBalance;
    qint64 currentBal = initialBal;
    
    QLineSeries *series = new QLineSeries();
    series->append(accounts[accountId].createdAt.toMSecsSinceEpoch(), currentBal);

    for (const auto &tx : transactions) {
        // 이전에 index로 저장된 잘못된 데이터가 있을 법하니 fallback(||) 지원
        if ((tx.accountId == realAccId || tx.accountId == accountId) && tx.status == "정상") {
            if (tx.type == "입금") currentBal += tx.amount;
            else if (tx.type == "출금" || tx.type == "송금") currentBal -= tx.amount;
            series->append(tx.occurredAt.toMSecsSinceEpoch(), currentBal);
        }
    }

    // 시간 순 정렬을 위해 
    // Qt Charts에서는 X값이 반드시 순차증가해야 깔끔하게 그려집니다.
    // 여기서는 기본적으로 시간순 발생이라고 가정합니다.

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();
    
    // X축 커스텀 (시간)
    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(4);
    axisX->setFormat("MM/dd hh:mm");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    // Y축 커스텀 (잔액)
    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%d");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    // Graph_Widget 내 기존 레이아웃 삭제 후 갱신
    QLayout *oldLayout = ui->Graph_Widget->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    QVBoxLayout *layout = new QVBoxLayout(ui->Graph_Widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(chartView);
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

  if(!ok || newAccNum.isEmpty()) return;

  QString newPassword =
      QInputDialog::getText(this, "비밀번호 설정", "사용하실 비밀번호를 입력해주세요",
                            QLineEdit::Password, "", &ok);
  if(!ok) return;

  // 매니저에게 계좌 추가를 시킵니다. (성공하면 true 반환)
  bool isSuccess = m_bankManager->addAccount(newAccNum, 0, newPassword);
  if (isSuccess) {
      ui->Sel_Acc_Tab->addTab(new QWidget(), newAccNum);
      QMessageBox::information(this, "알림", "계좌가 생겼습니다!");
      // [TODO] ui->tabWidget에 새로운 탭을 추가하는 코드를 작성하세요.
  } else {
      QMessageBox::critical(this, "오류", "계좌 생성에 실패했습니다 (예: 계좌번호 중복)");
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

  const auto &accounts = m_bankManager->accountModel()->accounts();
  int realAccId = accounts[id].id;
  m_bankManager->addTransaction(realAccId, amount, "입금", "사용자입금", "현금");
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

  const auto &accounts = m_bankManager->accountModel()->accounts();
  const Account &acc = accounts[id];

  // 잔고 부족 방어 (allowOverdraft가 false인 경우만 방어)
  if (!acc.allowOverdraft && acc.currentBalance < amount) {
      QMessageBox::warning(this, "출금 실패", "잔고가 부족합니다!");
      return;
  }

  // 비밀번호 인증 로직 (출금 시 팝업 활용)
  bool ok;
  QString inputPw = QInputDialog::getText(this, "비밀번호 확인", "출금을 위해 비밀번호를 입력해주세요:", QLineEdit::Password, "", &ok);
  if (!ok || inputPw != acc.password) {
      QMessageBox::warning(this, "출금 실패", "비밀번호가 다르거나 취소되었습니다.");
      return;
  }

  int realAccId = accounts[id].id;
  m_bankManager->addTransaction(realAccId, amount, "출금", "사용자출금", "현금");
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
  const Account &myAcc = accounts[myId];

  // 잔고 부족 방어
  if (!myAcc.allowOverdraft && myAcc.currentBalance < amount) {
      QMessageBox::warning(this, "송금 실패", "잔고가 부족합니다!");
      return;
  }

  // 비밀번호 인증 로직 (UI에 있는 Password_lbl 박스 이용)
  QString inputPw = ui->Password_lbl->text();
  if (inputPw != myAcc.password) {
      QMessageBox::warning(this, "송금 실패", "비밀번호가 일치하지 않습니다!");
      return;
  }

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

  int realMyId = accounts[myId].id;
  int realTargetId = accounts[targetId].id;

  m_bankManager->addTransaction(realMyId, amount, "송금", "송금 출금", "계좌 이체");
  m_bankManager->addTransaction(realTargetId, amount, "입금", "이체 입금",
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

  // 기능 연결 (ui 이름 임의 지정)
  // int row = ui->DummyTransactionTableView->currentIndex().row(); // 차후 수정 필요
  // if (row >= 0) {
  //     // m_bankManager->transactionModel()->transactions()[row].status = "취소"; // 차후 수정 필요
  //     m_bankManager->recalcAllBalances();
  // }
}

void MainWindow::on_Save_Btn_clicked() // [저장] 버튼
{
  // [TODO] m_bankManager 내의 데이터를 파일로 저장하는 기능을 구현해보세요.
  
  QJsonArray accountArray;
  const auto &accounts = m_bankManager->accountModel()->accounts();
  for (const auto &acc : accounts) {
      QJsonObject accObj;
      accObj["id"] = acc.id;
      accObj["name"] = acc.name;
      accObj["accountNumber"] = acc.accountNumber;
      accObj["bankName"] = acc.bankName;
      accObj["initialBalance"] = acc.initialBalance;
      accObj["createdAt"] = acc.createdAt.toString(Qt::ISODate);
      accObj["status"] = acc.status;
      accObj["allowOverdraft"] = acc.allowOverdraft;
      accObj["currentBalance"] = acc.currentBalance;
      accObj["password"] = acc.password;
      accountArray.append(accObj);
  }

  QJsonObject rootObj;
  rootObj["accounts"] = accountArray;

  QJsonDocument doc(rootObj);
  QFile file("account_info.json"); // 저장 경로 및 파일명 차후 수정 필요
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      file.write(doc.toJson());
      file.close();
      QMessageBox::information(this, "저장 성공", "계좌 정보가 JSON 형태로 저장되었습니다.\n(파일명: account_info.json)");
  } else {
      QMessageBox::critical(this, "저장 실패", "파일을 저장할 수 없습니다.");
  }
}

void MainWindow::on_Sel_Acc_Tab_currentChanged(int index) {
  Q_UNUSED(index)
  refreshSummary();
}

void MainWindow::on_tabWidget_semester_currentChanged(int index) {
  Q_UNUSED(index)
  // 기능 연결 (ui 이름 임의 지정)
  // ui->DummySemesterTab_NeedsFix->setCurrentIndex(index); // 차후 수정 필요
}

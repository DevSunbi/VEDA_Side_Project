#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QDateTimeAxis>
#include <QVBoxLayout>
#include <QInputDialog>
#include "DepositDialog.h"
#include "WithdrawDialog.h"
#include "TransferDialog.h"
#include <QHeaderView>

// 생성자
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_selectedAccountId(-1)                   // 초기값 : 선택된 계좌 없음
{
    ui->setupUi(this);
    m_bankManager = new BankManager(this);
    m_proxyModel = new AccountFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_bankManager->transactionModel());

    // [레이아웃 조정] 가로 비율을 원래대로 복구하여 데이터 잘림 방지 (Table:Btn:Graph = 3:1:3)
    ui->Top_Lay->setColumnStretch(0, 3);
    ui->Top_Lay->setColumnStretch(1, 1);
    ui->Top_Lay->setColumnStretch(2, 3);

    // [그래프 확대] 결과창 내에서 그래프가 차지하는 세로 비중을 대폭 확대 (결과텍스트:그래프 = 1:12)
    ui->verticalLayout_5->setStretch(0, 1);
    ui->verticalLayout_5->setStretch(1, 12);

    // 메인 화면 UI 비밀번호 모드 마스킹 (삭제됨)

    // 저장된 데이터 자동 로드
    loadFromFile();

    // 거래 내역 테이블뷰 설정: 글자 잘림 방지 (핵심 컬럼은 내용에 맞게, 메모만 늘림)
    ui->Acc_tableview->setModel(m_proxyModel);
    ui->Acc_tableview->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->Acc_tableview->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch); // 메모 컬럼만 늘림
    ui->Acc_tableview->verticalHeader()->setVisible(false);

    /* [True Toss Light Style] 최종 고도화 버전 */
    QString tossLightStyle = R"(
        /* 전역 폰트 및 배경 */
        QWidget { 
            background-color: #F2F4F6; 
            color: #191F28; 
            font-family: 'Segoe UI', 'Malgun Gothic', sans-serif; 
        }

        /* 메인 윈도우 */
        QMainWindow, QWidget#centralwidget, QDialog { 
            background-color: #F2F4F6; 
        }

        /* 카드 (그룹박스): 센터 타이틀 및 여백 강화 */
        QGroupBox { 
            background-color: #FFFFFF; 
            border: none; 
            border-radius: 28px; 
            margin: 12px;
            font-weight: bold; 
            padding: 25px;
        }
        QGroupBox::title { 
            subcontrol-origin: padding; 
            subcontrol-position: top center; 
            padding-top: 15px;
            color: #4E5968; 
            font-size: 13pt; 
            font-weight: 800;
        }

        /* 버튼 가시성 확대 */
        QPushButton { 
            background-color: #3182F6; 
            color: white; 
            border: none; 
            border-radius: 16px; 
            padding: 12px; 
            font-weight: bold; 
            font-size: 14pt; /* [확대] 글씨 크기 키움 */
            min-height: 45px; 
        }
        QPushButton:hover { background-color: #5195F8; }
        QPushButton:disabled { background-color: #E5E8EB; color: #ADB5BD; }

        /* 테이블 뷰 클린업 */
        QTableView { 
            background-color: #FFFFFF; 
            border: none; 
            gridline-color: transparent; 
            selection-background-color: #F2F4F6; 
            selection-color: #3182F6;
            outline: none;
        }
        QHeaderView::section { 
            background-color: #FFFFFF; 
            color: #8B95A1; 
            padding: 12px; 
            border: none; 
            border-bottom: 1px solid #F2F4F6;
            font-weight: bold;
        }

        /* 결과창 라벨 완전 투명화 */
        QLabel#Date_r_lbl, QLabel#Acc_r_lbl, QLabel#Date_lbl, QLabel#Acc_lbl {
            background-color: transparent;
            border: none;
            padding: 5px;
        }
        /* 중첩된 그래프 그룹박스의 여백 제거 (그래프 크기 극대화) */
        /* 중첩된 그래프/결과 그룹박스의 여백 및 제목 정리 (겹침 방지) */
        QGroupBox#Graph_Group, QGroupBox#ResultGroup {
            padding-top: 15px; /* 제목 대신 공간 확보 */
            padding-left: 0px;
            padding-right: 0px;
            padding-bottom: 0px;
            margin: 0px;
            background-color: transparent;
        }
        QGroupBox#Graph_Group::title, QGroupBox#ResultGroup::title { 
            height: 0px; 
            color: transparent;  /* 겹침의 원인인 제목 숨김 */
        }

        /* 상단 네비게이션용 모든 라벨 */
        QLabel#label, QLabel#Sel_acc, QLabel#CBal_lbl, QLabel#CBal_f_lbl, QLabel#History_lbl, QLabel#History_r_lbl { 
            font-weight: 800; 
            font-size: 13pt; 
            color: #3182F6; 
            background: transparent;
            padding: 0 3px;
        }
        
        /* 카드 및 전역 라벨 기본색 */
        QLabel { color: #191F28; }
    )";
    this->setStyleSheet(tossLightStyle);

    //계좌 미선택시 버튼 비활성화
    ui->Deposit_Btn->setEnabled(false);
    ui->Withdraw_Btn->setEnabled(false);
    ui->Confirm_Btn->setEnabled(false);
}

// 소멸자
MainWindow::~MainWindow()
{
    delete ui;
}


// AccSearchDialog 팝업 → 인증 성공 시 m_selectedAccountId 저장
void MainWindow::on_Check_Acc_triggered()
{
    AccSearchDialog dlg(this);

    //   - 사용자가 OK / Cancel 누를 때까지 대기
    //   - OK 누르면 QDialog::Accepted 반환
    if (dlg.exec() == QDialog::Accepted) {
        QString accountNumber = dlg.getAccountNumber();
        QString password      = dlg.getPassword();

        // 입력값 공백 검사
        if (accountNumber.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "계좌번호와 비밀번호를 입력해주세요.");
            return;
        }

        // 계좌 존재 여부 확인
        // [TODO] Account 파트 연동 후 실제 조회로 교체
        int accountId = -1;
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.accountNumber == accountNumber) {
                accountId = acc.id;
                break;
            }
        }

        if (accountId == -1) {
            QMessageBox::warning(this, "계좌 오류", "존재하지 않는 계좌번호입니다.");
            return;
        }

        // 비밀번호 인증 연동
        const auto &accList = m_bankManager->accountModel()->accounts();
        bool isPasswordMatch = false;
        for (const auto &acc : accList) {
            if (acc.id == accountId && acc.password == password) {
                isPasswordMatch = true;
                break;
            }
        }
        
        if(!isPasswordMatch) {
            QMessageBox::warning(this, "인증 오류", "비밀번호가 일치하지 않습니다.");
            return;
        }

        m_selectedAccountId = accountId;
        ui->Sel_acc->setText(accountNumber);

        //계좌 선택 시 버튼 활성화
        ui->Deposit_Btn->setEnabled(true);
        ui->Withdraw_Btn->setEnabled(true);
        ui->Confirm_Btn->setEnabled(true);
        QMessageBox::information(this, "계좌 조회", "계좌가 선택되었습니다.");

        // 조회 하자마자 즉시 화면 잔고 & 차트 최신화
        refreshSummary();
    }
}


// on_Deposit_Btn_clicked()
// 입금 모드로 설정 후 팝업으로 알림
void MainWindow::on_Deposit_Btn_clicked()
{
    DepositDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QString accountNumber = dlg.getAccountNumber();
        QString password      = dlg.getPassword();
        qint64  amount        = dlg.getAmount();

        // 입력값 검사
        if (accountNumber.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "모든 항목을 입력해주세요.");
            return;
        }

        if (amount <= 0) {
            QMessageBox::warning(this, "입력 오류", "올바른 금액을 입력해주세요.");
            return;
        }

        // 계좌 존재 여부 확인
        int accountId = -1;
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.accountNumber == accountNumber) {
                accountId = acc.id;
                break;
            }
        }

        if (accountId == -1) {
            QMessageBox::warning(this, "계좌 오류", "존재하지 않는 계좌번호입니다.");
            return;
        }

        // 비밀번호 인증 연동
        const auto &accList = m_bankManager->accountModel()->accounts();
        bool isPasswordMatch = false;
        for (const auto &acc : accList) {
            if (acc.id == accountId && acc.password == password) {
                isPasswordMatch = true;
                break;
            }
        }
        
        if(!isPasswordMatch) {
            QMessageBox::warning(this, "인증 오류", "비밀번호가 일치하지 않습니다.");
            return;
        }

        // 메모 추가 팝업 (선택 사항)
        QString memo = QInputDialog::getText(this, "메모 입력 (선택)", "입금 메모를 남기시겠습니까? (없으면 빈칸):");

        // 입금 처리
        m_bankManager->deposit(accountId, amount, memo);

        // 뷰어 포커스를 거래한 계좌로 자동 변경
        m_selectedAccountId = accountId;
        ui->Sel_acc->setText("선택된 계좌 : " + accountNumber);
        ui->Deposit_Btn->setEnabled(true);
        ui->Withdraw_Btn->setEnabled(true);
        ui->Confirm_Btn->setEnabled(true);

        // 결과 라벨 반영
        ui->Date_r_lbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
        ui->Acc_r_lbl->setText(accountNumber);
        ui->History_r_lbl->setText("입금  +" + QString::number(amount) + " 원");

        refreshSummary();
    }
}

// on_Withdraw_Btn_clicked()
// 출금 모드로 설정 후 팝업으로 알림
void MainWindow::on_Withdraw_Btn_clicked()
{
    WithdrawDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QString accountNumber = dlg.getAccountNumber();
        QString password      = dlg.getPassword();
        qint64  amount        = dlg.getAmount();

        // 입력값 검사
        if (accountNumber.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "모든 항목을 입력해주세요.");
            return;
        }

        if (amount <= 0) {
            QMessageBox::warning(this, "입력 오류", "올바른 금액을 입력해주세요.");
            return;
        }

        // 계좌 존재 여부 확인
        int accountId = -1;
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.accountNumber == accountNumber) {
                accountId = acc.id;
                break;
            }
        }

        if (accountId == -1) {
            QMessageBox::warning(this, "계좌 오류", "존재하지 않는 계좌번호입니다.");
            return;
        }

        // 비밀번호 인증 연동
        const auto &accList = m_bankManager->accountModel()->accounts();
        bool isPasswordMatch = false;
        for (const auto &acc : accList) {
            if (acc.id == accountId && acc.password == password) {
                isPasswordMatch = true;
                break;
            }
        }
        
        if(!isPasswordMatch) {
            QMessageBox::warning(this, "인증 오류", "비밀번호가 일치하지 않습니다.");
            return;
        }

        // 메모 추가 팝업 (선택 사항)
        QString memo = QInputDialog::getText(this, "메모 입력 (선택)", "출금 메모를 남기시겠습니까? (없으면 빈칸):");

        // 출금 처리 (잔고 부족 시 false 반환)
        bool ok = m_bankManager->withdraw(accountId, amount, memo);
        if (!ok) {
            QMessageBox::warning(this, "출금 오류", "잔고가 부족합니다.");
            return;
        }

        // 뷰어 포커스를 거래한 계좌로 자동 변경
        m_selectedAccountId = accountId;
        ui->Sel_acc->setText("선택된 계좌 : " + accountNumber);
        ui->Deposit_Btn->setEnabled(true);
        ui->Withdraw_Btn->setEnabled(true);
        ui->Confirm_Btn->setEnabled(true);

        // 결과 라벨 반영
        ui->Date_r_lbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
        ui->Acc_r_lbl->setText(accountNumber);
        ui->History_r_lbl->setText("출금  -" + QString::number(amount) + " 원");

        refreshSummary();
    }
}

// [슬롯] on_Confirm_Btn_clicked()
void MainWindow::on_Confirm_Btn_clicked()
{
    // 1. 계좌 선택 여부 확인
    if (m_selectedAccountId == -1) {
        QMessageBox::warning(this, "계좌 오류", "먼저 계좌를 선택해주세요.\n메뉴바 → 계좌 조회");
        return;
    }

    TransferDialog dlg(this);
    if (dlg.exec() == QDialog::Accepted) {
        QString toAccountNumber = dlg.getToAccountNumber().trimmed();
        QString password        = dlg.getPassword().trimmed();
        qint64  amount          = dlg.getAmount();

        // 2. 유효성 검사
        if (toAccountNumber.isEmpty() || password.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "모든 항목을 입력해주세요.");
            return;
        }

        if (amount <= 0) {
            QMessageBox::warning(this, "입력 오류", "올바른 금액을 입력해주세요.");
            return;
        }

        // 타겟 계좌 존재 여부 확인
        int toAccountId = -1;
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.accountNumber == toAccountNumber) {
                toAccountId = acc.id;
                break;
            }
        }

    if (toAccountId == -1) {
        QMessageBox::warning(this, "계좌 오류", "존재하지 않는 계좌번호입니다.");
        return;
    }

    // 비밀번호 인증 연동 (출금 계좌(내 계좌)의 비밀번호 검사)
    const auto &accList = m_bankManager->accountModel()->accounts();
    bool isPasswordMatch = false;
    for (const auto &acc : accList) {
        if (acc.id == m_selectedAccountId && acc.password == password) {
            isPasswordMatch = true;
            break;
        }
    }
    
    if(!isPasswordMatch) {
        QMessageBox::warning(this, "인증 오류", "선택된 계좌의 비밀번호가 일치하지 않습니다.");
        return;
    }

    // 메모 추가 팝업 (선택 사항)
    QString memo = QInputDialog::getText(this, "메모 입력 (선택)", "송금 메모를 남기시겠습니까? (없으면 빈칸):");

    // 4. 송금 처리
    bool result = m_bankManager->transfer(m_selectedAccountId, toAccountId, amount, memo);

    if (!result) {
        QMessageBox::warning(this, "송금 오류", "송금에 실패했습니다.\n잔고 부족 또는 동일 계좌 송금입니다.");
        return;
    }

    // 결과 라벨 반영 (보내는 계좌 화면이므로 대상이 명확히 나오게 개선)
    QString fromAccountNumber = "";
    for (const auto &acc : m_bankManager->accountModel()->accounts()) {
        if (acc.id == m_selectedAccountId) {
            fromAccountNumber = acc.accountNumber;
            break;
        }
    }
    ui->Date_r_lbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    ui->Acc_r_lbl->setText(fromAccountNumber + " ➔ " + toAccountNumber);
    ui->History_r_lbl->setText("송금  -" + QString::number(amount) + " 원");

    refreshSummary();
    }
}



// 계좌 생성(메뉴 바)
void MainWindow::on_InsertAcc_triggered()
{
    AddAccDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QString accountNumber   = dlg.getAccountNumber();
        QString password        = dlg.getPassword();
        QString passwordConfirm = dlg.getPasswordConfirm();

        // ──────────────────────────────────────────────────────
        // 입력값 공백 검사
        // ──────────────────────────────────────────────────────
        if (accountNumber.isEmpty() || password.isEmpty() || passwordConfirm.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "모든 항목을 입력해주세요.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 비밀번호 일치 여부 확인
        // ──────────────────────────────────────────────────────
        if (password != passwordConfirm) {
            QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 계좌 생성
        // 중복 계좌번호 시 false 반환
        // ──────────────────────────────────────────────────────
        bool ok = m_bankManager->addAccount(accountNumber, password, "", 0);

        if (!ok) {
            QMessageBox::warning(this, "생성 오류", "이미 존재하는 계좌번호입니다.");
            return;
        }

        QMessageBox::information(this, "계좌 생성", "계좌가 생성되었습니다.");
    }
}


void MainWindow::on_DeleteAcc_triggered()
{
    DeleteAccDialog dlg(this);

    if (dlg.exec() == QDialog::Accepted) {
        QString accountNumber   = dlg.getAccountNumber();
        QString password        = dlg.getPassword();
        QString passwordConfirm = dlg.getPasswordConfirm();

        // ──────────────────────────────────────────────────────
        // 입력값 공백 검사
        // ──────────────────────────────────────────────────────
        if (accountNumber.isEmpty() || password.isEmpty() || passwordConfirm.isEmpty()) {
            QMessageBox::warning(this, "입력 오류", "모든 항목을 입력해주세요.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 비밀번호 일치 여부 확인
        // ──────────────────────────────────────────────────────
        if (password != passwordConfirm) {
            QMessageBox::warning(this, "입력 오류", "비밀번호가 일치하지 않습니다.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 계좌 존재 여부 확인
        // ──────────────────────────────────────────────────────
        int accountId = -1;
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.accountNumber == accountNumber) {
                accountId = acc.id;
                break;
            }
        }

        if (accountId == -1) {
            QMessageBox::warning(this, "삭제 오류", "존재하지 않는 계좌번호입니다.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 비밀번호 인증 연동
        // ──────────────────────────────────────────────────────
        const auto &accList = m_bankManager->accountModel()->accounts();
        bool isPasswordMatch = false;
        for (const auto &acc : accList) {
            if (acc.id == accountId && acc.password == password) {
                isPasswordMatch = true;
                break;
            }
        }
        
        if(!isPasswordMatch) {
            QMessageBox::warning(this, "인증 오류", "비밀번호가 일치하지 않습니다.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 삭제 확인 팝업
        // ──────────────────────────────────────────────────────
        QMessageBox::StandardButton reply = QMessageBox::question(
            this,
            "계좌 삭제",
            "정말 삭제하시겠습니까?\n삭제된 계좌는 복구할 수 없습니다.",
            QMessageBox::Yes | QMessageBox::No
            );

        if (reply == QMessageBox::No) return;

        // ──────────────────────────────────────────────────────
        // 계좌 삭제
        // ──────────────────────────────────────────────────────
        bool ok = m_bankManager->removeAccount(accountId);

        if (!ok) {
            QMessageBox::warning(this, "삭제 오류", "계좌 삭제에 실패했습니다.");
            return;
        }

        // ──────────────────────────────────────────────────────
        // 삭제된 계좌가 현재 선택된 계좌면 뷰어 초기화 (Clear)
        // ──────────────────────────────────────────────────────
        if (m_selectedAccountId == accountId) {
            m_selectedAccountId = -1;
            ui->Sel_acc->setText("선택된 계좌 없음");
            ui->CBal_f_lbl->setText("0 원");
            ui->Acc_r_lbl->setText("");
            ui->Date_r_lbl->setText("");
            ui->History_r_lbl->setText("");
            m_proxyModel->setAccountId(-1);
            updateGraph(-1);

            // 버튼 다시 비활성화
            ui->Deposit_Btn->setEnabled(false);
            ui->Withdraw_Btn->setEnabled(false);
            ui->Confirm_Btn->setEnabled(false);
        }

        // 해당 계좌가 남긴 거래 내역(좀비 데이터)도 전부 청소
        m_bankManager->transactionModel()->removeTransactionsByAccountId(accountId);

        // ──────────────────────────────────────────────────────
        // 파일 즉시 저장 (사용자가 끄고 그냥 나갈 경우를 대비해 JSON 강제 동기화)
        // ──────────────────────────────────────────────────────
        on_Save_Btn_clicked();

        QMessageBox::information(this, "계좌 삭제", accountNumber + " 계좌가 삭제되었습니다.");
    }
}

// [슬롯] on_Save_Btn_clicked()
void MainWindow::on_Save_Btn_clicked()
{
    QJsonArray accountsArray;
    for (const auto &acc : m_bankManager->accountModel()->accounts()) {
        QJsonObject o;
        o["id"] = acc.id;
        o["accountNumber"] = acc.accountNumber;
        o["password"] = acc.password;
        o["bankName"] = acc.bankName;
        o["initialBalance"] = acc.initialBalance;
        o["currentBalance"] = acc.currentBalance;
        o["allowOverdraft"] = acc.allowOverdraft;
        o["createdAt"] = acc.createdAt.toMSecsSinceEpoch();
        accountsArray.append(o);
    }

    QJsonArray txArray;
    for (const auto &tx : m_bankManager->transactionModel()->transactions()) {
        QJsonObject o;
        o["id"] = tx.id;
        o["accountId"] = tx.accountId;
        o["amount"] = tx.amount;
        o["type"] = static_cast<int>(tx.type);
        o["status"] = static_cast<int>(tx.status);
        o["memo"] = tx.memo;
        o["counterpartyAccount"] = tx.counterpartyAccount;
        o["occurredAt"] = tx.occurredAt.toMSecsSinceEpoch();
        txArray.append(o);
    }

    QJsonObject root;
    root["accounts"] = accountsArray;
    root["transactions"] = txArray;

    QFile file("account_info.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(root).toJson());
        file.close();
        QMessageBox::information(this, "저장 성공", "계좌 데이터가 성공적으로 저장되었습니다!");
    } else {
        QMessageBox::warning(this, "저장 실패", "계좌 데이터를 저장할 수 없습니다.");
    }
}

// JSON 앱 구동 시 자동 복원 로드
void MainWindow::loadFromFile()
{
    QFile file("account_info.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();

    QJsonObject root = doc.object();
    QJsonArray accountsArray = root["accounts"].toArray();
    for (int i = 0; i < accountsArray.size(); ++i) {
        QJsonObject o = accountsArray[i].toObject();
        Account acc;
        acc.id = o["id"].toInt();
        acc.accountNumber = o["accountNumber"].toString();
        acc.password = o["password"].toString();
        acc.bankName = o["bankName"].toString();
        acc.initialBalance = o["initialBalance"].toVariant().toLongLong();
        acc.currentBalance = o["currentBalance"].toVariant().toLongLong();
        acc.allowOverdraft = o["allowOverdraft"].toBool();
        acc.createdAt = QDateTime::fromMSecsSinceEpoch(o["createdAt"].toVariant().toLongLong());
        m_bankManager->restoreAccount(acc);
    }

    QJsonArray txArray = root["transactions"].toArray();
    for (int i = 0; i < txArray.size(); ++i) {
        QJsonObject o = txArray[i].toObject();
        int txAccountId = o["accountId"].toInt();

        // ────────────────────────────────────────────────
        // 자가 치유(Self-healing): 계좌 모델에 존재하지 않는 accountId를
        // 가진 좀비 거래 내역은 로드하지 않고 버립니다. 과거 버그 방어.
        bool validAccount = false;
        for (const auto &acc : m_bankManager->accountModel()->accounts()) {
            if (acc.id == txAccountId) {
                validAccount = true;
                break;
            }
        }
        if (!validAccount) continue; // 고아 데이터 버림
        // ────────────────────────────────────────────────

        Transaction tx;
        tx.id = o["id"].toInt();
        tx.accountId = txAccountId;
        tx.amount = o["amount"].toVariant().toLongLong();
        tx.type = static_cast<TransactionType>(o["type"].toInt());
        tx.status = static_cast<TransactionStatus>(o["status"].toInt());
        tx.memo = o["memo"].toString();
        tx.counterpartyAccount = o["counterpartyAccount"].toString();
        tx.occurredAt = QDateTime::fromMSecsSinceEpoch(o["occurredAt"].toVariant().toLongLong());
        m_bankManager->restoreTransaction(tx);
    }

    m_bankManager->recalcAllBalances();
}

// 화면 잔고 갱신 & 차트 업데이트 연동
void MainWindow::refreshSummary()
{
    if (m_selectedAccountId == -1) return;

    qint64 balance = 0;
    for (const auto &acc : m_bankManager->accountModel()->accounts()) {
        if (acc.id == m_selectedAccountId) {
            balance = acc.currentBalance;
            break;
        }
    }
    ui->CBal_f_lbl->setText(QString::number(balance) + " 원");

    m_proxyModel->setAccountId(m_selectedAccountId);
    updateGraph(m_selectedAccountId);
}

// Qt Charts를 이용한 실시간 자금 운용 현황
void MainWindow::updateGraph(int accountId) {
    // 기존 그래프 위젯(레이아웃) 무조건 초기화 (Clear)
    QLayout *oldLayout = ui->Graph_Widget->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete oldLayout;
    }

    // -1 이면 빈 그래프 화면 상태로 종료 (계좌 삭제 또는 미선택 시)
    if (accountId < 0) return;

    const auto &transactions = m_bankManager->transactionModel()->transactions();
    const auto &accounts = m_bankManager->accountModel()->accounts();
    
    qint64 initialBal = 0;
    QDateTime createdAt;
    for (const auto &acc : accounts) {
        if (acc.id == accountId) {
            initialBal = acc.initialBalance;
            createdAt = acc.createdAt;
            break;
        }
    }
    
    qint64 currentBal = initialBal;
    QLineSeries *series = new QLineSeries();
    series->append(createdAt.toMSecsSinceEpoch(), currentBal);

    for (const auto &tx : transactions) {
        if (tx.accountId == accountId && tx.status == TransactionStatus::Posted) {
            if (tx.type == TransactionType::Deposit || tx.type == TransactionType::TransferIn) {
                currentBal += tx.amount;
            } else {
                currentBal -= tx.amount;
            }
            series->append(tx.occurredAt.toMSecsSinceEpoch(), currentBal);
        }
    }

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->legend()->hide();

    // Toss 라이트 스타일 그래프 데코레이션
    chart->setTheme(QChart::ChartThemeLight);
    chart->setBackgroundVisible(false); // 배경 투명화
    
    // [가시성 개선] 아주 미니멀한 마진으로 그래프 영역 최대 확보
    chart->setMargins(QMargins(5, 5, 5, 5));

    QPen pen(QColor("#3182F6")); // Toss Blue
    pen.setWidth(4);
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::RoundJoin);
    series->setPen(pen);
    
    // 점(Point) 표시를 추가하여 좀 더 모던하게
    series->setPointsVisible(true);
    series->setPointLabelsVisible(false);

    // ──────────────────────────────────────────
    // X축, Y축 최솟값/최댓값 자동 계산 및 적용
    // ──────────────────────────────────────────
    qint64 minBal = 0, maxBal = 0;
    QDateTime minTime = QDateTime::currentDateTime();
    QDateTime maxTime = QDateTime::currentDateTime();

    if (series->count() > 0) {
        minBal = maxBal = series->at(0).y();
        minTime = QDateTime::fromMSecsSinceEpoch(series->at(0).x());
        maxTime = QDateTime::fromMSecsSinceEpoch(series->at(series->count() - 1).x());

        for (int i = 0; i < series->count(); ++i) {
            qint64 bal = series->at(i).y();
            if (bal < minBal) minBal = bal;
            if (bal > maxBal) maxBal = bal;
        }
    }

    // 마진(여백) 확보
    if (minBal == maxBal) {
        minBal -= 10000;
        maxBal += 10000;
    } else {
        qint64 padding = (maxBal - minBal) * 0.2;
        minBal -= padding;
        maxBal += padding;
    }

    if (minTime >= maxTime) {
        minTime = minTime.addSecs(-3600);
        maxTime = maxTime.addSecs(3600);
    }

    QDateTimeAxis *axisX = new QDateTimeAxis;
    axisX->setTickCount(4);
    axisX->setFormat("MM/dd");
    axisX->setLabelsColor(QColor("#4E5968"));
    axisX->setGridLineColor(QColor("#F2F4F6"));
    axisX->setRange(minTime, maxTime);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis *axisY = new QValueAxis;
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#4E5968"));
    axisY->setGridLineColor(QColor("#F2F4F6"));
    // 폰트 크기 조정으로 잘림 방지
    QFont axisFont("Segoe UI", 9);
    axisX->setLabelsFont(axisFont);
    axisY->setLabelsFont(axisFont);

    axisY->setRange(minBal, maxBal);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setStyleSheet("background: transparent;");

    QVBoxLayout *layout = new QVBoxLayout(ui->Graph_Widget);
    layout->setContentsMargins(0, 0, 0, 0); 
    ui->Graph_Widget->setMinimumHeight(250); // 최소 높이 보장
    layout->addWidget(chartView);
}
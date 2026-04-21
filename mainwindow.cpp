#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

// 생성자
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_selectedAccountId(-1)                   // 초기값 : 선택된 계좌 없음
{
    ui->setupUi(this);
    m_bankManager = new BankManager(this);

    // 거래 내역 테이블뷰와 모델 연결
    ui->Acc_tableview->setModel(m_bankManager->transactionModel());

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

        // [##TODO##] 비밀번호 인증 → Account 파트 연동 후 실제 인증으로 교체
        m_selectedAccountId = accountId;
        ui->Sel_acc->setText("선택된 계좌 : " + accountNumber);

        //계좌 선택 시 버튼 활성화
        ui->Deposit_Btn->setEnabled(true);
        ui->Withdraw_Btn->setEnabled(true);
        ui->Confirm_Btn->setEnabled(true);
        QMessageBox::information(this, "계좌 조회", "계좌가 선택되었습니다.");
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

        // [TODO] 비밀번호 인증 → Account 파트 연동 후 교체

        // 입금 처리
        m_bankManager->deposit(accountId, amount);

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

        // [TODO] 비밀번호 인증 → Account 파트 연동 후 교체

        // 출금 처리 (잔고 부족 시 false 반환)
        bool ok = m_bankManager->withdraw(accountId, amount);
        if (!ok) {
            QMessageBox::warning(this, "출금 오류", "잔고가 부족합니다.");
            return;
        }

        // 결과 라벨 반영
        ui->Date_r_lbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
        ui->Acc_r_lbl->setText(accountNumber);
        ui->History_r_lbl->setText("출금  -" + QString::number(amount) + " 원");

        refreshSummary();
    }
}

// [슬롯] on_Confirm_Btn_clicked()
// 현재 m_currentType 에 따라 입금 or 출금 처리
// 처리 순서 :
//   1. 계좌 선택 여부 확인
//   2. 금액 입력값 수집 및 유효성 검사
//   3. BankManager 에 거래 추가
//   4. 결과 라벨 반영
void MainWindow::on_Confirm_Btn_clicked()
{
    // 1. 계좌 선택 여부 확인
    if (m_selectedAccountId == -1) {
        QMessageBox::warning(this, "계좌 오류", "먼저 계좌를 선택해주세요.\n메뉴바 → 계좌 조회");
        return;
    }

    // 2. 금액 입력값 수집 및 유효성 검사
    QString toAccountNumber = ui->AccountID_lbl->text().trimmed();
    QString password        = ui->Password_lbl->text().trimmed();
    QString amountStr       = ui->Amount_lbl->text().trimmed();

    // 3. 유효성 검사
    if (toAccountNumber.isEmpty() || password.isEmpty() || amountStr.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "모든 항목을 입력해주세요.");
        return;
    }

    bool ok;
    qint64 amount = amountStr.toLongLong(&ok);

    if (!ok || amount <= 0) {
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

    // [TODO] 비밀번호 인증 → Account 파트 연동 후 교체

    // 4. 송금 처리
    bool result = m_bankManager->transfer(m_selectedAccountId, toAccountId, amount);

    if (!result) {
        QMessageBox::warning(this, "송금 오류", "송금에 실패했습니다.\n잔고 부족 또는 동일 계좌 송금입니다.");
        return;
    }

    // 5. 결과 라벨 반영
    ui->Date_r_lbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    ui->Acc_r_lbl->setText(toAccountNumber);
    ui->History_r_lbl->setText("송금  -" + QString::number(amount) + " 원");

    // 입력 필드 초기화
    ui->AccountID_lbl->clear();
    ui->Password_lbl->clear();
    ui->Amount_lbl->clear();

    refreshSummary();
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
        bool ok = m_bankManager->addAccount(accountNumber, "", 0);

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
        // [TODO] 비밀번호 인증 → Account 파트 연동 후 교체
        // ──────────────────────────────────────────────────────

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
        // 삭제된 계좌가 현재 선택된 계좌면 초기화
        // ──────────────────────────────────────────────────────
        if (m_selectedAccountId == accountId) {
            m_selectedAccountId = -1;
            ui->Sel_acc->setText("선택된 계좌 없음");

            // 버튼 다시 비활성화
            ui->Deposit_Btn->setEnabled(false);
            ui->Withdraw_Btn->setEnabled(false);
            ui->Confirm_Btn->setEnabled(false);
        }

        QMessageBox::information(this, "계좌 삭제", "계좌가 삭제되었습니다.");
    }
}

// [슬롯] on_Save_Btn_clicked()
//
// 파일 저장
// [TODO] FileManager 완성 후 연결
void MainWindow::on_Save_Btn_clicked()
{
    QMessageBox::information(this, "저장", "저장 기능은 준비 중입니다.");
}

// 화면 잔고 갱신
// [TODO] Account 파트 연동 후 실제 잔고 표시로 교체
void MainWindow::refreshSummary()
{
    // [TODO] 잔고 라벨 업데이트
    // ui->CBal_f_lbl->setText(QString::number(balance) + " 원");
}
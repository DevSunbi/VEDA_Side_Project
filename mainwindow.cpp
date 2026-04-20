#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

// 생성자
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_currentType(TransactionType::Deposit)   // 초기 모드 : 입금
    , m_selectedAccountId(-1)                   // 초기값 : 선택된 계좌 없음
{
    ui->setupUi(this);
    m_bankManager = new BankManager(this);

    // 거래 내역 테이블뷰와 모델 연결
    ui->Acc_tableview->setModel(m_bankManager->transactionModel());
}

// 소멸자
MainWindow::~MainWindow()
{
    delete ui;
}

// on_Check_Acc_triggered()
// menubar → 계좌 조회 클릭 시 호출
// AccSearchDialog 팝업 → 인증 성공 시 m_selectedAccountId 저장
void MainWindow::on_Check_Acc_triggered()
{
    AccSearchDialog dlg(this);

    // [C++ 개념] exec()
    //   - 다이얼로그를 모달(modal)로 실행해요
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
        QMessageBox::information(this, "계좌 조회", "계좌가 선택되었습니다.");
    }
}

// on_InsertAcc_triggered()
// menubar → 계좌 생성 클릭 시 호출
// [TODO] AddAccDialog 완성 후 연결
void MainWindow::on_InsertAcc_triggered()
{
    QMessageBox::information(this, "계좌 생성", "계좌 생성 기능은 준비 중입니다.");
}

//  on_DeleteAcc_triggered()
// menubar → 계좌 삭제 클릭 시 호출
// [TODO] DeleteAccDialog 완성 후 연결
void MainWindow::on_DeleteAcc_triggered()
{
    QMessageBox::information(this, "계좌 삭제", "계좌 삭제 기능은 준비 중입니다.");
}

// on_Deposit_Btn_clicked()
// 입금 모드로 설정 후 팝업으로 알림
void MainWindow::on_Deposit_Btn_clicked()
{
    m_currentType = TransactionType::Deposit;
    QMessageBox::information(this, "모드 선택", "입금 모드로 설정되었습니다.");
}

// on_Withdraw_Btn_clicked()
// 출금 모드로 설정 후 팝업으로 알림
void MainWindow::on_Withdraw_Btn_clicked()
{
    m_currentType = TransactionType::Withdraw;
    QMessageBox::information(this, "모드 선택", "출금 모드로 설정되었습니다.");
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
    QString amountStr = ui->Amount_lbl->text().trimmed();

    if (amountStr.isEmpty()) {
        QMessageBox::warning(this, "입력 오류", "금액을 입력해주세요.");
        return;
    }

    bool ok;
    qint64 amount = amountStr.toLongLong(&ok);

    if (!ok || amount <= 0) {
        QMessageBox::warning(this, "입력 오류", "올바른 금액을 입력해주세요.");
        return;
    }

    // 3. BankManager 에 거래 추가
    QString memo = "";  // [##TODO##] 메모 입력 필드 추가 시 연결
    m_bankManager->addTransaction(m_selectedAccountId, amount, m_currentType, memo);

    // 4. 결과 라벨 반영
    QString typeStr = (m_currentType == TransactionType::Deposit) ? "입금" : "출금";
    QString sign    = (m_currentType == TransactionType::Deposit) ? "+"    : "-";

    ui->Date_r_lbl->setText(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm"));
    ui->History_r_lbl->setText(typeStr + "  " + sign + QString::number(amount) + " 원");

    // 입력 필드 초기화
    ui->Amount_lbl->clear();

    refreshSummary();
}

// [슬롯] on_Re_Btn_clicked()
//
// 정정(취소) 처리
// [TODO] 테이블에서 선택된 거래 취소 후 잔고 롤백
void MainWindow::on_Re_Btn_clicked()
{
    QMessageBox::information(this, "정정", "정정 기능은 준비 중입니다.");
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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include "bankmanager.h"
#include "transaction.h"
#include "AccSearchDialog.h"
#include "DepositDialog.h"
#include "WithdrawDialog.h"
#include "AddAccDialog.h"
#include "DeleteAccDialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


// 메인 화면 클래스
// 사용자의 클릭을 처리하고 BankManager 에게 일을 처리

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // menubar 액션 슬롯
    void on_Check_Acc_triggered();      // 계좌 조회 → AccSearchDialog 팝업
    void on_InsertAcc_triggered();      // 계좌 생성 → AddAccDialog 팝업
    void on_DeleteAcc_triggered();      // 계좌 삭제 → DeleteAccDialog 팝업

    // Select_Group 버튼 슬롯
    // 모드 설정만 하고 실제 처리는 Confirm_Btn
    void on_Deposit_Btn_clicked();      // 입금 처리
    void on_Withdraw_Btn_clicked();     // 출금 처리
    void on_Save_Btn_clicked();         // 파일 저장

    // Transfer_Group 버튼 슬롯
    void on_Confirm_Btn_clicked();      // 실제 입금/출금 처리

private:
    Ui::MainWindow *ui;
    BankManager *m_bankManager;     // 모든 로직을 담당하는 매니저

    // menubar → Check_Acc 에서 인증 후 저장되는 현재 계좌 ID
    // -1 이면 아직 선택된 계좌 없음
    int m_selectedAccountId;

    // UI 보조 함수
    void refreshSummary();              // 화면 잔고 갱신
};



#endif // MAINWINDOW_H
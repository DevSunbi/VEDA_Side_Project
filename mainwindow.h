#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "bankmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// =======================================================================
// 메인 화면 클래스 (사용자의 클릭을 처리하고 매니저에게 일을 시킴)
// =======================================================================
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    // ── UI 버튼 클릭 이벤트 (슬롯) ──
    void on_pushButton_calc_clicked();      // [입금] 버튼
    void on_pushButton_save_clicked();      // [출금] 버튼
    void on_pushButton_login_clicked();     // [송금] 버튼
    void on_pushButton_reset_clicked();     // [정정/취소] 버튼
    void on_pushButton_help_clicked();      // [저장] 버튼

    // ── 메뉴/기타 이벤트 ──
    void on_actionAdd_triggered();          // 계좌 추가 메뉴
    void on_actionDelete_triggered();       // 계좌 삭제 메뉴
    void on_tabWidget_semester_currentChanged(int index); // 탭 전환 시

private:
    Ui::MainWindow *ui;
    BankManager    *m_bankManager; // 우리 프로그램의 모든 로직을 담당하는 매니저

    // ── UI 보조 함수 (구현 힌트) ──
    int currentAccountId() const;    // [TODO] 현재 선택된 탭의 계좌 ID를 가져오는 함수
    void refreshSummary();           // [TODO] 화면에 잔고 숫자를 다시 써주는 함수
};

#endif // MAINWINDOW_H

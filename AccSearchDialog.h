#ifndef ACCSEARCHDIALOG_H
#define ACCSEARCHDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

// menubar → 계좌 조회(Check_Acc) 클릭 시 팝업되는 다이얼로그
// 역할 : 계좌번호 + 비밀번호 입력받아 MainWindow 로 반환
class AccSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccSearchDialog(QWidget *parent = nullptr);
    ~AccSearchDialog();

    QString getAccountNumber() const;   // 입력된 계좌번호 반환
    QString getPassword() const;   // 입력된 비밀번호 반환

private:
    Ui::Dialog *ui;
};

#endif // ACCSEARCHDIALOG_H
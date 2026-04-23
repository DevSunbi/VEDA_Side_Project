#ifndef DELETEACCDIALOG_H
#define DELETEACCDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class DeleteAccDialog;
}
QT_END_NAMESPACE


// 역할 : 계좌번호 + 비밀번호 + 비밀번호 확인 입력받아 MainWindow 로 반환
class DeleteAccDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteAccDialog(QWidget *parent = nullptr);
    ~DeleteAccDialog();

    QString getAccountNumber()    const;    // 입력된 계좌번호 반환
    QString getPassword()         const;    // 입력된 비밀번호 반환
    QString getPasswordConfirm()  const;    // 입력된 비밀번호 확인 반환

private:
    Ui::DeleteAccDialog *ui;
};

#endif // DELETEACCDIALOG_H
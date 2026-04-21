#ifndef ADDACCDIALOG_H
#define ADDACCDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class AddAccDialog;
}
QT_END_NAMESPACE

class AddAccDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddAccDialog(QWidget *parent = nullptr);
    ~AddAccDialog();

    QString getAccountNumber() const;   // 입력된 계좌번호 반환
    QString getPassword()      const;   // 입력된 비밀번호 반환
    QString getPasswordConfirm() const; // 입력된 비밀번호 확인 반환

private:
    Ui::AddAccDialog *ui;
};

#endif // ADDACCDIALOG_H
#ifndef ACCSEARCHDIALOG_H
#define ACCSEARCHDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class AccSearchDialog;
}
QT_END_NAMESPACE

class AccSearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AccSearchDialog(QWidget *parent = nullptr);
    ~AccSearchDialog();

    QString getAccountNumber() const;   // 입력된 계좌번호 반환
    QString getPassword() const;   // 입력된 비밀번호 반환

private:
    Ui::AccSearchDialog *ui;
};

#endif // ACCSEARCHDIALOG_H
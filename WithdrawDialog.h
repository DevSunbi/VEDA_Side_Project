#ifndef WITHDRAWDIALOG_H
#define WITHDRAWDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class WithdrawDialog;
}
QT_END_NAMESPACE

class WithdrawDialog : public QDialog
{
    Q_OBJECT

public:
    explicit WithdrawDialog(QWidget *parent = nullptr);
    ~WithdrawDialog();

    QString getAccountNumber() const;
    QString getPassword() const;
    qint64  getAmount() const;

private:
    Ui::WithdrawDialog *ui;
};

#endif // WITHDRAWDIALOG_H
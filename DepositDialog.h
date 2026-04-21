#ifndef DEPOSITDIALOG_H
#define DEPOSITDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class DepositDialog;
}
QT_END_NAMESPACE

class DepositDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositDialog(QWidget *parent = nullptr);
    ~DepositDialog();

    QString getAccountNumber() const;
    QString getPassword() const;
    qint64  getAmount() const;

private:
    Ui::DepositDialog *ui;
};

#endif // DEPOSITDIALOG_H
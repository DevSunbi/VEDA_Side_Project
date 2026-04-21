#ifndef TRANSFERDIALOG_H
#define TRANSFERDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class TransferDialog;
}
QT_END_NAMESPACE

class TransferDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TransferDialog(QWidget *parent = nullptr);
    ~TransferDialog();

    QString getToAccountNumber() const;
    QString getPassword() const;
    qint64  getAmount() const;

private:
    Ui::TransferDialog *ui;
};

#endif // TRANSFERDIALOG_H

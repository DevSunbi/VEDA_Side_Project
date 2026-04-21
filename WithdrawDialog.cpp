#include "WithdrawDialog.h"
#include "ui_withdraw.h"

WithdrawDialog::WithdrawDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WithdrawDialog)
{
    ui->setupUi(this);
    setWindowTitle("출금");
}

WithdrawDialog::~WithdrawDialog()
{
    delete ui;
}

QString WithdrawDialog::getAccountNumber() const
{
    return ui->w_acc_le->text().trimmed();
}

QString WithdrawDialog::getPassword() const
{
    return ui->w_pass_le->text().trimmed();
}

qint64 WithdrawDialog::getAmount() const
{
    bool ok;
    qint64 amount = ui->w_amount_le->text().trimmed().toLongLong(&ok);
    return ok ? amount : 0;
}
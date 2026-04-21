#include "DepositDialog.h"
#include "ui_deposit.h"

DepositDialog::DepositDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DepositDialog)
{
    ui->setupUi(this);
    setWindowTitle("입금");
}

DepositDialog::~DepositDialog()
{
    delete ui;
}

QString DepositDialog::getAccountNumber() const
{
    return ui->d_acc_le->text().trimmed();
}

QString DepositDialog::getPassword() const
{
    return ui->d_pass_le->text().trimmed();
}

qint64 DepositDialog::getAmount() const
{
    bool ok;
    qint64 amount = ui->d_amount_le->text().trimmed().toLongLong(&ok);
    return ok ? amount : 0;
}
#include "TransferDialog.h"
#include "ui_transfer.h"

TransferDialog::TransferDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TransferDialog)
{
    ui->setupUi(this);
}

TransferDialog::~TransferDialog()
{
    delete ui;
}

// 송금 다이얼로그 입력값 반환 (송금할 계좌 번호 / 비밀번호 / 송금 금액)
QString TransferDialog::getToAccountNumber() const
{
    return ui->d_acc_le->text();
}

QString TransferDialog::getPassword() const
{
    return ui->d_pass_le->text();
}

qint64 TransferDialog::getAmount() const
{
    bool ok;
    qint64 amt = ui->d_amount_le->text().toLongLong(&ok);
    return ok ? amt : -1;
}

#include "WithdrawDialog.h"
#include "ui_withdraw.h"

WithdrawDialog::WithdrawDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WithdrawDialog)
{
    ui->setupUi(this);
    setWindowTitle("출금");
    ui->w_pass_le->setEchoMode(QLineEdit::Password);
}

WithdrawDialog::~WithdrawDialog()
{
    delete ui;
}

// 출금 다이얼로그 입력값 반환 (현재 계좌 번호 / 비밀번호 / 출금 금액)
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
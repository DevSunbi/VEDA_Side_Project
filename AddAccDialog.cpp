#include "AddAccDialog.h"
#include "ui_add_acc.h"     // add_acc.ui 자동 생성 헤더

AddAccDialog::AddAccDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AddAccDialog)
{
    ui->setupUi(this);
    
    // 비밀번호 보안 마스킹
    ui->addpass_le->setEchoMode(QLineEdit::Password);
    ui->addPassRe_le->setEchoMode(QLineEdit::Password);
}

AddAccDialog::~AddAccDialog()
{
    delete ui;
}

//입력된 계좌번호 반환
QString AddAccDialog::getAccountNumber() const
{
    return ui->addAc_le->text().trimmed();
}


// 입력된 비밀번호 반환
QString AddAccDialog::getPassword() const
{
    return ui->addpass_le->text().trimmed();
}

// 입력된 확인 비밀번호  반환
QString AddAccDialog::getPasswordConfirm() const
{
    return ui->addPassRe_le->text().trimmed();
}
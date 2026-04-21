#include "AccSearchDialog.h"
#include "ui_Acc_search.h"    // Acc_search.ui 자동 생성 헤더


AccSearchDialog::AccSearchDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::AccSearchDialog)
{
    ui->setupUi(this);
    setWindowTitle("계좌 조회");
}

AccSearchDialog::~AccSearchDialog()
{
    delete ui;
}

QString AccSearchDialog::getAccountNumber() const
{
    return ui->s_acc_le->text().trimmed();
}

// [Getter] 입력된 비밀번호 반환
QString AccSearchDialog::getPassword() const
{
    return ui->s_pass_le->text().trimmed();
}
#include "DeleteAccDialog.h"
#include "ui_delete_acc.h"      // delete_acc.ui 자동 생성 헤더

// ══════════════════════════════════════════════════════════════
// [생성자] DeleteAccDialog::DeleteAccDialog()
// ══════════════════════════════════════════════════════════════
DeleteAccDialog::DeleteAccDialog(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::DeleteAccDialog)
{
    ui->setupUi(this);
    setWindowTitle("계좌 삭제");
}

// ══════════════════════════════════════════════════════════════
// [소멸자] DeleteAccDialog::~DeleteAccDialog()
// ══════════════════════════════════════════════════════════════
DeleteAccDialog::~DeleteAccDialog()
{
    delete ui;
}

// ══════════════════════════════════════════════════════════════
// [Getter] 입력된 계좌번호 반환
// ══════════════════════════════════════════════════════════════
QString DeleteAccDialog::getAccountNumber() const
{
    return ui->delAc_le->text().trimmed();
}

// ══════════════════════════════════════════════════════════════
// [Getter] 입력된 비밀번호 반환
// ══════════════════════════════════════════════════════════════
QString DeleteAccDialog::getPassword() const
{
    return ui->delpass_le->text().trimmed();
}

// ══════════════════════════════════════════════════════════════
// [Getter] 입력된 비밀번호 확인 반환
// ══════════════════════════════════════════════════════════════
QString DeleteAccDialog::getPasswordConfirm() const
{
    return ui->delPassRe_le->text().trimmed();
}
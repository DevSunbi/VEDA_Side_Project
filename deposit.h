#ifndef DEPOSITDIALOG_H
#define DEPOSITDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
namespace Ui {
class Dialog;
}
QT_END_NAMESPACE

// 역할 : 계좌번호 + 비밀번호 + 입금 금액 입력받아 MainWindow 로 반환

class DepositDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DepositDialog(QWidget *parent = nullptr);
    ~DepositDialog();
    //getter
    QString getAccountNumber() const;   // 입력된 계좌번호 반환
    QString getPassword()      const;   // 입력된 비밀번호 반환
    qint64  getAmount()        const;   // 입력된 입금 금액 반환

private:
    Ui::Dialog *ui;
};

#endif // DEPOSITDIALOG_H
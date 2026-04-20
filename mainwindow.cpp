#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // TODO: 초기 화면 및 탭 세팅
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ── UI 편의 함수들 ────────────────────────────────────────────────────────
int MainWindow::currentAccountId() const
{
    // TODO: 현재 탭의 index를 이용해 m_accounts 의 id 구하기
    return -1;
}

void MainWindow::refreshTransactionList()
{
    // TODO: UI (테이블 등) 에 m_transactions 내용을 싹 다 지우고 새로 그리기
}

void MainWindow::refreshSummary()
{
    // TODO: 총 잔고, 현재 계좌 잔고 등을 화면의 lineEdit 에 적어주기
}

void MainWindow::recalcAllBalances()
{
    // 1. 모든 계좌 잔고를 초기 잔고로 돌림
    for(auto &acc : m_accounts) {
        acc.currentBalance = acc.initialBalance;
    }

    // 2. 모든 거래 내역을 하나씩 돌아가면서 잔고에 더하고 빼기 (초보자형 직관적 로직)
    for(const auto &tx : m_transactions) {
        if(tx.status != "정상") continue; // 취소된 내역은 무산됨

        // 이 거래가 발생한 계좌 찾기
        for(auto &acc : m_accounts) {
            if(acc.id == tx.accountId) {
                if(tx.type == "입금") {
                    acc.currentBalance += tx.amount;
                } else if(tx.type == "출금" || tx.type == "송금") {
                    // (송금 기능 구현 방식에 따라 다르겠지만, 내 계좌에서 돈이 빠진다면 출금)
                    acc.currentBalance -= tx.amount;
                }
                break;
            }
        }
    }
}

// ── 메뉴 버튼 클릭 함수들 (계좌 생성/삭제 등) ─────────────────────────────────

void MainWindow::on_actionAdd_triggered()
{
    // 화면(Input)에서 이름, 계좌번호 등 입력값을 가져왔다고 치는 예시 테스트 코드
    Account accountData;
    accountData.name = "테스트 계좌";
    accountData.accountNumber = "123-456-789";   // 이 부분을 UI 텍스트 상자에서 읽어와야 함!
    
    QString outError = ""; // 에러 메시지 담을 변수

    // 1. 방금 구성하신 아주 훌륭하고 명료한 예외 체크!
    if(accountData.accountNumber.isEmpty()) {
        outError = "계좌 번호가 입력되지 않았습니다.";
    }

    // 2. for문 돌려서 중복 계좌 찾기 (매니저 클래스 없이 그냥 MainWindow 안에서!)
    for(const auto &acc : m_accounts)
    {
        if(acc.accountNumber == accountData.accountNumber)
        {
            outError = "이미 존재하는 계좌 번호입니다.";
            break;
        }
    }

    // 에러가 있다면 알림창 띄우고 아래로 못 내려가게 함수 끝내기
    if(!outError.isEmpty()) {
        QMessageBox::warning(this, "경고", outError);
        return;
    }

    // 3. 문제가 없다면 멤버 리스트에 진짜 넣기
    Account newAccount = accountData;
    newAccount.id = m_nextAccountId++; // 숫자 1 증가시켜서 ID로 줌
    
    m_accounts.append(newAccount);

    QMessageBox::information(this, "성공", "계좌가 무사히 생성되었습니다!");

    // TODO: 이 아래에 UI(tabWidget)에도 새 탭 하나 추가해주는 코드 넣기
}

void MainWindow::on_actionDelete_triggered()
{
    int currentId = currentAccountId();
    if(currentId == -1) return;

    // 해당 계좌 찾아서 문자열 상태만 "비활성"으로 변경
    for(auto &acc : m_accounts) {
        if(acc.id == currentId) {
            acc.status = "비활성"; // 복잡한 enum 없이 그냥 글씨로!!
            QMessageBox::information(this, "성공", "계좌가 삭제(비활성) 처리 되었습니다.");
            break;
        }
    }
}

// ── 주요 기능 버튼 클릭 (입금, 출금, 송금) ───────────────────────────────────

void MainWindow::on_pushButton_calc_clicked()   // 입금
{
    int currentId = currentAccountId();
    if(currentId == -1) return;

    // TODO: UI에서 금액, 메모 등 읽어오기
    qint64 inputAmount = 10000; 

    // 금액 예외 처리
    if(inputAmount <= 0) {
        QMessageBox::warning(this, "오류", "입금액은 0보다 커야 합니다.");
        return;
    }

    // 1. 거래 내역 구조체 만들기
    Transaction tx;
    tx.id = m_nextTransactionId++;
    tx.accountId = currentId;
    tx.amount = inputAmount;
    tx.type = "입금";      // enum 대신 문자 대입!
    tx.status = "정상";    // enum 대신 문자 대입!
    
    // 2. 내역 리스트에 저장
    m_transactions.append(tx);

    // 3. 잔고 처음부터 다 계산
    recalcAllBalances();
    
    // 4. 화면 새로 쓰기
    refreshTransactionList();
    refreshSummary();

    QMessageBox::information(this, "완료", "입금 완료!");
}

void MainWindow::on_pushButton_save_clicked()   // 출금
{
    // 입금과 비슷하게 복사-붙여넣기 하면 됩니다. 
    // "잔고가 마이너스로 가는지 체크하는 if문"만 하나 더 넣으면 끝입니다!
}

void MainWindow::on_pushButton_login_clicked()  // 송금
{
    // 출금 한번 하고, 입금 한번 하는 식으로 Transaction 2개를 구조체로 만들어서 append 하면 끝납니다!
}

void MainWindow::on_pushButton_reset_clicked()  // 정정 (또는 취소)
{
    // UI에서 선택된 거래 ID를 받아서, for문으로 m_transactions에서 찾은 뒤 
    // status 문자열을 "취소"로 바꾸고 recalcAllBalances() 한 번만 실행하면 모든게 끝나는 마법!
}

void MainWindow::on_pushButton_help_clicked()   // 내역 저장
{
    // CSV 파일 내보내기 구현
}

// ── 탭 바뀔 때 ────────────────────────────────────────────────────────────
void MainWindow::on_tabWidget_semester_currentChanged(int index)
{
    Q_UNUSED(index)
    refreshTransactionList();
    refreshSummary();
}

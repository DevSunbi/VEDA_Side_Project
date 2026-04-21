#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 전역 스타일시트 적용 (Toss Style)
    QFile file(":/styles.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
        file.close();
    } else {
        // 리소스 로드 실패 시 디버깅을 위한 경고 (실제 배포 시에는 제거 권장)
        // 만약 이 경고창이 뜬다면 'Build -> Clean All' 후 다시 'Build' 하셔야 합니다.
        // QMessageBox::warning(nullptr, "UI 테마 로드 실패", "styles.qss 리소스를 찾을 수 없습니다.\n프로젝트를 'Rebuild' 해주세요.");
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}

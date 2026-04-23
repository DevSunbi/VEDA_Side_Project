#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QMessageBox>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 전역 스타일시트 적용
    QFile file(":/styles.qss");
    if (file.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(file.readAll());
        a.setStyleSheet(styleSheet);
        file.close();
    } else {
        // QMessageBox::warning(nullptr, "UI 테마 로드 실패", "styles.qss 리소스를 찾을 수 없습니다.\n프로젝트를 리빌드 해주세요.");
    }

    MainWindow w;
    w.show();
    return QApplication::exec();
}

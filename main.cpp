#include "gamewindow.h"
#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GameWindow w;
    w.show();

/*
    QString styleSheet;
    auto* qssFile = new QFile(":/stylesheet/dark");
    if(qssFile->open(QFile::ReadOnly)) {
        styleSheet = QString(qssFile->readAll());
        qApp->setStyleSheet(styleSheet);
        qssFile->close();
    }
*/


    return a.exec();
}

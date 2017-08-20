#include "geekspacewindow.h"
#include <QApplication>
#include <QTextCodec>

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    QApplication a(argc, argv);
    GeekSpaceWindow w;
    w.show();



    return a.exec();
}

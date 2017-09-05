#include "geekspacewindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QStandardPaths>

#ifdef Q_OS_WIN
#include <windows.h>

QString shortPathName(const QString & file) // returns 8.3 file format from long path
{
    wchar_t* input = new wchar_t[file.size()+1];
    file.toWCharArray(input);
    input[file.size()] = L'\0'; // terminate string
    long length = GetShortPathName(input, NULL, 0);
    wchar_t* output = new wchar_t[length];
    GetShortPathName(input,output,length);
    QString ret = QString::fromWCharArray(output,length-1); // discard
    delete [] input;
    delete [] output;
    return ret;
}
#endif

QString userHome;
QString scriptsFolder;
QString draftScriptsFolder;

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("Windows-1251"));

    userHome = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    draftScriptsFolder = userHome + "/GeekSpace/DraftScripts/";
    scriptsFolder = userHome + "/GeekSpace/Scripts/";

    QApplication a(argc, argv);
    GeekSpaceWindow w;
    w.show();



    return a.exec();
}

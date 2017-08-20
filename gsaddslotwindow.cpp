#include "gsaddslotwindow.h"
#include "ui_gsaddslotwindow.h"

#include <QMdiSubWindow>
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QTextCodec>

#include "gstextedit.h"


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

GSAddSlotWindow::GSAddSlotWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GSAddSlotWindow)
{
    ui->setupUi(this);
    ui->dockActions->setWidget(ui->treeActions);
    ui->dockConsole->setWidget(ui->console);

    //ui->mdiArea->addSubWindow();

    setCentralWidget(ui->horizontalLayoutWidget);

    processCompiller = new QProcess(this);

    connect(processCompiller, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromCompiller()));
    connect(processCompiller, SIGNAL(readyReadStandardError()), this, SLOT(readErrorCompiller()));
    connect(processCompiller, SIGNAL(finished(int)), this, SLOT(finishedCompiller(int)));

    connect(ui->dockConsole, SIGNAL(visibilityChanged(bool)), this, SLOT(on_actionConsole_triggered(bool)));
    connect(ui->dockActions, SIGNAL(visibilityChanged(bool)), this, SLOT(on_actionActions_triggered(bool)));
}

void GSAddSlotWindow::addNewSlot()
{
    on_actionNew_triggered();
}

GSAddSlotWindow::~GSAddSlotWindow()
{
    delete ui;
}

void GSAddSlotWindow::on_actionNew_triggered()
{
    GSTextEdit *newEdit = new GSTextEdit(this);
    QMdiSubWindow *newWindow = ui->mdiArea->addSubWindow(newEdit);
    newEdit->show();
    newWindow->setWindowTitle(QString("newSlot%1").arg(ui->mdiArea->subWindowList().count()));
}

void GSAddSlotWindow::on_actionCheck_triggered()
{

    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();

    QString userHome = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString currentTabName = currentWindow->windowTitle();
    QString saveFolder = userHome + "/GeekSpace/DraftSlots/";
    QString fullPath = saveFolder + currentTabName+".gslc";

    QDir dir(saveFolder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    printMessage(QString("Проверяем файл '%1'.").arg(currentTabName));

    QFile outFile(fullPath);
    if(outFile.open(QFile::WriteOnly)){
        outFile.write(((GSTextEdit*)(currentWindow->widget()))->toPlainText().toUtf8());
        printMessage(QString("Файл записан в '%1'.").arg(fullPath));
    }
    else {
        printMessage(QString("Ошибка записи файла '%1'.").arg(fullPath));
        return;
    }

    //    QTextCodec *codec = QTextCodec::codecForName("CP866");
    //    QByteArray encodedString = codec->fromUnicode(fullPath);

    QString program = "Compiller/GeekSpaceCompiller.exe";
    QString workPath = saveFolder;

#ifdef Q_OS_WIN
    fullPath = shortPathName(fullPath);
    workPath = shortPathName(workPath);
#endif

    processCompiller->setWorkingDirectory(workPath);
    QStringList arguments = QStringList() << fullPath
                                          << "-A"
                                          << "-o"
                                          << (currentTabName + ".gsbc");


    processCompiller->start(program, arguments);
    if(processCompiller->state() == QProcess::NotRunning) {
        printMessage("Ошибка запуска компиляции");
        return;
    }

    printMessage("Компиляция...");
}

void GSAddSlotWindow::printMessage(QString msg)
{
    if(!ui->dockConsole->isVisible())
        ui->dockConsole->setVisible(true);
    ui->console->append(msg);
}

void GSAddSlotWindow::readFromCompiller()
{
    QString compillerOutput = processCompiller->readAllStandardOutput();
    printMessage(compillerOutput);
}

void GSAddSlotWindow::readErrorCompiller()
{
    QString compillerError = processCompiller->readAllStandardError();
    printMessage(QString("Ошибка в процессе компиляции:\r\n%1").arg(compillerError));
}

void GSAddSlotWindow::finishedCompiller(int exitCode)
{
    if(exitCode > 0) {
        printMessage(QString("Ошибка процесс компиляции завершился с кодом %1").arg(exitCode));
    }
    else {
        printMessage("Компиляция завершена");
    }
}

void GSAddSlotWindow::on_actionUndo_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
    if(currentWindow != NULL) {
        ((QPlainTextEdit *)(currentWindow->widget()))->undo();
    }
}

void GSAddSlotWindow::on_actionRedo_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
    if(currentWindow != NULL) {
        ((QPlainTextEdit *)(currentWindow->widget()))->redo();
    }
}

void GSAddSlotWindow::on_actionConsole_triggered(bool checked)
{
    ui->dockConsole->setVisible(checked);
    ui->actionConsole->setChecked(checked);
}

void GSAddSlotWindow::on_actionActions_triggered(bool checked)
{
    ui->dockActions->setVisible(checked);
    ui->actionActions->setChecked(checked);
}

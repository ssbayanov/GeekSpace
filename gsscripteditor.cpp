#include "gsscripteditor.h"
#include "ui_gsscripteditor.h"

#include <QMdiSubWindow>
#include <QFile>
#include <QDir>
#include <QTextCodec>

#include "gstextedit.h"
#include "gssavescriptdialog.h"


#ifdef Q_OS_WIN
extern QString shortPathName(const QString & file);
#endif

GSScriptEditor::GSScriptEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GSScriptEditor)
{
    ui->setupUi(this);

    ui->dockConsole->setWidget(ui->console);
    connect(ui->dockConsole, SIGNAL(visibilityChanged(bool)), this, SLOT(consoleVisibleChanged(bool)));
    connect(ui->actionConsole, SIGNAL(triggered(bool)), ui->dockConsole, SLOT(setVisible(bool)));

    ui->dockActions->setWidget(ui->treeActions);
    connect(ui->dockActions, SIGNAL(visibilityChanged(bool)), this, SLOT(actionsVisibleChanged(bool)));
    connect(ui->actionActions, SIGNAL(triggered(bool)), ui->dockActions, SLOT(setVisible(bool)));

    //ui->mdiArea->addSubWindow();

    setCentralWidget(ui->horizontalLayoutWidget);

    saveDialog = new GSSaveScriptDialog(this);

    processCompiller = new QProcess(this);

    connect(processCompiller, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromCompiller()));
    connect(processCompiller, SIGNAL(readyReadStandardError()), this, SLOT(readErrorCompiller()));
    connect(processCompiller, SIGNAL(finished(int)), this, SLOT(finishedCompiller(int)));

    connect(ui->dockConsole, SIGNAL(visibilityChanged(bool)), this, SLOT(consoleVisibleChanged(bool)));
    connect(ui->dockActions, SIGNAL(visibilityChanged(bool)), this, SLOT(actionsVisibleChanged(bool)));

    _modules  = new QTreeWidgetItem(ui->treeActions, QStringList() << "Модули");
    _modules->setData(0, Qt::UserRole, TI_Type);

    _timers = new QTreeWidgetItem(ui->treeActions, QStringList() << "Таймеры");
    _timers->setData(0, Qt::UserRole, TI_Type);

    _sheldules  = new QTreeWidgetItem(ui->treeActions, QStringList() << "Расписания");
    _sheldules->setData(0, Qt::UserRole, TI_Type);

    _processes  = new QTreeWidgetItem(ui->treeActions, QStringList() << "Процессы");
    _processes->setData(0, Qt::UserRole, TI_Type);

    _scripts = new QTreeWidgetItem(ui->treeActions, QStringList() << "Сценарии");
    _scripts->setData(0, Qt::UserRole, TI_Type);
}

void GSScriptEditor::addNewSlot()
{
    on_actionNew_triggered();
}

void GSScriptEditor::loadObjects(ObjectsHash objects)
{
    _objects = objects;

    clearItem(_modules);
    clearItem(_timers);
    clearItem(_sheldules);
    clearItem(_processes);
    clearItem(_scripts);

    ObjectsList objs = objects.values();

    for(int i = 0; i < objs.count(); i++) {
        switch (objs[i]->type()) {
        case GSSheldule::TO_Module:
            loadObjectTo(objs[i], _modules);
            break;
        case GSSheldule::TO_Process:
            loadObjectTo(objs[i], _processes);
            break;
        case GSSheldule::TO_Script:
            loadObjectTo(objs[i], _scripts);
            break;
        case GSSheldule::TO_Sheldule:
            loadObjectTo(objs[i], _sheldules);
            break;
        case GSSheldule::TO_Timer:
            loadObjectTo(objs[i], _timers);
            break;
        default:
            break;
        }
    }
}

void GSScriptEditor::loadObjectTo(GSObject *what, QTreeWidgetItem *item)
{
    QTreeWidgetItem *newObject = new QTreeWidgetItem(item,
                                                     QStringList() << what->alias());
    newObject->setData(0, Qt::UserRole, TI_Object);

    for(int i = 0; i < what->slotsList().count(); i++) {
        QTreeWidgetItem *newSlot = new QTreeWidgetItem(newObject, QStringList() << what->slotsList()[i] + "()");
        newSlot->setData(0, Qt::UserRole, TI_Slot);
    }
}

GSScriptEditor::~GSScriptEditor()
{
    delete ui;
}

void GSScriptEditor::editScript(GSScript *script)
{

    if(!isVisible()) {
        ui->mdiArea->closeAllSubWindows();
        show();
    }

    QList<QMdiSubWindow *> windowList = ui->mdiArea->subWindowList();

    for(int i = 0; i < windowList.count(); i++) {
        if(windowList[i]->windowTitle() == script->alias()){
            windowList[i]->activateWindow();
            return;
        }
    }

    GSTextEdit *newEdit = new GSTextEdit(this);
    QMdiSubWindow *newWindow = ui->mdiArea->addSubWindow(newEdit);
    newEdit->show();
    connect(newEdit, &GSTextEdit::textChanged, [=] () {
        newWindow->setProperty("isEdit", true);
    });

    extern QString scriptsFolder;
    QString fullPath = scriptsFolder + script->alias() + ".gslc";
    QString scriptText;
    QFile inFile(fullPath);
    if(inFile.open(QFile::ReadOnly)){
        scriptText = inFile.readAll();
    }
    else {
        writeMessage(QString("Ошибка чтения файла '%1'.").arg(fullPath));
        return;
    }

    newEdit->setPlainText(scriptText);
    newWindow->setWindowTitle(script->alias());
    newWindow->setProperty("type", TSW_Edit);
    newWindow->setProperty("scriptAddress", (uint32_t) script);
    newWindow->setProperty("isEdit", false);

    //connect(newWindow, SIGNAL(destroyed(QObject*), );
    //connect(ui->mdiArea, SIGNAL())
}

void GSScriptEditor::exec()
{
    //setWindowModality(Qt::ApplicationModal);
    if(!isVisible()) {
        ui->mdiArea->closeAllSubWindows();
        show();
    }
    on_actionNew_triggered();
}

void GSScriptEditor::on_actionNew_triggered()
{
    GSTextEdit *newEdit = new GSTextEdit(this);
    QMdiSubWindow *newWindow = ui->mdiArea->addSubWindow(newEdit);
    newEdit->show();
    newWindow->setWindowTitle(QString("newSlot%1").arg(ui->mdiArea->subWindowList().count()));
    newWindow->setProperty("type", TSW_New);
}

void GSScriptEditor::on_actionCheck_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
    if(currentWindow != NULL) {
        GSTextEdit *textEdit = (GSTextEdit*)(currentWindow->widget());
        isValidScript(textEdit->toPlainText());
    }
}

void GSScriptEditor::writeMessage(QString msg)
{
    if(!ui->dockConsole->isVisible())
        ui->dockConsole->setVisible(true);
    ui->console->append(msg);
}

void GSScriptEditor::readFromCompiller()
{
    QString compillerOutput = processCompiller->readAllStandardOutput();
    writeMessage(compillerOutput);
}

void GSScriptEditor::readErrorCompiller()
{
    QString compillerError = processCompiller->readAllStandardError();
    writeMessage(QString("Ошибка в процессе компиляции:\r\n%1").arg(compillerError));
}

void GSScriptEditor::finishedCompiller(int exitCode)
{
    if(exitCode > 0) {
        writeMessage(QString("Ошибка процесс компиляции завершился с кодом %1").arg(exitCode));
    }
    else {
        writeMessage("Компиляция завершена");
    }
}

void GSScriptEditor::on_actionUndo_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
    if(currentWindow != NULL) {
        ((QPlainTextEdit *)(currentWindow->widget()))->undo();
    }
}

void GSScriptEditor::on_actionRedo_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
    if(currentWindow != NULL) {
        ((QPlainTextEdit *)(currentWindow->widget()))->redo();
    }
}

void GSScriptEditor::consoleVisibleChanged(bool checked)
{
    if(ui->dockConsole->parent() != NULL) {
        if(!((QMainWindow *)ui->dockConsole->parent())->isMinimized())
            ui->actionConsole->setChecked(checked);
    }
}

void GSScriptEditor::actionsVisibleChanged(bool checked)
{

    if(ui->dockActions->parent() != NULL) {
        if(!((QMainWindow *)ui->dockActions->parent())->isMinimized())
            ui->actionActions->setChecked(checked);
    }
}

void GSScriptEditor::clearItem(QTreeWidgetItem *item)
{
    while(item->childCount()) {
        delete item->child(0);
    }
}

void GSScriptEditor::on_treeActions_itemDoubleClicked(QTreeWidgetItem *item, int)
{
    if(item != NULL) {
        TypeItem type = (TypeItem) (item->data(0, Qt::UserRole).toInt());
        if(type == TI_Slot) {
            QString slotText = QString("%1.%2;").arg(item->parent()->text(0),item->text(0));

            QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
            if(currentWindow != NULL) {
                GSTextEdit *textEdit = (GSTextEdit*)(currentWindow->widget());
                textEdit->appendPlainText(slotText);
            }
        }
    }
}

void GSScriptEditor::on_actionSaveScript_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();
    if(currentWindow != NULL) {
        if(currentWindow->property("type") == TSW_Edit) {
            GSTextEdit *textEdit = (GSTextEdit*)(currentWindow->widget());

            if(isValidScript(textEdit->toPlainText())) {
                GSScript *script = (GSScript *)(currentWindow->property("scriptAddress").toUInt());

                saveScriptToFile(script->alias(), textEdit->toPlainText());
                script->setScript(textEdit->toPlainText());
            }
            else {
                return;
            }
        }
        else {
            on_actionSaveScriptAs_triggered();
        }
    }
}

void GSScriptEditor::on_actionSaveScriptAs_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();

    if(currentWindow != NULL) {
        GSTextEdit *textEdit = (GSTextEdit*)(currentWindow->widget());
        if(isValidScript(textEdit->toPlainText())){

            QString currentTabName = currentWindow->windowTitle();

            if(saveDialog->exec(currentTabName, _objects.keys()) != QDialog::Accepted)
                return;

            currentTabName = saveDialog->alias();
            currentWindow->setWindowTitle(currentTabName);

            delete saveDialog;

            saveScriptToFile(currentTabName, textEdit->toPlainText());

            GSScript *newScript = new GSScript(currentTabName);
            newScript->setScript(textEdit->toPlainText());

            currentWindow->setProperty("type", TSW_Edit);
            currentWindow->setProperty("scriptAddress", (uint32_t) newScript);

            emit haveNewObject(newScript);

            loadObjectTo(newScript, _scripts);
        }
    }
}

bool GSScriptEditor::isValidScript(QString script)
{
    QStringList scriptList = GSScript::getScriptList(script);

    bool haveError = false;
    for(int i = 0; i < scriptList.count(); i++) {
        QString line = scriptList[i];
        if(line.length() == 0)
            continue;

        QStringList slot = line.split(".");
        if(slot.count() != 2) {
            writeMessage(QString("Ошибка: в строке \"%1\". Синтаксическая ошибка.")
                         .arg(line));
            haveError = true;
            continue;
        }
        if(_objects.contains(slot[0])) {
            if(!_objects.value(slot[0])->slotsList().contains(slot[1])) {
                writeMessage(QString("Ошибка: в строке \"%1\". Объект \"%2\" не имеет слота \"%3\"")
                             .arg(line)
                             .arg(slot[0])
                        .arg(slot[1]));
                haveError = true;
            }
        }
        else {
            writeMessage(QString("Ошибка: в строке \"%1\". Объект \"%2\" не найден")
                         .arg(line)
                         .arg(slot[0]));
            haveError = true;
        }
    }
    if(haveError) {
        writeMessage(QString("Проверка закончилась с ошибками."));
        return false;
    }

    writeMessage(QString("Проверка закончилась удачно."));

    return true;
}

void GSScriptEditor::saveScriptToFile(QString scriptName, QString data)
{
    extern QString scriptsFolder;
    QString fullPath = scriptsFolder + scriptName +".gslc";

    QDir dir(scriptsFolder);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    QFile outFile(fullPath);
    if(outFile.open(QFile::WriteOnly)){
        outFile.write(data.toUtf8());
        writeMessage(QString("Файл записан в '%1'.").arg(fullPath));
    }
    else {
        writeMessage(QString("Ошибка записи файла '%1'.").arg(fullPath));
        return;
    }
}



void GSScriptEditor::on_actionRun_triggered()
{
    QMdiSubWindow *currentWindow = ui->mdiArea->activeSubWindow();

    if(currentWindow != NULL) {
        GSTextEdit *textEdit = (GSTextEdit*)(currentWindow->widget());
        if(isValidScript(textEdit->toPlainText())){

            QString currentTabName = currentWindow->windowTitle();

            GSScript *testingScript = new GSScript(currentTabName);
            testingScript->setScript(textEdit->toPlainText());

            connect(testingScript, SIGNAL(callingSlot(QString)), this, SLOT(callSlot(QString)));
            testingScript->run();

            //QTimer::singleShot(500,testingScript, SLOT(deleteLater()));

            delete testingScript;
        }
        else {
            writeMessage(QString("Не возвожно выполнить скрипт \"%1\".").arg(currentWindow->windowTitle()));
        }
    }
}

void GSScriptEditor::callSlot(QString slot)
{
    emit callingSlot(slot);
}

void GSScriptEditor::closeTab(QObject *tab)
{
    //QMdiSubWindow *closingTab =
}

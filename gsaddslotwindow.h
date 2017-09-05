#ifndef GSADDSLOTWINDOW_H
#define GSADDSLOTWINDOW_H

#include <QMainWindow>
#include <QSplitter>

#include <QProcess>

#include "gsobjects.h"
#include "gssavescriptdialog.h"

namespace Ui {
class GSAddSlotWindow;
}

class GSAddSlotWindow : public QMainWindow
{
    Q_OBJECT

    typedef enum TypeItem {
        TI_Object,
        TI_Slot,
        TI_Type
    } TypeItem;

    typedef enum TypeSubWindow {
        TSW_New,
        TSW_Edit
    } TypeSubWindow;

public:
    explicit GSAddSlotWindow(QWidget *parent = 0);

    void addNewSlot();

    void loadObjects(ObjectsHash objects);

    void loadObjectTo(GSObject *what, QTreeWidgetItem *item);

    ~GSAddSlotWindow();

    void editScript(GSScript *script);

public slots:
    void exec();

signals:
    void haveNewObject(GSObject *);
    void callingSlot(QString);

private slots:
    void on_actionNew_triggered();

    void on_actionCheck_triggered();

    void writeMessage(QString msg);

    void readFromCompiller();
    void readErrorCompiller();
    void finishedCompiller(int exitCode);

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void consoleVisibleChanged(bool checked);

    void actionsVisibleChanged(bool checked);

    void clearItem(QTreeWidgetItem *item);

    void on_treeActions_itemDoubleClicked(QTreeWidgetItem *item, int);

    void on_actionSaveScriptAs_triggered();

    void on_actionSaveScript_triggered();

    void saveScriptToFile(QString scriptName, QString data);

    void on_actionRun_triggered();

    void callSlot(QString slot);

    void closeTab(QObject *tab);


private:
    bool isValidScript(QString script);

    Ui::GSAddSlotWindow *ui;
    QSplitter splitter;

    QProcess *processCompiller;

    QTreeWidgetItem *_timers;
    QTreeWidgetItem *_modules;
    QTreeWidgetItem *_sheldules;
    QTreeWidgetItem *_processes;
    QTreeWidgetItem *_scripts;

    ObjectsHash _objects;

    GSSaveScriptDialog *saveDialog;
};

#endif // GSADDSLOTWINDOW_H

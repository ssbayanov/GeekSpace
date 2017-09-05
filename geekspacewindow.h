#ifndef GEEKSPACEWINDOW_H
#define GEEKSPACEWINDOW_H

#include <QMainWindow>
#include <QSettings>

#include "gsobjects.h"
#include "gsaddslotwindow.h"
#include "gsaddmoduledialog.h"
#include "gsaddtimerdialog.h"
#include "gsaddshelduledialog.h"
#include "gsaddprocessdialog.h"

namespace Ui {
class GeekSpaceWindow;
}

typedef enum TypeItem {
    TI_Object,
    TI_Slot,
    TI_Signal,
    TI_ConnectionSignal,
    TI_ConnectionSlot,
    TI_Other
} TypeItem;

typedef enum TypeMessage {
    TM_Info,
    TM_Dedug,
    TM_Warning,
    TM_Error
} TypeMessage;

class GeekSpaceWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GeekSpaceWindow(QWidget *parent = 0);

    ~GeekSpaceWindow();

private slots:
    void addObject(GSObject *object);

    void writeMessage(QString msg, TypeMessage type);
    void writeMessage(QString msg);
    void writeError(QString msg);

    void on_actionFindModule_triggered();

//    void on_actionAddSlot_triggered();

    void hiddenConsole(bool checked);

    void appendModules(ModulesList *newModules);

    void addModuleItem(GSModule *module);
    void addTimerItem(GSTimer *timer);
    void addShelduleItem(GSSheldule *sheldule);
    void addProcessItem(GSProcess *process);
    void addScriptItem(GSScript *script);

    void updateConnections();

    void updateObjectItem(GSObject *object);

    void on_treeModules_itemDoubleClicked(QTreeWidgetItem *item, int column);
    void doubleClickedBySlot(QTreeWidgetItem *item);
    void doubleClickedBySignal(QTreeWidgetItem *item);
    void doubleClickedByObject(QTreeWidgetItem *item);

    void signalsProcessor(QString signal);

    void callSlotByName(QString slot);

    void on_treeModules_customContextMenuRequested(const QPoint &pos);

    void on_actionAddSignalTimer_triggered();

    void on_actionAddSignalSheldule_triggered();

    void on_actionAddProcess_triggered();

    void on_actionScript_triggered();

private:
    void loadSettings();

    void writeSettings();
    void writeSettingsModule(GSModule *module);
    void writeSettingsTimer(GSTimer *timer);
    void writeSettingsSheldule(GSSheldule *sheldule);
    void writeSettingsProcess(GSProcess *process);
    void writeSettingsScript(GSScript *script);

    Ui::GeekSpaceWindow *ui;

    GSAddSlotWindow *addScriptWindow;
    GSAddModuleDialog *addModuleDialog;
    GSAddSignalTimerDialog *timerDialog;
    GSAddShelduleDialog *shelduleDialog;
    GSAddProcessDialog *processDialog;
    QSettings *settings;

    //QHash <QString, GSModule *> modules;
    ObjectsHash objects;
    QHash <GSModule *, QTreeWidgetItem *> modulesAndItem;

    QHash<QString, QString> _connections;

    QTreeWidgetItem *modulesItem;
    QTreeWidgetItem *timersItem;
    QTreeWidgetItem *sheldulesItem;
    QTreeWidgetItem *scriptsItem;
    QTreeWidgetItem *processesItem;
    QTreeWidgetItem *connectsItem;
};

#endif // GEEKSPACEWINDOW_H

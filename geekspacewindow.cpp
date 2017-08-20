#include "geekspacewindow.h"
#include "ui_geekspacewindow.h"

#include "gsmodule.h"

#include <QDateTime>

GeekSpaceWindow::GeekSpaceWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GeekSpaceWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->verticalLayoutWidget);

    ui->dockConsole->setWidget(ui->console);
    connect(ui->dockConsole, SIGNAL(visibilityChanged(bool)), this, SLOT(on_actionConsole_triggered(bool)));
    connect(ui->dockConsole, SIGNAL())

    objects.clear();

    addSlotWindow = new GSAddSlotWindow(this);
    addSlotWindow->setHidden(true);

    addModuleDialog = new GSAddModuleDialog(this);
    addModuleDialog->setHidden(true);
    connect(addModuleDialog, SIGNAL(accepted(ModulesList*)), this, SLOT(appendModules(ModulesList*)));

    timerDialog = new GSAddSignalTimerDialog(this);
    connect(timerDialog, SIGNAL(accept(GSObject *)), this, SLOT(addObject(GSObject*)));

    shelduleDialog = new GSAddShelduleDialog(this);
    connect(shelduleDialog, SIGNAL(accept(GSObject *)), this, SLOT(addObject(GSObject*)));

    modulesItem = new QTreeWidgetItem(ui->treeModules, QStringList() << "Модули");
    slotsItem = new QTreeWidgetItem(ui->treeModules, QStringList() << "Пользовательские слоты");
    signalsItem = new QTreeWidgetItem(ui->treeModules, QStringList() << "Пользовательские сигналы");
    connectsItem = new QTreeWidgetItem(ui->treeModules, QStringList() << "Соединения");
    ui->treeModules->setContextMenuPolicy(Qt::CustomContextMenu);

    loadSettings();

    updateConnections();

    writeMessage("Стартуем.");
    writeMessage("Продключаемся к модулям.");
}

GeekSpaceWindow::~GeekSpaceWindow()
{
    writeSettings();

    delete ui;
}

void GeekSpaceWindow::writeMessage(QString msg, TypeMessage type)
{
    QString color,icontype;
    switch(type){
    case TM_Info:
        color = "#02549F";
        icontype="info";
        break;
    case TM_Error:
        color = "#B71116";
        icontype="error";
        break;
    case TM_Dedug:
        color = "#FF7338";
        icontype="debug";
        break;
    case TM_Warning:
        color = "#FCB712";
        icontype="warning";
        break;
    }
    QDateTime dt = QDateTime::currentDateTime();
    ui->console->insertHtml("<span style = \"color:"+color+"\"><img src=\":/"+icontype+".png\" /> ["+dt.time().toString()+"] "+msg.toStdString().c_str()+"</span><br>");
    ui->console->moveCursor(ui->console->textCursor().Down);
}

void GeekSpaceWindow::writeMessage(QString msg)
{
    writeMessage(msg, TM_Info);
}

void GeekSpaceWindow::writeError(QString msg)
{
    writeMessage(msg, TM_Error);
}

void GeekSpaceWindow::on_actionFindModule_triggered()
{
    addModuleDialog->show();

    //connect(d, SIGNAL(haveMessage(QString)), this, SLOT(writeMessage(QString)));
}

//void GeekSpaceWindow::on_actionAddSlot_triggered()
//{
//    addSlotWindow->show();
//    addSlotWindow->addNewSlot();
//}

void GeekSpaceWindow::on_actionConsole_triggered(bool checked)
{
    ui->dockConsole->setVisible(checked);
}

void GeekSpaceWindow::appendModules(ModulesList *newModules)
{
    for(int i = 0; i < newModules->count(); i++) {
        addObject((GSObject *)(newModules->at(i)));
    }
    writeMessage(QString("Добавлено модулей: %1").arg(newModules->count()));
}

void GeekSpaceWindow::updateConnections()
{
    while(connectsItem->childCount() != 0) {
        connectsItem->removeChild(connectsItem->child(0));
    }

    QStringList connectedSignals = _connections.uniqueKeys();
    for(int i = 0; i < connectedSignals.count(); i++) {
        QTreeWidgetItem *newSignalItem = new QTreeWidgetItem(connectsItem, QStringList() << connectedSignals[i]+"()");
        newSignalItem->setData(0, Qt::UserRole, TI_Other);

        QStringList connectedSlots = _connections.values(connectedSignals[i]);
        for(int j = 0; j < _connections.count(connectedSignals[i]); j++){
            QTreeWidgetItem *newSlotItem = new QTreeWidgetItem(newSignalItem,  QStringList() << connectedSlots[j]+"()");
            newSlotItem->setData(0, Qt::UserRole, TI_ConnectionSlot);
        }

    }
}

void GeekSpaceWindow::on_treeModules_itemDoubleClicked(QTreeWidgetItem *item, int)
{
    TypeItem type = (TypeItem) item->data(0, Qt::UserRole).toInt();

    if(type == TI_Slot) {
        QString slotName = item->text(0).left(item->text(0).length() - 2);
        GSObject *object = (GSObject *)(item->parent()->parent()->data(1, Qt::UserRole).toUInt());
        if(object != NULL)
            object->callSlot(slotName);
    }
}

void GeekSpaceWindow::signalsProcessor(QString signal)
{
    //    qDebug() << "Have signal: " << signal;
    if(_connections.contains(signal)){
        QStringList callingSlots = _connections.values(signal);
        for(int i = 0; i < callingSlots.count(); i++)
            callSlotByName(callingSlots[i]);
    }
}

void GeekSpaceWindow::callSlotByName(QString slot)
{
    QStringList data = slot.split(".");
    GSObject *object = objects.value(data[0]);
    if(object != NULL) {
        object->callSlot(data[1]);
    }
    else {
        writeError(QString("Обращение к неизвестному объекту \"%1\"").arg(data[0]));
    }
}

void GeekSpaceWindow::on_treeModules_customContextMenuRequested(const QPoint &pos)
{
    qDebug() << "Menu request";
    QModelIndex index =  ui->treeModules->indexAt(pos);
    QTreeWidgetItem *item = (QTreeWidgetItem *) index.internalPointer();

    if(item->data(0, Qt::UserRole).toInt() == TI_Signal) {
        QMenu *menu=new QMenu(this);
        QMenu *connectTo = menu->addMenu("Подключить к слоту...");

        ObjectsList objs = objects.values();
        for(int i = 0; i < objs.count(); i++) {
            QMenu *moduleMenu = connectTo->addMenu(objs[i]->alias());
            QStringList moduleSlots = objs[i]->slotsList();
            for(int j = 0; j < moduleSlots.count(); j++) {
                QAction *slotAction = moduleMenu->addAction(moduleSlots[j]);
                connect(slotAction, &QAction::triggered, [=] () {
                    GSObject *signalObject = (GSObject *) (item->parent()->parent()->data(1, Qt::UserRole).toUInt());
                    _connections.insertMulti(signalObject->alias()+"."+item->text(0).left(item->text(0).length() - 2),
                                             objs[i]->alias()+"."+moduleSlots[j] );
                    updateConnections();
                });
            }
        }
        menu->popup(ui->treeModules->viewport()->mapToGlobal(pos));

        return;
    }

    if(item->data(0, Qt::UserRole).toInt() == TI_ConnectionSlot) {
        QMenu *menu=new QMenu(this);
        QAction *deleteAction = menu->addAction("Удалить соединение");
        connect(deleteAction, &QAction::triggered, [=] () {
            QTreeWidgetItem *signalItem = item->parent();
            QString signalText = signalItem->text(0).left(signalItem->text(0).length() - 2);
            QString slotText = item->text(0).left(item->text(0).length() - 2);

            QHash<QString, QString>::iterator i = _connections.find(signalText);
            while (i != _connections.end() && i.key() == signalText) {
                if(i.value() == slotText)
                    _connections.erase(i);
                ++i;
            }
            updateConnections();
        });

        menu->popup(ui->treeModules->viewport()->mapToGlobal(pos));

        return;
    }

    if(item->data(0, Qt::UserRole).toInt() == TI_Object) {
        QMenu *menu=new QMenu(this);
        QAction *deleteAction = menu->addAction("Удалить объект");
        connect(deleteAction, &QAction::triggered, [=] () {
            GSObject *deletingObject = (GSObject *)(item->data(1,Qt::UserRole).toUInt());

            if(deletingObject != NULL) {
                objects.remove(deletingObject->alias());
                deletingObject->~GSObject();
                delete item;
            }

        });

        menu->popup(ui->treeModules->viewport()->mapToGlobal(pos));

        return;
    }
}

void GeekSpaceWindow::loadSettings()
{
    //Load Settings
    settings = new QSettings("settings.ini", QSettings::IniFormat);

    settings->beginGroup("MAIN_WINDOW");
    setGeometry(settings->value("geometry", geometry()).toRect());
    ui->actionConsole->setChecked(settings->value("console", true).toBool());
    ui->dockConsole->setFloating(settings->value("consoleFloating", false).toBool());
    ui->dockConsole->setGeometry(settings->value("consoleGeometry",ui->dockConsole->geometry()).toRect());
    settings->endGroup();

    settings->beginGroup("MODULES");
    QStringList modulesGroups = settings->childGroups();
    for(int i = 0; i < modulesGroups.count(); i++) {
        settings->beginGroup(modulesGroups[i]);
        QString peerAddress = settings->value("peerAddress", "NULL_ADDRESS").toString();
        uint peerPort = settings->value("peerPort", 0).toUInt();
        if(peerAddress != "NULL_ADDRESS" && peerPort != 0) { // if load module fail, skip it
            GSModule *module = new GSModule(peerAddress, peerPort);
            module->setAlias(modulesGroups[i]);
            module->connect();
            module->updateInfo();

            addObject((GSObject *) module);
        }
        else {
            writeError("Ошибка загрузки настроек модуля.");
        }
        settings->endGroup();
    }
    settings->endGroup();

    settings->beginGroup("TIMERS");
    QStringList timersGroups = settings->childGroups();
    for(int i = 0; i < timersGroups.count(); i++) {
        settings->beginGroup(timersGroups[i]);

        int time = settings->value("time", 0).toInt();

        if(time != 0) {
            GSTimer *timer = new GSTimer(timersGroups[i],
                                         time,
                                         settings->value("repeat", 0).toBool());

            if(settings->value("activated", 0).toBool())
                timer->start();

            addObject((GSObject *) timer);
        }

        settings->endGroup();
    }
    settings->endGroup();


    settings->beginGroup("SHELDULES");
    QStringList sheldulesGroups = settings->childGroups();
    for(int i = 0; i < sheldulesGroups.count(); i++) {
        settings->beginGroup(sheldulesGroups[i]);

        TypeSheldule type = (TypeSheldule) settings->value("type", -1).toInt();
        if(type != -1) {
            GSSheldule *sheldule = new GSSheldule(sheldulesGroups[i],
                                                  type);

            settings->beginGroup("times");
            QStringList shelduleTimes = settings->childKeys();
            for(int j = 0; j < shelduleTimes.count(); j++) {
                QDateTime newDate = settings->value(shelduleTimes[j], QDateTime()).toDateTime();
                sheldule->addDate(newDate);
            }
            settings->endGroup(); //times

            if(settings->value("running", 0).toBool())
                sheldule->start();

            addObject((GSObject *) sheldule);
        }
        settings->endGroup();
    }
    settings->endGroup();

    settings->beginGroup("CONNECTIONS");
    QStringList signalsList = settings->childGroups();
    for(int i = 0; i < signalsList.count(); i++) {
        settings->beginGroup(signalsList[i]);

        QStringList slotsList = settings->childKeys();
        for(int j = 0; j < slotsList.count(); j++) {
            _connections.insertMulti(signalsList[i], settings->value(slotsList[j], "NULL").toString());
        }
        settings->endGroup();
    }
    settings->endGroup();

}

void GeekSpaceWindow::writeSettings()
{
    settings->beginGroup("MAIN_WINDOW");
    settings->setValue("geometry", geometry());
    settings->setValue("console", ui->actionConsole->isChecked());
    settings->setValue("consoleFloating", ui->dockConsole->isFloating());
    settings->setValue("consoleGeometry",ui->dockConsole->geometry());
    settings->endGroup();


    settings->remove("CONNECTIONS");
    settings->remove("MODULES");
    settings->remove("TIMERS");
    settings->remove("SHELDULES");

    ObjectsList objs = objects.values();
    for(int i = 0; i < objs.count(); i++) {
        GSObject *object = objs[i];
        switch (object->type()) {
        case TO_Module:
            writeSettingsModule((GSModule *) object);
            break;
        case TO_Timer:
            writeSettingsTimer((GSTimer *) object);
            break;
        case TO_Sheldule:
            writeSettingsSheldule((GSSheldule *) object);
            break;
        default:
            //writeError("Неизвестный тип об");
            break;
        }
    }

    settings->beginGroup("CONNECTIONS");
    QStringList childs = settings->childGroups();
    for(int i = 0; i < childs.count(); i++)
        settings->remove(childs[i]);

    QStringList connectedSignals = _connections.uniqueKeys();
    for(int i = 0; i < connectedSignals.count(); i++) {
        settings->beginGroup(connectedSignals[i]);
        for(int j = 0; j < _connections.count(connectedSignals[i]); j++){
            settings->setValue(QString("slot%1").arg(j), _connections.values(connectedSignals[i])[j]);
        }
        settings->endGroup();

    }
    settings->endGroup();

    settings->sync();
}

void GeekSpaceWindow::writeSettingsModule(GSModule *module)
{
    settings->beginGroup("MODULES");

    settings->beginGroup(module->alias());
    settings->setValue("peerAddress", module->address());
    settings->setValue("peerPort", module->port());
    settings->setValue("displayName", module->displayName());
    settings->endGroup();

    settings->endGroup();
}

void GeekSpaceWindow::writeSettingsTimer(GSTimer *timer)
{
    settings->beginGroup("TIMERS");

    settings->beginGroup(timer->alias());
    settings->setValue("time", timer->time());
    settings->setValue("repeat", timer->repeat());
    settings->setValue("activated", timer->isActive());
    settings->endGroup();

    settings->endGroup();
}

void GeekSpaceWindow::writeSettingsSheldule(GSSheldule *sheldule)
{
    settings->beginGroup("SHELDULES");

    settings->beginGroup(sheldule->alias());

    settings->beginGroup("times");
    DateTimeList dates = sheldule->dateTimes();
    for(int i = 0; i < dates.count(); i++) {
        settings->setValue(QString("time%1").arg(i), dates[i]);
    }
    settings->endGroup();
    settings->setValue("type", sheldule->typeSheldule());
    settings->setValue("running", sheldule->isRunninig());
    settings->endGroup();

    settings->endGroup();
}

void GeekSpaceWindow::addObject(GSObject *object)
{
    if(objects.contains(object->alias())) {
        writeError(QString("Ошибка добавления объекта типа %1. Объект %2 с именем %3 уже существует.")
                   .arg(typeNames[object->type()])
                .arg(typeNames[objects.value(object->alias())->type()])
                .arg(object->alias()));
        return;
    }

    objects.insert(object->alias(), object);

    connect(object, SIGNAL(haveSignal(QString)),
            this, SLOT(signalsProcessor(QString)));

    connect(object, SIGNAL(haveError(QString)),
            this, SLOT(writeError(QString)));
    connect(object, SIGNAL(haveMessage(QString)),
            this, SLOT(writeMessage(QString)));

    switch (object->type()) {
    case TO_Module:
        addModuleItem((GSModule *) object);
        break;
    case TO_Timer:
        addTimerItem((GSTimer *) object);
        break;
    case TO_Sheldule:
        addShelduleItem((GSSheldule *) object);
        break;
    default:
        //writeError("Неизвестный тип об");
        break;
    }
}

void GeekSpaceWindow::addModuleItem(GSModule *module)
{
    QString moduleText = module->alias()
            + QString("(%1:%2)").arg(module->address()).arg(module->port());

    QTreeWidgetItem *newModuleItem = new QTreeWidgetItem(modulesItem, QStringList() << moduleText);
    newModuleItem->setData(0, Qt::UserRole, TI_Object);
    newModuleItem->setData(1, Qt::UserRole, (uint) module);

    module->setItem(newModuleItem);

    if(module->isOnline())
        newModuleItem->setIcon(0, QIcon(":/connected"));
    else
        newModuleItem->setIcon(0, QIcon(":/disconnected"));

    connect(module, &GSModule::connected, [=] () {
        newModuleItem->setIcon(0,QIcon(":/connected"));
        newModuleItem->setDisabled(false);
    });
    connect(module, &GSModule::disconnected, [=] () {
        newModuleItem->setIcon(0,QIcon(":/disconnected"));
        newModuleItem->setDisabled(true);
    });

    connect(module, SIGNAL(infoUpdated(GSObject*)), this, SLOT(updateObjectItem(GSObject*)));
    updateObjectItem(module);
}

void GeekSpaceWindow::addTimerItem(GSTimer *timer)
{
    QTreeWidgetItem *newTimerItem = new QTreeWidgetItem(signalsItem, QStringList() << timer->alias());
    newTimerItem->setData(0, Qt::UserRole, TI_Object);
    newTimerItem->setData(1, Qt::UserRole, (uint) timer);
    timer->setItem(newTimerItem);

    if(timer->isActive())
        newTimerItem->setIcon(0, QIcon(":/run"));
    else
        newTimerItem->setIcon(0, QIcon(":/pause"));

    connect(timer, &GSTimer::started, [=] () {
        newTimerItem->setIcon(0,QIcon(":/run"));
    });
    connect(timer, &GSTimer::stopped, [=] () {
        newTimerItem->setIcon(0,QIcon(":/pause"));
    });

    updateObjectItem(timer);
}

void GeekSpaceWindow::addShelduleItem(GSSheldule *sheldule)
{
    QTreeWidgetItem *newShelduleItem = new QTreeWidgetItem(signalsItem, QStringList() << sheldule->alias());
    newShelduleItem->setData(0, Qt::UserRole, TI_Object);
    newShelduleItem->setData(1, Qt::UserRole, (uint) sheldule);
    sheldule->setItem(newShelduleItem);

    if(sheldule->isRunninig())
        newShelduleItem->setIcon(0, QIcon(":/run"));
    else
        newShelduleItem->setIcon(0, QIcon(":/pause"));

    connect(sheldule, &GSSheldule::started, [=] () {
        newShelduleItem->setIcon(0,QIcon(":/run"));
    });
    connect(sheldule, &GSSheldule::paused, [=] () {
        newShelduleItem->setIcon(0,QIcon(":/pause"));
    });

    updateObjectItem(sheldule);
}

void GeekSpaceWindow::updateObjectItem(GSObject *object)
{
    QTreeWidgetItem *updatingItem = object->item();

    if(updatingItem != NULL) {
        if(object->type() == TO_Module) {
            GSModule *module = (GSModule*) object;
            QString moduleText = module->alias()
                    + QString("(%1:%2)").arg(module->address()).arg(module->port());
            updatingItem->setText(0, moduleText);
        }

        if(object->slotsList().count() > 0) {
            QTreeWidgetItem *slotsItem = new QTreeWidgetItem(updatingItem, QStringList() << "Слоты");

            QStringList itemSlots = object->slotsList();
            for(int i = 0; i < itemSlots.count(); i++) {
                QTreeWidgetItem *functionItem = new QTreeWidgetItem(slotsItem, QStringList() << itemSlots[i] + "()");
                functionItem->setData(0, Qt::UserRole, TI_Slot);
            }
        }

        if(object->signalsList().count() > 0) {
            QTreeWidgetItem *signalsItem = new QTreeWidgetItem(updatingItem, QStringList() << "Сигналы");

            QStringList itemSignals = object->signalsList();
            for(int i = 0; i < itemSignals.count(); i++) {
                QTreeWidgetItem *signalItem = new QTreeWidgetItem(signalsItem, QStringList() << itemSignals[i] + "()");
                signalItem->setData(0, Qt::UserRole, TI_Signal);
            }
        }

    }
}


void GeekSpaceWindow::on_actionAddSignalTimer_triggered()
{
    timerDialog->exec();
    timerDialog->setReservedNames(objects.keys());
    //connect(dialog, SIGNAL(accepted()), dialog, SLOT(deleteLater()));
}

void GeekSpaceWindow::on_actionAddSignalSheldule_triggered()
{
    shelduleDialog->exec();
    shelduleDialog->setReservedNames(objects.keys());
}

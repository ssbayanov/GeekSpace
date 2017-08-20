#include "gsaddmoduledialog.h"
#include "ui_gsappendmoduledialog.h"

#include <QHostInfo>
#include <QThread>

GSAddModuleDialog::GSAddModuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSAppendModuleDialog)
{
    ui->setupUi(this);
    setLayout(ui->centralLayout);

    searcher = NULL;
}

GSAddModuleDialog::~GSAddModuleDialog()
{
    delete ui;
}

void GSAddModuleDialog::setStatus(QString status)
{
    ui->labelStatus->setText(status);
}

void GSAddModuleDialog::writeMessage(QString msg)
{
    emit haveMessage(msg);
}

void GSAddModuleDialog::addModule(GSModule *module)
{
    modules.append(module);

    QString moduleText = module->address() + QString(":%1").arg(module->port());
    QTreeWidgetItem *newModule = new QTreeWidgetItem(ui->treeModules, QStringList() << moduleText);
    newModule->setCheckState(0, Qt::Unchecked);
    newModule->setData(0, Qt::UserRole, (uint) module);

    QTreeWidgetItem *functionsItem = new QTreeWidgetItem(newModule, QStringList() << "Слоты");

    QStringList functions = module->slotsList();
    for(int i = 0; i < functions.count(); i++) {
        QTreeWidgetItem *functionItem = new QTreeWidgetItem(functionsItem, QStringList() << functions.at(i));
        //functionItem->setData(0, Qt::UserRole, (uint) functions.at(i));
    }
}

void GSAddModuleDialog::on_buttonSearch_clicked()
{
    if(searcher != NULL) {
        searcher->deleteLater();
    }
    searcher = new GSModuleSearcher(45544, 45545);

    connect(searcher, SIGNAL(hasResult(GSModule *)), this, SLOT(addModule(GSModule *)));
    connect(searcher, SIGNAL(updateSearchStatus(QString)), this, SLOT(setStatus(QString)));

    //    searcher->moveToThread(thread);
    //    connect(searcher, SIGNAL(hasResult(QString)), this, SLOT(addModule(QString)));
    //    connect(thread, SIGNAL(started()), searcher, SLOT(process()));
    //    connect(searcher, SIGNAL(finished()), thread, SLOT(quit()));
    //    connect(searcher, SIGNAL(finished()), searcher, SLOT(deleteLater()));
    //    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    //    thread->start();
}

void GSAddModuleDialog::on_treeModules_itemDoubleClicked(QTreeWidgetItem *item, int)
{
//    Slot *function = (Slot *)item->data(0, Qt::UserRole).toUInt();
//    qDebug() << "Clicked on function " << item->text(0) << ". Adress: " << item->data(Qt::UserRole, 0).toUInt();
//    if(function != NULL)
//        function->module->callSlotByCode(function->slotCode);
}

void GSAddModuleDialog::on_GSAppendModuleDialog_rejected()
{
    for(int i =0; i < ui->treeModules->topLevelItemCount(); i++) {
            GSModule *module = (GSModule *)ui->treeModules->topLevelItem(i)->data(0, Qt::UserRole).toUInt();
            module->disconnect();
            module->deleteLater();
    }
}

void GSAddModuleDialog::on_buttonCancel_clicked()
{
    reject();
}

void GSAddModuleDialog::on_buttonAdd_clicked()
{
    ModulesList *checkedModules = new ModulesList;

    for(int i =0; i < ui->treeModules->topLevelItemCount(); i++) {
        GSModule *module = (GSModule *)ui->treeModules->topLevelItem(i)->data(0, Qt::UserRole).toUInt();
        if(ui->treeModules->topLevelItem(i)->checkState(0) == Qt::Checked) {
            checkedModules->append(module);
        }
        else {
            module->disconnect();
            module->deleteLater();
        }
    }

    emit accepted(checkedModules);
    accept();
}

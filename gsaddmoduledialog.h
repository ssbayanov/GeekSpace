#ifndef GSAPPENDMODULEDIALOG_H
#define GSAPPENDMODULEDIALOG_H

#include <QDialog>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include "gsmodule.h"
#include "gsmodulesearcher.h"

namespace Ui {
class GSAppendModuleDialog;
}

class GSAddModuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GSAddModuleDialog(QWidget *parent = 0);
    ~GSAddModuleDialog();

public slots:
    void setStatus(QString status);

private slots:
    void writeMessage(QString msg);
    void addModule(GSModule *module);

    void on_buttonSearch_clicked();

    void on_treeModules_itemDoubleClicked(QTreeWidgetItem *item, int);

    void on_GSAppendModuleDialog_rejected();

    void on_buttonCancel_clicked();

    void on_buttonAdd_clicked();

signals:
    void haveMessage(QString);

    void hasModules();

    void accepted(ModulesList *);

private:
    Ui::GSAppendModuleDialog *ui;

    GSModuleSearcher* searcher;

    ModulesList modules;
};

#endif // GSAPPENDMODULEDIALOG_H

#include "gsaddprocessdialog.h"
#include "ui_gsaddprocessdialog.h"

#include <QFileDialog>

#include "gsprocess.h"

GSAddProcessDialog::GSAddProcessDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSAddProcessDialog)
{
    ui->setupUi(this);

    setLayout(ui->centralLayout);
    //ui->paramsList->setEditTriggers(

    isEdit = false;
}

GSAddProcessDialog::~GSAddProcessDialog()
{
    delete ui;
}

void GSAddProcessDialog::setReservedNames(QStringList reservedNames)
{
    _reservedNames = reservedNames;
}

void GSAddProcessDialog::editProcess(GSProcess *process)
{
    _process = process;

    isEdit = true;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Применить");
    setWindowTitle("Изменить процесс");
    _reservedNames.removeOne(_process->alias());

    ui->aliasEdit->setText(_process->alias());

    ui->paramsList->clear();
    ui->paramsList->addItems(process->parametrs());
    ui->paramsList->addItem("");
    for(int i = 0; i < ui->paramsList->count(); i++) {
        ui->paramsList->item(i)->setFlags(ui->paramsList->item(i)->flags() | Qt::ItemIsEditable);
    }


    ui->processEdit->setText(process->processString());

    show();
}

int GSAddProcessDialog::exec()
{
    isEdit = false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Ok");
    ui->aliasEdit->setText("newProcess");
    setWindowTitle("Добавить процесс");
    ui->paramsList->clear();
    on_paramsList_currentRowChanged(0);
    ui->processEdit->clear();

    return QDialog::exec();
}

void GSAddProcessDialog::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Выбор файла процесса"), "", tr("Исполняеые файлы (*.exe)"));

    ui->processEdit->setText(fileName);
}

void GSAddProcessDialog::on_aliasEdit_textEdited(const QString &arg1)
{
    if(_reservedNames.contains(arg1) || arg1.contains(" ")) {
        ui->aliasEdit->setStyleSheet("color: red");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
        ui->aliasEdit->setStyleSheet("color: black");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void GSAddProcessDialog::on_buttonBox_accepted()
{
    QStringList paramsList;
    for(int i = 0; i < ui->paramsList->count(); i++) {
        if(ui->paramsList->item(i)->text().length() > 0)
            paramsList << ui->paramsList->item(i)->text();
    }

    if(isEdit) {
        _process->setAlias(ui->aliasEdit->text());
        _process->setParametrs(paramsList);
        _process->setProcessString(ui->processEdit->text());
    }
    else {

        GSProcess *newProcess = new GSProcess(ui->aliasEdit->text(),
                                              ui->processEdit->text(),
                                              paramsList);

        emit accept(newProcess);
    }
}

void GSAddProcessDialog::on_paramsList_currentRowChanged(int currentRow)
{
    if(currentRow-1 <= ui->paramsList->count()){
        QListWidgetItem *newItem =  new QListWidgetItem("", ui->paramsList);
        newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
    }
}

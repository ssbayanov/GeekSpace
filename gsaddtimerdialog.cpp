#include "gsaddtimerdialog.h"
#include "ui_gsaddtimerdialog.h"

#include <QPushButton>

GSAddSignalTimerDialog::GSAddSignalTimerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSAddSignalTimerDialog)
{
    ui->setupUi(this);    
}

GSAddSignalTimerDialog::~GSAddSignalTimerDialog()
{
    delete ui;
}

void GSAddSignalTimerDialog::setReservedNames(QStringList reservedNames)
{
    _reservedNames = reservedNames;
}

void GSAddSignalTimerDialog::on_lineAlias_textChanged(const QString &arg1)
{
    if(_reservedNames.contains(arg1)) {
        ui->lineAlias->setStyleSheet("color: red");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
        ui->lineAlias->setStyleSheet("color: black");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void GSAddSignalTimerDialog::on_buttonBox_accepted()
{
    GSTimer *timer = new GSTimer(ui->lineAlias->text(),
                                 ui->spinBoxTime->value()*1000,
                                 ui->checkBoxRepeat->isChecked());

    if(ui->checkBoxRun->isChecked())
        timer->start();

    emit accept(timer);
}

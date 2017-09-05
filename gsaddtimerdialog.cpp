#include "gsaddtimerdialog.h"
#include "ui_gsaddtimerdialog.h"

#include <QPushButton>

GSAddSignalTimerDialog::GSAddSignalTimerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSAddSignalTimerDialog)
{
    ui->setupUi(this);
    setLayout(ui->centralLayout);

    isEdit = false;
}

GSAddSignalTimerDialog::~GSAddSignalTimerDialog()
{
    delete ui;
}

void GSAddSignalTimerDialog::setReservedNames(QStringList reservedNames)
{
    _reservedNames = reservedNames;
}

void GSAddSignalTimerDialog::editTimer(GSTimer *timer)
{
    _timer = timer;

    isEdit = true;
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Применить");
    setWindowTitle("Изменить таймер");

    ui->aliasEdit->setText(timer->alias());
    _reservedNames.removeOne(timer->alias());

    ui->spinBoxTime->setValue(timer->time() / 1000);
    ui->checkBoxRun->setChecked(timer->isActive());

    if(timer->count() == 0) {
        ui->countSpin->setValue(1);
        ui->unlimitedCheckBox->setChecked(true);
    }
    else {
        ui->countSpin->setValue(timer->count());
        ui->unlimitedCheckBox->setChecked(false);
    }
    on_unlimitedCheckBox_clicked(ui->unlimitedCheckBox->isChecked());

    show();
}

int GSAddSignalTimerDialog::exec()
{
    isEdit = false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Ok");
    ui->aliasEdit->setText("newTimer");
    setWindowTitle("Добавить таймер");
    ui->spinBoxTime->setValue(1.0);
    ui->countSpin->setValue(1);
    ui->checkBoxRun->setChecked(true);
    ui->unlimitedCheckBox->setChecked(true);
    on_unlimitedCheckBox_clicked(true);

    return QDialog::exec();
}

void GSAddSignalTimerDialog::on_aliasEdit_textEdited(const QString &arg1)
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

void GSAddSignalTimerDialog::on_buttonBox_accepted()
{
    if(isEdit){
        if(ui->unlimitedCheckBox->isChecked())
            _timer->setCount(0);
        else
            _timer->setCount(ui->countSpin->value());
        _timer->setAlias(ui->aliasEdit->text());
        _timer->setTime(ui->spinBoxTime->value() * 1000);

        if(ui->checkBoxRun->isChecked())
            _timer->restart();
        else
            _timer->stop();
    }
    else {
        uint32_t count = 0;
        if(!ui->unlimitedCheckBox->isChecked()) {
            count = ui->countSpin->value();
        }
        GSTimer *timer = new GSTimer(ui->aliasEdit->text(),
                                     ui->spinBoxTime->value() * 1000,
                                     count);

        if(ui->checkBoxRun->isChecked())
            timer->start();

        emit accept(timer);
    }
}

void GSAddSignalTimerDialog::on_unlimitedCheckBox_clicked(bool checked)
{
    ui->countSpin->setEnabled(!checked);
    ui->countLabel->setEnabled(!checked);
}

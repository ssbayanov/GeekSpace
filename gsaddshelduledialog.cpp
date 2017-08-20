#include "gsaddshelduledialog.h"
#include "ui_gsaddshelduledialog.h"

#include <QPushButton>

GSAddShelduleDialog::GSAddShelduleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSAddSheldule)
{
    ui->setupUi(this);

    setLayout(ui->centralLayout);

    ui->tabOnce->setLayout(ui->onceLayout);
    ui->tabDaily->setLayout(ui->dailyLayout);
    ui->tabWeekly->setLayout(ui->weeklyLayout);

    ui->dateTimeOnce->setDate(QDate::currentDate());

    //ui->weeklyTable->set


    QStringList headers = QStringList() << "Пн"
                                        << "Вт"
                                        << "Ср"
                                        << "Чт"
                                        << "Пт"
                                        << "Сб"
                                        << "Вс";

    ui->weeklyTable->verticalHeader()->setHidden(false);
    ui->weeklyTable->setVerticalHeaderLabels(headers);

    for(int i = 0; i < ui->weeklyTable->rowCount(); i++) {
        GSTimeWithCheckBox *timeEdit = new GSTimeWithCheckBox(i, 0);
        ui->weeklyTable->setCellWidget(i, 0, timeEdit);
        connect(timeEdit, SIGNAL(activated(int,int)), this, SLOT(cellActivated(int,int)));
    }

}

GSAddShelduleDialog::~GSAddShelduleDialog()
{
    delete ui;
}


void GSAddShelduleDialog::setReservedNames(QStringList reservedNames)
{
    _reservedNames = reservedNames;
}

void GSAddShelduleDialog::on_calendarOnce_clicked(const QDate &date)
{
    ui->dateTimeOnce->setDate(date);
}

void GSAddShelduleDialog::on_dateTimeOnce_dateTimeChanged(const QDateTime &dateTime)
{
    if(dateTime < QDateTime::currentDateTime()) {
        ui->dateTimeOnce->setStyleSheet("color: red");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
        ui->dateTimeOnce->setStyleSheet("color: black");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void GSAddShelduleDialog::cellActivated(int row, int column)
{
    if(column+1 == ui->weeklyTable->columnCount()){
        ui->weeklyTable->insertColumn(column+1);
        for(int i = 0; i < ui->weeklyTable->rowCount(); i++) {
            GSTimeWithCheckBox *timeEdit = new GSTimeWithCheckBox(i, column+1);
            ui->weeklyTable->setCellWidget(i, column+1, timeEdit);
            connect(timeEdit, SIGNAL(activated(int,int)), this, SLOT(cellActivated(int,int)));
        }
    }
}


void GSAddShelduleDialog::on_buttonBox_accepted()
{
    GSSheldule *newSheldule;
    switch(ui->tabWidget->currentIndex()) {
    case 0: //once
        newSheldule = new GSSheldule(ui->aliasEdit->text(),
                                     TS_Once);
        newSheldule->addDate(ui->dateTimeOnce->dateTime());
        break;
    case 1: //dayly
        //        newSheldule = new GSSheldule(ui->aliasEdit->text(),
        //                                     TS_Once);
        //        for(int i = 0; i < ui->)
        break;
    case 2: //weekly
        newSheldule = new GSSheldule(ui->aliasEdit->text(),
                                     TS_Weekly);
        for(int row = 0; row < ui->weeklyTable->rowCount(); row++) {
            for(int col =0; col < ui->weeklyTable->columnCount(); col++) {
                GSTimeWithCheckBox *timeWidget = (GSTimeWithCheckBox *)(ui->weeklyTable->cellWidget(row, col));
                if(timeWidget->isChecked()) {
                    quint8 dayOfWeek = QDateTime::currentDateTime().date().dayOfWeek() - 1;
                    QDateTime newDate = QDateTime::currentDateTime().addDays(row - dayOfWeek);
                    newDate.setTime(QTime());
                    newDate.setTime(timeWidget->time());

                    newSheldule->addDate(newDate);
                }
            }
        }
        break;
    case 3: //monthly
        break;
    case 4: //yearly
        break;
    default:
        break;
    }

    emit accept(newSheldule);
}

void GSAddShelduleDialog::on_aliasEdit_textChanged(const QString &arg1)
{
    if(_reservedNames.contains(arg1)) {
        ui->aliasEdit->setStyleSheet("color: red");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
        ui->aliasEdit->setStyleSheet("color: black");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

GSTimeWithCheckBox::GSTimeWithCheckBox(int row, int column, QWidget *parent)
{
    _check = new QCheckBox(this);
    _time = new QTimeEdit(this);
    _time->setEnabled(false);

    _layout = new QHBoxLayout(this);
    _layout->addWidget(_check);
    _layout->addWidget(_time);
    setLayout(_layout);

    _layout->setContentsMargins(5,0,5,0);

    setProperty("row", row);
    setProperty("column", column);

    connect(_check, SIGNAL(clicked(bool)), _time, SLOT(setEnabled(bool)));
    connect(_check, SIGNAL(clicked(bool)), this, SLOT(activate()));
    //connect(time, SIGNAL(dateChanged(QDate)), this, SLOT(activate()));
}

GSTimeWithCheckBox::~GSTimeWithCheckBox()
{
    delete _check;
    delete _time;
    delete _layout;
}

bool GSTimeWithCheckBox::isChecked()
{
    return _check->isChecked();
}

QTime GSTimeWithCheckBox::time()
{
    return _time->time();
}

void GSTimeWithCheckBox::activate()
{
    emit activated(property("row").toInt(), property("column").toInt());
}


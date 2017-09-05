#include "gsaddshelduledialog.h"
#include "ui_gsaddshelduledialog.h"

#include <QPushButton>
#include <QTime>

#include <QDebug>

GSAddShelduleDialog::GSAddShelduleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSAddSheldule)
{
    ui->setupUi(this);

    setLayout(ui->centralLayout);

    ui->tabOnce->setLayout(ui->onceLayout);
    ui->tabWeekly->setLayout(ui->weeklyLayout);

    connect(ui->calendarOnce, SIGNAL(selectionChanged()), this, SLOT(recalcNextDate()));
    connect(ui->dateTimeOnce, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(recalcNextDate()));
    connect(ui->repeatCheckBox, SIGNAL(toggled(bool)), this, SLOT(recalcNextDate()));
    connect(ui->repeatTimeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(recalcNextDate()));
    connect(ui->tabWidget, SIGNAL(currentChanged(int)), this, SLOT(recalcNextDate()));
}

GSAddShelduleDialog::~GSAddShelduleDialog()
{
    delete ui;
}


void GSAddShelduleDialog::setReservedNames(QStringList reservedNames)
{
    _reservedNames = reservedNames;
}

void GSAddShelduleDialog::editSheldule(GSSheldule *sheldule)
{
    isEdit = true;
    _sheldule = sheldule;

    initWeeklyTable();
    addNewColumnWeeklyTable();

    if (sheldule->typeSheldule() == TS_Once
            || sheldule->dateTimes().count() > 1) {
        ui->repeatCheckBox->setChecked(false);
    }
    else {
        ui->repeatCheckBox->setChecked(true);
    }

    on_repeatCheckBox_toggled(ui->repeatCheckBox->isChecked());

    switch (sheldule->typeSheldule()) {
    case TS_Once:
    case TS_Dayly:
        ui->repeatTimeComboBox->setCurrentIndex(0);
        break;
    case TS_Weekly:
        ui->repeatTimeComboBox->setCurrentIndex(0);

        if (sheldule->dateTimes().count() > 1) {

            DateTimeList dateTimes = sheldule->dateTimes();
            for(int i = 0; i < dateTimes.count(); i++) {
                int column = 0;
                int row = dateTimes[i].date().dayOfWeek()-1;
                GSTimeWithCheckBox *timeWidget = NULL;
                do {
                    timeWidget = (GSTimeWithCheckBox *)(ui->weeklyTable->cellWidget(row, column));

                    if(timeWidget == NULL)
                        break;

                    if(timeWidget->isChecked()){
                        column++;
                        if(ui->weeklyTable->columnCount() <= column)
                            addNewColumnWeeklyTable();
                    }

                }
                while(timeWidget->isChecked());

                if(timeWidget != NULL) {
                    timeWidget->setChecked(true);
                    timeWidget->setTime(dateTimes[i].time());
                }
                else {
                    break;
                    qDebug() << "Have error on load Sheldule weekly. No valid cell";
                }
            }
            ui->dateTimeOnce->setDateTime(QDateTime::currentDateTime().addSecs(3600));
            addNewColumnWeeklyTable();
            ui->tabWidget->setCurrentIndex(1);
        }
        else {
            ui->repeatTimeComboBox->setCurrentIndex(1);
        }
        break;
    case TS_Monthly:
        ui->repeatTimeComboBox->setCurrentIndex(2);
        break;
    case TS_Yearly:
        ui->repeatTimeComboBox->setCurrentIndex(3);
        break;
    default:
        break;
    }

    if(sheldule->dateTimes().count() == 1) {
        ui->dateTimeOnce->setDateTime(sheldule->dateTimes()[0]);
        ui->tabWidget->setCurrentIndex(0);
    }

    _reservedNames.removeOne(sheldule->alias());
    ui->aliasEdit->setText(sheldule->alias());

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("Применить");
    setWindowTitle("Изменить расписание");

    show();
}

int GSAddShelduleDialog::exec()
{
    isEdit = false;

    initWeeklyTable();
    addNewColumnWeeklyTable();
    ui->tabWidget->setCurrentIndex(0);

    QDateTime currentDateTime = QDateTime::currentDateTime().addSecs(3600);
    ui->dateTimeOnce->setDateTime(currentDateTime);

    ui->aliasEdit->setText("newSheldule");

    ui->repeatCheckBox->setChecked(false);
    on_repeatCheckBox_toggled(ui->repeatCheckBox->isChecked());

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("OK");
    setWindowTitle("Добавить расписание");

    return QDialog::exec();
}

void GSAddShelduleDialog::on_calendarOnce_clicked(const QDate &date)
{
    ui->dateTimeOnce->setDate(date);
}

void GSAddShelduleDialog::cellActivated(int, int column)
{
    if(column+1 == ui->weeklyTable->columnCount()){
        addNewColumnWeeklyTable();
    }
}


void GSAddShelduleDialog::on_buttonBox_accepted()
{
    DateTimeList weeklyDateTimes;
    TypeSheldule typeSheldule;



    switch(ui->tabWidget->currentIndex()) {
    case 0: //date
        if(ui->repeatCheckBox->isChecked()) {
            switch (ui->repeatTimeComboBox->currentIndex()) {
            case 0: //dayly
                typeSheldule = TS_Dayly;
                break;
            case 1: //weekly
                typeSheldule = TS_Weekly;
                break;
            case 2: //monthly
                typeSheldule = TS_Dayly;
                break;
            case 3: //yearly
                typeSheldule = TS_Yearly;
                break;
            default:
                break;
            }
        }
        else {
            typeSheldule = TS_Once;
        }

        weeklyDateTimes.append(ui->dateTimeOnce->dateTime());
        break;
    case 1: //weekly
        typeSheldule = TS_Weekly;

        for(int row = 0; row < ui->weeklyTable->rowCount(); row++) {
            for(int col =0; col < ui->weeklyTable->columnCount(); col++) {
                GSTimeWithCheckBox *timeWidget = (GSTimeWithCheckBox *)(ui->weeklyTable->cellWidget(row, col));
                if(timeWidget != NULL) {
                    if(timeWidget->isChecked()) {
                        quint8 dayOfWeek = QDateTime::currentDateTime().date().dayOfWeek() - 1;
                        QDateTime newDate = QDateTime::currentDateTime().addDays(row - dayOfWeek);
                        newDate.setTime(QTime());
                        newDate.setTime(timeWidget->time());

                        weeklyDateTimes.append(newDate);
                    }
                }
            }
        }
    default:
        break;
    }

    if(isEdit) {
        _sheldule->setAlias(ui->aliasEdit->text());
        _sheldule->setTypeSheldule(typeSheldule);
        _sheldule->setDates(weeklyDateTimes);
    }
    else {
        GSSheldule *newSheldule = new GSSheldule(ui->aliasEdit->text(), typeSheldule);
        newSheldule->setDates(weeklyDateTimes);
        emit accept(newSheldule);
    }
}

void GSAddShelduleDialog::on_aliasEdit_textEdited(const QString &arg1)
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

    connect(_time, &QTimeEdit::editingFinished, [=] () {
        emit timeEdit();
    });
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

void GSTimeWithCheckBox::setChecked(bool checked)
{
    _check->setChecked(checked);
    _time->setEnabled(checked);
}

QTime GSTimeWithCheckBox::time()
{
    return _time->time();
}

void GSTimeWithCheckBox::setTime(QTime time)
{
    _time->setTime(time);
}

void GSTimeWithCheckBox::activate()
{
    emit activated(property("row").toInt(), property("column").toInt());
}


void GSAddShelduleDialog::on_tabWidget_currentChanged(int index)
{
    recalcNextDate();
}

void GSAddShelduleDialog::recalcNextDate()
{
    QDateTime nextDateTime;
    nextDateTime.setMSecsSinceEpoch(0);

    switch(ui->tabWidget->currentIndex()){
    case 0:
        if(!ui->repeatCheckBox->isChecked()) {
            nextDateTime = ui->dateTimeOnce->dateTime();
        }
        else {
            switch (ui->repeatTimeComboBox->currentIndex()) {
            case 0:
                nextDateTime = GSSheldule::nextDate(ui->dateTimeOnce->dateTime(), TS_Dayly);
                break;
            case 1:
                nextDateTime = GSSheldule::nextDate(ui->dateTimeOnce->dateTime(), TS_Weekly);
                break;
            case 2:
                nextDateTime = GSSheldule::nextDate(ui->dateTimeOnce->dateTime(), TS_Monthly);
                break;
            case 3:
                nextDateTime = GSSheldule::nextDate(ui->dateTimeOnce->dateTime(), TS_Yearly);
                break;
            default:
                break;
            }
        }
        break;
    case 1:
        for(int cols = 0; cols < ui->weeklyTable->columnCount(); cols++) {
            for(int rows = 0; rows < ui->weeklyTable->rowCount(); rows++) {
                GSTimeWithCheckBox *timeWidget = (GSTimeWithCheckBox *)(ui->weeklyTable->cellWidget(rows, cols));
                if(timeWidget != NULL) {
                    if(timeWidget->isChecked()) {
                        quint8 dayOfWeek = QDateTime::currentDateTime().date().dayOfWeek() - 1;
                        QDateTime newDate = QDateTime::currentDateTime().addDays(rows - dayOfWeek);
                        newDate.setTime(QTime());
                        newDate.setTime(timeWidget->time());
                        newDate = GSSheldule::nextDate(newDate, TS_Weekly);

                        if(nextDateTime.toMSecsSinceEpoch() == 0) {
                            nextDateTime = newDate;
                        }
                        else {
                            if(nextDateTime > newDate) {
                                nextDateTime = newDate;
                            }
                        }
                    }
                }
            }
        }
        break;
    default:
        break;
    }

    if(nextDateTime < QDateTime::currentDateTime()){
        ui->nextDateTimelabel->setText("никогда");
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else {
        ui->nextDateTimelabel->setText(nextDateTime.toString("dddd d MMMM yyyy HH:mm"));
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void GSAddShelduleDialog::addNewColumnWeeklyTable()
{
    int column = ui->weeklyTable->columnCount();
    ui->weeklyTable->setColumnCount(column + 1);

    for(int i = 0; i < ui->weeklyTable->rowCount(); i++) {
        GSTimeWithCheckBox *timeEdit = new GSTimeWithCheckBox(i, column);
        ui->weeklyTable->setCellWidget(i, column, timeEdit);
        connect(timeEdit, SIGNAL(activated(int,int)), this, SLOT(cellActivated(int,int)));
        connect(timeEdit, SIGNAL(timeEdit()), this, SLOT(recalcNextDate()));
        connect(timeEdit, SIGNAL(activated(int,int)), this, SLOT(recalcNextDate()));
    }
}

void GSAddShelduleDialog::initWeeklyTable()
{
    ui->weeklyTable->clear();
    ui->weeklyTable->setColumnCount(0);

    QStringList headers = QStringList() << "Пн"
                                        << "Вт"
                                        << "Ср"
                                        << "Чт"
                                        << "Пт"
                                        << "Сб"
                                        << "Вс";

    ui->weeklyTable->verticalHeader()->setHidden(false);
    ui->weeklyTable->setVerticalHeaderLabels(headers);
}

void GSAddShelduleDialog::on_repeatCheckBox_toggled(bool checked)
{
    ui->repeatTimeComboBox->setEnabled(checked);
    ui->repeatLabel->setEnabled(checked);
}

void GSAddShelduleDialog::on_dateTimeOnce_dateTimeChanged(const QDateTime &dateTime)
{
    if(dateTime.date() != ui->calendarOnce->selectedDate()){
        ui->calendarOnce->setSelectedDate(dateTime.date());
    }
}

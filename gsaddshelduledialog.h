#ifndef GSADDSHELDULE_H
#define GSADDSHELDULE_H

#include <QDialog>

#include <QTableWidgetItem>

#include <QTimeEdit>
#include <QWidget>
#include <QCheckBox>
#include <QHBoxLayout>

#include "gssheldule.h"


class GSTimeWithCheckBox : public QWidget
{
    Q_OBJECT

public:
    GSTimeWithCheckBox(int row, int column, QWidget *parent = 0);
    ~GSTimeWithCheckBox();

    bool isChecked();
    void setChecked(bool checked);

    QTime time();
    void setTime(QTime time);

private:
    QCheckBox *_check;
    QTimeEdit *_time;
    QHBoxLayout *_layout;

private slots:
    void activate();

signals:
    void activated(int, int);
    void timeEdit();
};

namespace Ui {
class GSAddSheldule;
}

class GSAddShelduleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GSAddShelduleDialog(QWidget *parent = 0);
    ~GSAddShelduleDialog();

    void setReservedNames(QStringList reservedNames);

    void editSheldule(GSSheldule *sheldule);

public slots:
    int exec();

signals:
    void accept(GSObject *);

private slots:

    void on_calendarOnce_clicked(const QDate &date);

    void cellActivated(int, int column);

    void on_buttonBox_accepted();

    void on_aliasEdit_textEdited(const QString &arg1);

    void on_tabWidget_currentChanged(int index);

    void on_repeatCheckBox_toggled(bool checked);

    void recalcNextDate();

    void on_dateTimeOnce_dateTimeChanged(const QDateTime &dateTime);

private:
    Ui::GSAddSheldule *ui;

    QStringList _reservedNames;

    void addNewColumnWeeklyTable();

    void initWeeklyTable();

    bool isEdit;

    GSSheldule *_sheldule;
};

#endif // GSADDSHELDULE_H

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
    QTime time();

private:
    QCheckBox *_check;
    QTimeEdit *_time;
    QHBoxLayout *_layout;

private slots:
    void activate();

signals:
    void activated(int, int);
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

signals:
    void accept(GSObject *);

private slots:

    void on_calendarOnce_clicked(const QDate &date);

    void on_dateTimeOnce_dateTimeChanged(const QDateTime &dateTime);

    void cellActivated(int row, int column);

    void on_buttonBox_accepted();

    void on_aliasEdit_textChanged(const QString &arg1);

private:
    Ui::GSAddSheldule *ui;

    QStringList _reservedNames;
};

#endif // GSADDSHELDULE_H

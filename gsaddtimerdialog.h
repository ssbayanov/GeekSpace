#ifndef GSADDSIGNALTIMERDIALOG_H
#define GSADDSIGNALTIMERDIALOG_H

#include <QDialog>
#include "gstimer.h"

namespace Ui {
class GSAddSignalTimerDialog;
}

class GSAddSignalTimerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GSAddSignalTimerDialog(QWidget *parent = 0);
    ~GSAddSignalTimerDialog();

    void setReservedNames(QStringList reservedNames);

    void editTimer(GSTimer *timer);

public slots:
    int exec();

private slots:
    void on_aliasEdit_textEdited(const QString &arg1);

    void on_buttonBox_accepted();

    void on_unlimitedCheckBox_clicked(bool checked);

signals:
    void accept(GSObject *);

private:
    GSTimer *_timer;

    Ui::GSAddSignalTimerDialog *ui;
    QStringList _reservedNames;

    bool isEdit;
};

#endif // GSADDSIGNALTIMERDIALOG_H

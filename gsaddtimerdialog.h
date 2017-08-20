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

private slots:
    void on_lineAlias_textChanged(const QString &arg1);

    void on_buttonBox_accepted();

signals:
    void accept(GSObject *);

private:
    Ui::GSAddSignalTimerDialog *ui;
    QStringList _reservedNames;
};

#endif // GSADDSIGNALTIMERDIALOG_H

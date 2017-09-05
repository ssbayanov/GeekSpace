#ifndef GSADDPROCESSDIALOG_H
#define GSADDPROCESSDIALOG_H

#include <QDialog>

#include "gsprocess.h"

namespace Ui {
class GSAddProcessDialog;
}

class GSAddProcessDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GSAddProcessDialog(QWidget *parent = 0);
    ~GSAddProcessDialog();

    void setReservedNames(QStringList reservedNames);

    void editProcess(GSProcess *process);

public slots:
    int exec();

private slots:

    void on_pushButton_clicked();

    void on_aliasEdit_textEdited(const QString &arg1);

    void on_buttonBox_accepted();

    void on_paramsList_currentRowChanged(int currentRow);



signals:
    void accept(GSObject *);

private:
    bool isEdit;
    Ui::GSAddProcessDialog *ui;

    QStringList _reservedNames;

    GSProcess *_process;
};

#endif // GSADDPROCESSDIALOG_H

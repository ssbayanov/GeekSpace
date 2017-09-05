#ifndef GSSAVESCRIPTDIALOG_H
#define GSSAVESCRIPTDIALOG_H

#include <QDialog>

namespace Ui {
class GSSaveScriptDialog;
}

class GSSaveScriptDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GSSaveScriptDialog(QWidget *parent = 0);
    ~GSSaveScriptDialog();

    void setReservedNames(QStringList reservedNames);

    QString alias();

    int exec(QString alias, QStringList reservedNames);

private slots:
    void on_aliasEdit_textEdited(const QString &arg1);

private:
    Ui::GSSaveScriptDialog *ui;
    QStringList _reservedNames;
};

#endif // GSSAVESCRIPTDIALOG_H

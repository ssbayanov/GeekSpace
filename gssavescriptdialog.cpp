#include "gssavescriptdialog.h"
#include "ui_gssavescriptdialog.h"

#include <QPushButton>

GSSaveScriptDialog::GSSaveScriptDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GSSaveScriptDialog)
{
    ui->setupUi(this);
    setLayout(ui->centralLayout);
}

GSSaveScriptDialog::~GSSaveScriptDialog()
{
    delete ui;
}

void GSSaveScriptDialog::setReservedNames(QStringList reservedNames)
{
    _reservedNames = reservedNames;
}

QString GSSaveScriptDialog::alias()
{
    return ui->aliasEdit->text();
}

int GSSaveScriptDialog::exec(QString alias, QStringList reservedNames)
{
    ui->aliasEdit->setText(alias);
    _reservedNames = reservedNames;

    return QDialog::exec();
}

void GSSaveScriptDialog::on_aliasEdit_textEdited(const QString &arg1)
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

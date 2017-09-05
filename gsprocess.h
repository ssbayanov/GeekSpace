#ifndef GSPROCESS_H
#define GSPROCESS_H

#include "gsobject.h"

#include <QProcess>

class GSProcess : public GSObject
{
    Q_OBJECT
public:
    GSProcess(QString alias, QString process, QStringList params);
    ~GSProcess();

    bool isRunninig();

    QString processString();
    QStringList parametrs();

    void setParametrs(const QStringList &params);

    void setProcessString(const QString &processString);

public slots:
    void callSlot(QString slot);
    void start();
    void stop();

signals:
    void stopped();
    void started();

private slots:
    void processStateChanged(QProcess::ProcessState state);

private:
    QProcess *_process;
    QString _processString;
    QStringList _params;
};

#endif // GSPROCESS_H

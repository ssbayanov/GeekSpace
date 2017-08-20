#ifndef GSSIGNAL_H
#define GSSIGNAL_H

#include "gsobject.h"
#include <QTimer>

class GSTimer : public GSObject
{
    Q_OBJECT
public:
    explicit GSTimer(QString alias, int time, bool repeat);
    ~GSTimer();

    int time();
    bool repeat();
    bool isActive();

signals:
    void started();
    void stopped();

public slots:
    void start();
    void restart();
    void stop();

    void callSlot(QString slot);
private:
    QTimer *timer;

private slots:
    void timeOut();
};
#endif // GSSIGNAL_H

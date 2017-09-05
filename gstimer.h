#ifndef GSSIGNAL_H
#define GSSIGNAL_H

#include "gsobject.h"
#include <QTimer>

class GSTimer : public GSObject
{
    Q_OBJECT
public:
    explicit GSTimer(QString alias, int time, uint32_t count);
    ~GSTimer();

    int time();
    void setTime(int time);
    bool isActive();

    uint32_t count() const;
    void setCount(uint32_t value);

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
    uint32_t _count;

    uint32_t _counter;

private slots:
    void timeOut();
};
#endif // GSSIGNAL_H

#include "gstimer.h"

GSTimer::GSTimer(QString alias, int time, bool repeat)
    : GSObject(TO_Timer, alias)
{
    timer = new QTimer();
    timer->setInterval(time);
    timer->setSingleShot(!repeat);

    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    _signals.append("timeout");
    _slots << "start"
           << "stop"
           << "restart";
}

GSTimer::~GSTimer()
{
    timer->stop();
    delete timer;
}

int GSTimer::time()
{
    return timer->interval();
}

bool GSTimer::repeat()
{
    return !timer->isSingleShot();
}

bool GSTimer::isActive()
{
    return timer->isActive();
}

void GSTimer::callSlot(QString slot)
{
    if(_slots.contains(slot)) {
        QMetaObject::invokeMethod(this, slot.toStdString().c_str());
    }
}

void GSTimer::start()
{
    if(!timer->isActive())
        timer->start();

    emit started();
}

void GSTimer::restart()
{
    timer->stop();
    timer->start();

    emit started();
}

void GSTimer::stop()
{
    timer->stop();

    emit stopped();
}

void GSTimer::timeOut()
{
    emit haveSignal(QString("%1.%2").arg(_alias).arg(_signals[0]));
}

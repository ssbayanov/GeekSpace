#include "gstimer.h"

GSTimer::GSTimer(QString alias, int time, uint32_t count)
    : GSObject(TO_Timer, alias)
{
    timer = new QTimer();
    timer->setInterval(time);

    connect(timer, SIGNAL(timeout()), this, SLOT(timeOut()));

    _count = count;
    _counter = 0;
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

void GSTimer::setTime(int time)
{
    timer->setInterval(time);
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

uint32_t GSTimer::count() const
{
    return _count;
}

void GSTimer::setCount(uint32_t value)
{
    _count = value;
}

void GSTimer::start()
{
    if(!timer->isActive()) {
        timer->start();
        emit started();
    }
}

void GSTimer::restart()
{
    stop();
    start();
}

void GSTimer::stop()
{
    timer->stop();
    _counter = 0;
    emit stopped();
}

void GSTimer::timeOut()
{
    emit haveSignal(QString("%1.%2").arg(_alias).arg(_signals[0]));

    if(_count != 0) {
        _counter++;
        if(_count <= _counter)
            stop();
    }
}

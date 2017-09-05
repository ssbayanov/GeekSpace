#include "gssheldule.h"
#include <QDebug>

GSSheldule::GSSheldule(QString alias, TypeSheldule type)
    : GSObject(TO_Sheldule, alias)
{
    _signals << "timeCome";
    _slots << "start"
           << "pause";

    _running = false;

    _typeSheldule = type;


    shelduleTimer = new QTimer();
    connect(shelduleTimer, SIGNAL(timeout()), this, SLOT(timeCome()));

    //    QTimer *debugTimer = new QTimer();
    //    connect(debugTimer, &QTimer::timeout, [=] () {
    //        qDebug() << "Timer remain:" << shelduleTimer->remainingTime();
    //    });

    //    debugTimer->start(1000);
}

void GSSheldule::addDate(QDateTime date)
{
    //qDebug() << "GSSheldule.addDate Date: " << date.toString();
    if(!_dates.contains(date)) {
        _dates.append(date);
    }
    else {
        return;
    }

    date = nextDate(date);
    //qDebug() << "GSSheldule.addDate Updated date: " << date.toString();

    if(_updatedDates.count() == 0) {
        setNextDate(date);
    }
    else {
        if (_nextDate > date) {
            setNextDate(date);
        }
    }

    _updatedDates.append(date);
}

void GSSheldule::setDates(DateTimeList dates)
{
    _dates.clear();
    _updatedDates.clear();

    for(int i = 0; i < dates.count(); i++){
        addDate(dates[i]);
    }
}

bool GSSheldule::isRunninig()
{
    return _running;
}

DateTimeList GSSheldule::dateTimes()
{
    return _dates;
}

TypeSheldule GSSheldule::typeSheldule()
{
    return _typeSheldule;
}

void GSSheldule::callSlot(QString slot)
{
    if(_slots.contains(slot)) {
        QMetaObject::invokeMethod(this, slot.toStdString().c_str());
    }
}

void GSSheldule::start()
{
    _running = true;
    emit started();
}

void GSSheldule::pause()
{
    _running = false;
    emit paused();
}

void GSSheldule::timeCome()
{
    if(_nextDate <= QDateTime::currentDateTime()) {
        if(_running) {
            haveMessage(QString("GSSheldule.%1: Время пришло.").arg(_alias));
            emit haveSignal(_alias+"."+_signals[0]);
        }
        if(_typeSheldule != TS_Once) {
            findNextTime();
            haveMessage(QString("GSSheldule.%1: Следующее время срабатывания %2.").arg(_alias).arg(_nextDate.toString()));
        }
        else {
            pause();
        }
    }
    else {
        setTimer(_nextDate);
    }


}

void GSSheldule::setTypeSheldule(const TypeSheldule &typeSheldule)
{
    _typeSheldule = typeSheldule;
}

void GSSheldule::setTimer(QDateTime toTime)
{
    if((QDateTime::currentDateTime().msecsTo(toTime)) > maxInt) {
        shelduleTimer->start(maxInt);
    }
    else {
        shelduleTimer->start(QDateTime::currentDateTime().msecsTo(toTime));
    }

    qDebug() << "GSSheldule." << _alias << ".setTimer Date: " << _nextDate.toString() << " Remaining msecs: " << shelduleTimer->interval();
}

void GSSheldule::findNextTime()
{
    _updatedDates.removeOne(_nextDate);
    _nextDate = nextDate(_nextDate);
    _updatedDates.append(_nextDate);

    for(int i = 0; i < _updatedDates.count(); i++) {
        if (_nextDate > _updatedDates[i]) {
            _nextDate = _updatedDates[i];
        }
    }

    setNextDate(_nextDate);
}

void GSSheldule::setNextDate(QDateTime date)
{
    _nextDate = date;
    setTimer(_nextDate);
}

QDateTime GSSheldule::nextDate(QDateTime date)
{
    return nextDate(date, _typeSheldule);
}

QDateTime GSSheldule::nextDate(QDateTime date, TypeSheldule period)
{
    while(date <= QDateTime::currentDateTime()) {
        switch (period) {
        case TS_Dayly:
            date = date.addDays(1);
            break;
        case TS_Weekly:
            date = date.addDays(7);
            break;
        case TS_Monthly:
            date = date.addMonths(1);
            break;
        case TS_Yearly:
            date = date.addYears(1);
            break;
        default:
            break;
        }
    }

    return date;
}

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
}

void GSSheldule::addDate(QDateTime date)
{
    qDebug() << "GSSheldule.addDate Date: " << date.toString();
    while(date < QDateTime::currentDateTime()){
        date = nextDate(date);
    }

    qDebug() << "GSSheldule.addDate Updated date: " << date.toString();

    if(_dates.count() == 0) {
        setNextDate(date);
    }
    else {
        if (_nextDate > date) {
            setNextDate(date);
        }
    }

    _dates.append(date);
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
            emit haveSignal(_alias+"."+_signals[0]);
        }
        haveMessage(QString("GSSheldule.%1: Время пришло.").arg(_alias));

        if(_typeSheldule != TS_Once) {
            findNextTime();
            haveMessage(QString("GSSheldule.%1: Следующее время срабатывания %2.").arg(_alias).arg(_nextDate.toString()));
        }
    }

    if(_typeSheldule == TS_Once) {
        pause();
    }
}

void GSSheldule::setTimer(QDateTime toTime)
{
    if((QDateTime::currentDateTime().msecsTo(toTime)/10) > maxInt) {
        shelduleTimer->start(maxInt);
    }
    else {
        shelduleTimer->start(QDateTime::currentDateTime().msecsTo(toTime));
    }

    qDebug() << "GSSheldule.setTimer Date: " << _nextDate.toString() << " Remaining msecs: " << shelduleTimer->remainingTime();
}

void GSSheldule::findNextTime()
{
    _dates.removeOne(_nextDate);
    _nextDate = nextDate(_nextDate);
    _dates.append(_nextDate);

    for(int i = 0; i < _dates.count(); i++) {
        if (_nextDate > _dates[i]) {
            setNextDate(_dates[i]);
        }
    }
}

void GSSheldule::setNextDate(QDateTime date)
{
    _nextDate = date;
    setTimer(_nextDate);
}

QDateTime GSSheldule::nextDate(QDateTime date)
{
    switch (_typeSheldule) {
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

    return date;
}

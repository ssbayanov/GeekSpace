#ifndef GSSHELDULE_H
#define GSSHELDULE_H

#include "gsobject.h"

#include <QTimer>
#include <QDateTime>

#define maxInt 2147483648

typedef enum TypeSheldule {
    TS_Once,
    TS_Dayly,
    TS_Weekly,
    TS_Monthly,
    TS_Yearly
} TypeSheldule;

typedef QList <QDateTime> DateTimeList;

class GSSheldule : public GSObject
{
    Q_OBJECT
public:
    GSSheldule(QString alias, TypeSheldule type);

    void addDate(QDateTime date);
    void setDates(DateTimeList dates);

    bool isRunninig();

    DateTimeList dateTimes();

    TypeSheldule typeSheldule();
    void setTypeSheldule(const TypeSheldule &typeSheldule);
    static QDateTime nextDate(QDateTime date, TypeSheldule period);

public slots:
    void callSlot(QString slot);

    void start();
    void pause();

    void timeCome();

signals:
    void paused();
    void started();



private:

    TypeSheldule _typeSheldule;
    QDateTime _nextDate;

    DateTimeList _dates;
    DateTimeList _updatedDates;

    bool _running;

    QTimer *checkTimer;
    QTimer *shelduleTimer;

    void setTimer(QDateTime toTime);

    void findNextTime();

    void setNextDate(QDateTime date);

    QDateTime nextDate(QDateTime date);
};



#endif // GSSHELDULE_H

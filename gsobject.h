#ifndef GSOBJECT_H
#define GSOBJECT_H

#include <QObject>
#include <QTreeWidgetItem>

class GSObject;

//typedef struct {
//    GSObject *object;
//    QString slotName;
//    quint8 slotCode;
//} Slot;

//typedef struct {
//    GSObject *object;
//    QString signalName;
//    quint8 signalCode;
//} Signal;

typedef enum TypeObject {
    TO_Object,
    TO_Module,
    TO_Timer,
    TO_Sheldule
} TypeObject;

const QStringList typeNames = QStringList()
        << "Объект"
        << "Модуль"
        << "Таймер"
        << "Расписание";

class GSObject : public QObject
{
    Q_OBJECT
public:
    explicit GSObject(TypeObject type, QString alias, QTreeWidgetItem *item = 0, QObject *parent = 0);
    virtual ~GSObject();

    QStringList signalsList();
    QStringList slotsList();

    TypeObject type();
    QTreeWidgetItem *item();
    void setItem(QTreeWidgetItem *item);

    QString alias();

signals:
    void haveMessage(QString msg);
    void haveError(QString msg);
    void haveSignal(QString signal);

public slots:
    virtual void callSlot(QString slot);

private:
    TypeObject _type;

protected:
    QStringList _signals;
    QStringList _slots;

    QString _alias;

    QTreeWidgetItem *_item;
};

#endif // GSOBJECT_H

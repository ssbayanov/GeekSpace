#ifndef GSOBJECT_H
#define GSOBJECT_H

#include <QObject>
#include <QTreeWidgetItem>

class GSObject;

class GSObject : public QObject
{
    Q_OBJECT
public:
    typedef enum TypeObject {
        TO_Object,
        TO_Module,
        TO_Timer,
        TO_Sheldule,
        TO_Process,
        TO_Script
    } TypeObject;

    explicit GSObject(TypeObject type, QString alias, QTreeWidgetItem *item = 0, QObject *parent = 0);
    virtual ~GSObject();

    QStringList signalsList();
    QStringList slotsList();

    TypeObject type();
    QString typeText();
    QTreeWidgetItem *item();
    void setItem(QTreeWidgetItem *item);

    QString alias();

    void setAlias(const QString &alias);

signals:
    void haveMessage(QString msg);
    void haveError(QString msg);
    void haveSignal(QString signal);

public slots:
    virtual void callSlot(QString slot);

private:
    TypeObject _type;
    const QStringList _typeNames = QStringList()
            << "Объект"
            << "Модуль"
            << "Таймер"
            << "Расписание"
            << "Процесс"
            << "Сценарий";

protected:
    QStringList _signals;
    QStringList _slots;

    QString _alias;

    QTreeWidgetItem *_item;
};

#endif // GSOBJECT_H

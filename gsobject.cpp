#include "gsobject.h"

GSObject::GSObject(TypeObject type, QString alias, QTreeWidgetItem *item, QObject *parent) : QObject(parent)
{
    _alias = alias;
    _type = type;
    _item = item;

    _slots.clear();
    _signals.clear();
}

GSObject::~GSObject()
{

}

QStringList GSObject::signalsList()
{
    return _signals;
}

QStringList GSObject::slotsList()
{
    return _slots;
}

TypeObject GSObject::type()
{
    return _type;
}

QTreeWidgetItem *GSObject::item()
{
    return _item;
}

void GSObject::setItem(QTreeWidgetItem *item)
{
    _item = item;
}

QString GSObject::alias()
{
    return _alias;
}

void GSObject::callSlot(QString slot)
{
    emit haveError("Неправильный вызов слота");
}

void GSObject::setAlias(const QString &alias)
{
    _alias = alias;
}

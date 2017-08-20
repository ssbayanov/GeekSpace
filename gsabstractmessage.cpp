#include "gsabstractmessage.h"

GSAbstractMessage::GSAbstractMessage()
{

}

void GSAbstractMessage::setType(TypeOfMessage type)
{
    _type = type;
}

TypeOfMessage GSAbstractMessage::type()
{
    return _type;
}

QByteArray GSAbstractMessage::getSendData()
{
    QByteArray data;
    data.append((quint8) GSM_Ping);

    return data;
}


GSPingMessage::GSPingMessage()
{
    setType(GSM_Ping);
}

QByteArray GSPingMessage::getSendData()
{
    QByteArray data;
    data.append((quint8) GSM_Ping);

    return data;
}

QByteArray GSPingMessage::getPingData()
{
    QByteArray data;
    data.append((quint8) GSM_Ping);

    return data;
}

QDataStream &operator >>(QDataStream &in, GSAbstractMessage &msg)
{
    quint8 readType;
    in >> readType;

    msg.setType((TypeOfMessage) readType);

    return in;
}

QDataStream &operator <<(QDataStream &out, GSAbstractMessage &msg)
{
    out << msg.getSendData();
    return out;
}

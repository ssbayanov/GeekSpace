#ifndef GEEKSPACEMESSAGE_H
#define GEEKSPACEMESSAGE_H

#include <QByteArray>
#include <QDataStream>
#include <QObject>

typedef enum {
    GSM_Ping = 0x01,
    GSM_Pong = 0x02,
    GSM_CallingFunction = 0x03,
    GSM_WriteToBuffer = 0x04,
    GSM_Info = 0x05,
    GSM_CallingSignal = 0x06
} TypeOfMessage;


class GSAbstractMessage
        #ifndef QT_NO_QOBJECT
            :public QObject
        #endif
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
public:

    GSAbstractMessage();

    void setType(TypeOfMessage type);

    TypeOfMessage type();

    virtual QByteArray getSendData();

private:
    QByteArray data;
    TypeOfMessage _type;
};

class GSPingMessage : public GSAbstractMessage
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
public:
    GSPingMessage();
    QByteArray getSendData();
    static QByteArray getPingData();
};

QDataStream &operator >>(QDataStream &out, GSAbstractMessage &msg);
QDataStream &operator <<(QDataStream &in, GSAbstractMessage &msg);

#endif // GEEKSPACEMESSAGE_H

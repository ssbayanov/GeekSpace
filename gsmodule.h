#ifndef GEEKSPACEMODULE_H
#define GEEKSPACEMODULE_H

#include "gsobject.h"
#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QString>
#include <QTimer>
#include <QXmlStreamReader>

class GSModule;

typedef QList <GSModule *> ModulesList;

typedef QHash<quint8, QString> SignalsList;
typedef QHash<QString, quint8> SlotsList;

class GSModule : public GSObject
{
    Q_OBJECT
public:
    GSModule(QString address, quint16 port);
    ~GSModule();
    SlotsList getSlots();
    SignalsList getSignals();
    bool isModule();

    QString displayName();
    QString aliasName();

    quint16 port();
    QString address();

    void setAlias(QString alias);


public slots:
    void connect();
    void disconnect();
    bool isOnline();

    void updateInfo();
    void callSlotByCode(quint8 code);
    void callSlot(QString slot);

    void deleteLater();

    void setDisplayName(QString newName);

signals:
    void infoUpdated(GSObject *);
    void disconnected();
    void connected();

private slots:
    void connectionEstablished();

    void readMessage();
    void writeMessage(QString messageText);
    void haveErrorMessage(QAbstractSocket::SocketError e);

    void parseInfo(QString infoXML);
    void loadSlots(QXmlStreamReader *data);
    void loadSignals(QXmlStreamReader *data);

    void pingSlot();

private:
    QString _address;
    quint16 _port;

    QDataStream moduleStream;
    QTimer pingTimer;

    quint8 attemptsConnect;

    SlotsList _slotsHash;
    SignalsList _signalsHash;

    QString _displayName;

    quint8 pingCounts;
    bool hasPing;
    bool hasInfo;

    bool _connected;

    QTcpSocket *socket;
};

#endif // GEEKSPACEMODULE_H

#ifndef GSMODULESEARCHER_H
#define GSMODULESEARCHER_H

#include <QObject>
#include <QTimer>
#include <QUdpSocket>

#include "gsmodule.h"

class GSModuleSearcher : public QObject
{
    Q_OBJECT
public:
    explicit GSModuleSearcher(uint portBroadcast, uint portRecieve, QObject *parent = 0);
    ~GSModuleSearcher();

public slots:

signals:
    void finished();
    void hasResult(GSModule *host);
    void updateSearchStatus(QString status);

private slots:
    void finish();
    void readFromUDP();

    void broadcastDatagram();

private:
    const uint maxBroadcastCounts = 10;

    void clearModules();

    QTimer *broadcastTimer;
    QUdpSocket *inUDPSocket;
    QUdpSocket *outUDPSocket;

    QHash<QString, GSModule *> modules;

    uint _portBroadcast;
    uint _portReceive;

    uint _broadcastCount;
};

#endif // GSMODULESEARCHER_H

#include "gsmodulesearcher.h"

#include <QHostInfo>
#include <QNetworkInterface>

#include "gsmodule.h"

GSModuleSearcher::GSModuleSearcher(uint portBroadcast, uint portReceive, QObject *parent) : QObject(parent)
{
    _portBroadcast = portBroadcast;
    _portReceive = portReceive;
    _broadcastCount = 0;

    // Start listner for UDP
    outUDPSocket = new QUdpSocket(this);
    connect(outUDPSocket, SIGNAL(readyRead()), this, SLOT(readFromUDP()));
    outUDPSocket->bind(_portReceive, QUdpSocket::ShareAddress);

    // Configuration and start broadcasting
    inUDPSocket = new QUdpSocket(this);
    broadcastDatagram();

    broadcastTimer = new QTimer(this);
    broadcastTimer->start(1000);
    connect(broadcastTimer, SIGNAL(timeout()), this, SLOT(broadcastDatagram()));

}

GSModuleSearcher::~GSModuleSearcher()
{
    clearModules();
}

void GSModuleSearcher::broadcastDatagram()
{
    if(_broadcastCount >= maxBroadcastCounts) {
        QTimer::singleShot(2000, this, SLOT(finish()));
        return;
    }
    _broadcastCount++;

    emit updateSearchStatus(QString("Идёт поиск модулей.%1").arg(QString(_broadcastCount % 3, '.')));
    QByteArray datagram = "GeekSpace?";
    inUDPSocket->writeDatagram(datagram.data(), datagram.size(),
                               QHostAddress::Broadcast, _portBroadcast);


    qDebug() << "Send datagram number: " << _broadcastCount;
}

void GSModuleSearcher::clearModules()
{
    qDeleteAll(modules);
    modules.clear();
}

void GSModuleSearcher::readFromUDP()
{
    while (outUDPSocket->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(outUDPSocket->pendingDatagramSize());

        QHostAddress *address = new QHostAddress();
        outUDPSocket->readDatagram(datagram.data(), datagram.size(), address);

        QString adressText = address->toString();
        adressText = adressText.right(adressText.length() - adressText.lastIndexOf(':') -1);

        qDebug() << "Find host with adress " << adressText << " Message size: " << datagram.length();
        if(datagram.length() > 0)
            if(!modules.contains(adressText)) {
                qDebug() << "Add module with adress " << adressText;
                GSModule *module = new GSModule(adressText, 123);
                module->connect();
                module->updateInfo();
                modules.insert(adressText, module);
            }
    }
}

void GSModuleSearcher::finish()
{
    QStringList findedModules = modules.keys();
    for(int i = 0; i < findedModules.count(); i++) {
        if(modules.value(findedModules[i])->isModule()) {
            emit hasResult(modules.value(findedModules[i]));
            modules.remove(findedModules[i]);
        }
    }
    clearModules();
    emit updateSearchStatus("Поиск окончен.");
    emit finished();
}

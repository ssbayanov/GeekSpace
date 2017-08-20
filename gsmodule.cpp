#include "gsmodule.h"
#include "gsmessages.h"
#include <QHostAddress>
#include <QFile>

GSModule::GSModule(QString adress, quint16 port) :
    GSObject(TO_Module, adress)
{
    socket = new QTcpSocket();

    attemptsConnect = 0;
    QObject::connect(socket, SIGNAL(connected()), this, SLOT(connectionEstablished()));

    moduleStream.setDevice(socket);
    _address = adress;
    _port = port;
    pingCounts = 0;

    //qDebug() << "Module:" << adress << ":" << port;

    QObject::connect(&pingTimer, SIGNAL(timeout()), this, SLOT(pingSlot()));
    pingTimer.setSingleShot(false);

    QObject::connect(socket, SIGNAL(readyRead()), this, SLOT(readMessage()));
    QObject::connect(socket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(haveErrorMessage(QAbstractSocket::SocketError)));

    _connected = false;

}

GSModule::~GSModule()
{
    socket->abort();
    //qDeleteAll(_slotsHash);
    _slotsHash.clear();
}

SignalsList GSModule::getSignals()
{
    return _signalsHash;
}

SlotsList GSModule::getSlots()
{
    return _slotsHash;
}

void GSModule::connect()
{
    socket->abort();
    socket->connectToHost(QHostAddress(_address), _port);

    attemptsConnect++;
}

void GSModule::disconnect()
{
    pingTimer.stop();
    socket->abort();
}

bool GSModule::isOnline()
{
    return _connected;
}

void GSModule::updateInfo()
{
    if(socket->state() != QAbstractSocket::ConnectedState) {
        QTimer::singleShot(1000, this, SLOT(updateInfo()));
        return;
    }

    moduleStream.startTransaction();

    QByteArray m;
    m.append(0x05);

    moduleStream << m;
    socket->flush();
}

void GSModule::callSlotByCode(quint8 code)
{
    moduleStream.startTransaction();
    moduleStream << GSCallFunctionMessage::getSendData(code);

    socket->flush();
}

void GSModule::callSlot(QString slot)
{
    if(_slotsHash.contains(slot)) {
        callSlotByCode(_slotsHash.value(slot));
    }
    else {
        emit haveError(QString("GSModule.%1: Вызов неизвестного слота \"%2\".").arg(_alias).arg(slot));
    }
}

void GSModule::deleteLater()
{
    socket->abort();
    socket->deleteLater();
}

void GSModule::setDisplayName(QString newName)
{
    _displayName = newName;
}

void GSModule::connectionEstablished()
{
    pingTimer.start(5000);
//    qDebug() << "CONNECTED!!! Attempts: "
//             << attemptsConnect;
    writeMessage(QString("GSModule.%1: Подключен!").arg(_alias));

    attemptsConnect = 0;
    pingCounts = 0;

    _connected = true;
    emit connected();
}

void GSModule::readMessage()
{
    //        QByteArray messageRaw = readAll();

    //        for(int i = 0; i < messageRaw.length(); i++) {
    //            qDebug() << QString("Raw code: 0x%1").arg((quint8) messageRaw.at(i), 2, 16, QChar('0'));
    //        }

    moduleStream.startTransaction();

//    qDebug() << "Available bytes: " << socket->bytesAvailable();

    QByteArray message;

    moduleStream >> message;

    if (!moduleStream.commitTransaction())
        return;     // wait for more data

    if(message.length() > 0) {
//        qDebug() << "Have message. Code:" <<  QString("0x%1").arg(message.at(0), 2, 16, QChar('0'));
        switch (message.at(0)) {
        case GSM_Pong:
            hasPing = true;
            pingCounts = 0;
//            qDebug() << "Pong from " << socket->peerAddress();
            break;
        case GSM_Info:
//            qDebug() << message.right(message.length() - 1);
            parseInfo(message.right(message.length() - 1));
            break;
        case GSM_CallingSignal:
            if(message.length() > 1)
                if(_signalsHash.contains(message.at(1))){
//                    qDebug() << "Have signal!" << _signalsHash.value(message.at(1)) + "()";

                    emit haveSignal(_alias + "." + _signalsHash.value(message.at(1)));
                }
            break;
        default:
            break;
        }

    }
}

void GSModule::writeMessage(QString messageText)
{
    emit haveMessage(messageText);
}

void GSModule::haveErrorMessage(QAbstractSocket::SocketError e)
{
    switch (e) {
    case QAbstractSocket::RemoteHostClosedError:
        emit haveError(QString("GSModule.%1: Удалённый хост закрыл соединение.").arg(_alias));
        break;
    case QAbstractSocket::HostNotFoundError:
        emit haveError(QString("GSModule.%1: Ошибка подключения: Хост не найден.").arg(_alias));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        emit haveError(QString("GSModule.%1: Соединение разорвоно модулем.").arg(_alias));
        break;
    default:
        emit haveError(QString("GSModule.%1: Ошибка сокета: %2.").arg(_alias).arg(socket->errorString()));
    }
    pingTimer.stop();

    if(attemptsConnect <= 5) {
        QTimer::singleShot(500, this, SLOT(connect()));
    }
    else {
        QTimer::singleShot(5000, this, SLOT(connect()));
    }

    socket->abort();
    _connected = false;
    emit disconnected();
}

void GSModule::parseInfo(QString infoXML)
{
    hasInfo = true;

    QXmlStreamReader *openFile = new QXmlStreamReader(infoXML);
    while (!openFile->atEnd() && !openFile->hasError())
    {
        QXmlStreamReader::TokenType token = openFile->readNext();
        if (token == QXmlStreamReader::Invalid) {
//            qDebug() << openFile->errorString() << openFile->text();
        }
        if (token == QXmlStreamReader::StartDocument)
            continue;
        if (token == QXmlStreamReader::StartElement)
        {
            if (openFile->name() == "slots") {
                loadSlots(openFile);
                continue;
            }
            if (openFile->name() == "signals") {
                loadSignals(openFile);
                continue;
            }
            if (openFile->name() == "module") {
                _alias = openFile->attributes().value("alias").toString();
                continue;
            }
        }
    }

    emit infoUpdated(this);
}

void GSModule::loadSlots(QXmlStreamReader *data)
{
    _slotsHash.clear();

    while (!data->atEnd() && !data->hasError()){
        QXmlStreamReader::TokenType token = data->readNext();
        if (token == QXmlStreamReader::StartElement)
        {
            if (data->name() == "slot"){
                _slots.append(data->attributes().value("name").toString());

                _slotsHash.insert(data->attributes().value("name").toString(),
                                  data->attributes().value("code").toUShort());
                continue;
            }
            else {
                break;
            }
        }
        if (token == QXmlStreamReader::EndElement)
        {
            if (data->name() == "slots")
                return;
        }
    }
}

void GSModule::loadSignals(QXmlStreamReader *data)
{
    _signalsHash.clear();

    while (!data->atEnd() && !data->hasError()){
        QXmlStreamReader::TokenType token = data->readNext();
        if (token == QXmlStreamReader::StartElement)
        {
            if (data->name() == "signal"){

                _signals.append(data->attributes().value("name").toString());

                _signalsHash.insert(data->attributes().value("code").toUShort(),
                                    data->attributes().value("name").toString());
                continue;
            }
            else {
                break;
            }
        }
        if (token == QXmlStreamReader::EndElement)
        {
            if (data->name() == "signals")
                return;
        }
    }
}

void GSModule::pingSlot()
{
    if(pingCounts < 2){
        pingCounts++;
        moduleStream.startTransaction();
        moduleStream << GSPingMessage::getPingData();

        socket->flush();
    }
    else {
        disconnect();
        emit haveError(QString("GSModule.%1: Удалённый хост не отвечает. Переподключаемся.").arg(_alias));
        QTimer::singleShot(1000, this, SLOT(connect()));
    }
}

bool GSModule::isModule()
{
    return (hasInfo && hasPing);
}

QString GSModule::displayName()
{
    return _displayName;
}

quint16 GSModule::port()
{
    return _port;
}

QString GSModule::address()
{
    return _address;
}

void GSModule::setAlias(QString alias)
{
    _alias = alias;
}

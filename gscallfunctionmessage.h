#ifndef GSCALLFUNCTIONMESSAGE_H
#define GSCALLFUNCTIONMESSAGE_H

#include "gsabstractmessage.h"
#include <QHash>

typedef QHash <QString, quint8> FunctionList;

class GSCallFunctionMessage : GSAbstractMessage
{
#ifndef QT_NO_QOBJECT
    Q_OBJECT
#endif
public:
    GSCallFunctionMessage(quint8 code);

    void appendFunction(QString functionName, quint8 functionCode);

    QByteArray getSendData();
    static QByteArray getSendData(quint8 code);

private:
    quint8 functionCode;

};

#endif // GSCALLFUNCTIONMESSAGE_H

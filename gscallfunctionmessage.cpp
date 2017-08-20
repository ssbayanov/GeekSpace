#include "gscallfunctionmessage.h"

//functions.insert(functionName, functionCode);

GSCallFunctionMessage::GSCallFunctionMessage(quint8 code)
{
    setType(GSM_CallingFunction);
    functionCode = code;
}

QByteArray GSCallFunctionMessage::getSendData()
{
    QByteArray data;
    data.append((quint8) GSM_CallingFunction);
    data.append(functionCode);

    return data;
}

QByteArray GSCallFunctionMessage::getSendData(quint8 code)
{
    QByteArray data;
    data.append((quint8) GSM_CallingFunction);
    data.append(code);

    return data;
}

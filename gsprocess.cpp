#include "gsprocess.h"

extern QString shortPathName(const QString & file);

GSProcess::GSProcess(QString alias, QString process, QStringList params)
    : GSObject(TO_Process, alias)
{
    _processString = process;
    _params = params;

    _process = new QProcess(this);
    connect(_process, SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(processStateChanged(QProcess::ProcessState)));

    _signals << "started"
             << "stopped";

    _slots << "start"
           << "stop";
}

GSProcess::~GSProcess()
{
    _process->close();
    delete _process;
}

bool GSProcess::isRunninig()
{
    return (_process->state() == QProcess::Running);
}

QString GSProcess::processString()
{
    return _processString;
}

QStringList GSProcess::parametrs()
{
    return _params;
}

void GSProcess::callSlot(QString slot)
{
    if(_slots.contains(slot)) {
        QMetaObject::invokeMethod(this, slot.toStdString().c_str());
    }
}

void GSProcess::start()
{

    if (_process->state() == QProcess::NotRunning) {
        QString fullPath = _processString;
#ifdef Q_OS_WIN
        fullPath = shortPathName(fullPath);
        //workPath = shortPathName(workPath);
#endif

        //processCompiller->setWorkingDirectory(workPath);

        _process->start(_processString, _params);
        if(_process->state() == QProcess::NotRunning) {
            emit haveError(QString("GSProcess.%1: Ошибка запуска процесса \"%2\"").arg(_alias, _processString));
            return;
        }

        emit haveMessage(QString("GSProcess.%1: Процесс \"%2\" запущен").arg(_alias, _processString));
    }
}

void GSProcess::stop()
{
    if (_process->state() != QProcess::NotRunning) {
        _process->close();
        emit haveMessage(QString("GSProcess.%1: Процесс \"%2\" остановлен").arg(_alias, _processString));
    }
    else {
        emit haveError(QString("GSProcess.%1: Процесс \"%2\" не запущен").arg(_alias, _processString));
    }
}

void GSProcess::processStateChanged(QProcess::ProcessState state)
{
    switch (state) {
    case QProcess::Running:
        emit started();
        emit haveSignal(_alias + "." + "started");
        break;
    default:
        emit stopped();
        emit haveSignal(_alias + "." + "stopped");
        break;
    }
}

void GSProcess::setProcessString(const QString &processString)
{
    _processString = processString;
}

void GSProcess::setParametrs(const QStringList &params)
{
    _params = params;
}

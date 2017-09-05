#include "gsscript.h"

GSScript::GSScript(QString alias)
    : GSObject(TO_Script, alias)
{
    _slots << "run";
}

GSScript::~GSScript()
{

}

void GSScript::setScript(QStringList script)
{
    _script = script;
}

void GSScript::setScript(QString script)
{
    setScript(getScriptList(script));
}

void GSScript::callSlot(QString slot)
{
    if(_slots.contains(slot)) {
        QMetaObject::invokeMethod(this, slot.toStdString().c_str());
    }
}

void GSScript::run()
{
    for(int i = 0; i < _script.count(); i++) {
        callingSlot(_script[i]);
    }
}

QStringList GSScript::getScriptList(QString script)
{
    QStringList stringList = script.split(";");
    QStringList scriptList;

    for(int i = 0; i < stringList.count(); i++) {
        if(stringList[i].length() == 0)
            continue;
        scriptList << stringList[i].remove("\n").remove("()").remove(" ");
    }

    return scriptList;
}

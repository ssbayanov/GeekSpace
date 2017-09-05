#ifndef GSSCRIPT_H
#define GSSCRIPT_H

#include "gsobject.h"

class GSScript : public GSObject
{
    Q_OBJECT
public:
    GSScript(QString alias);
    ~GSScript();

    void setScript(QStringList script);
    void setScript(QString script);

    static QStringList getScriptList(QString script);

public slots:
    void callSlot(QString slot);
    void run();

signals:
    void callingSlot(QString);

private:
    QStringList _script;
    bool _isDraft;

};

#endif // GSSCRIPT_H

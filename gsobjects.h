#ifndef GSOBJECTS_H
#define GSOBJECTS_H

#include "gsmodule.h"
#include "gstimer.h"
#include "gssheldule.h"
#include "gsprocess.h"
#include "gsscript.h"

class GSObject;

typedef QHash <QString, GSObject *> ObjectsHash;
typedef QList <GSObject *> ObjectsList;

#endif // GSOBJECTS_H

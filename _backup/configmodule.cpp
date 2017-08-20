#include "configmodule.h"
#include <string.h>

ConfigModule::ConfigModule()
{
    first = NULL;
    functionCodeCounter = 0;
}

void ConfigModule::addFunction(char *functionName, void (*functionPtr)())
{
    Function *tmp = new Function;
    tmp->functionCode = functionCodeCounter++;
    //memcpy(tmp->functionName, functionName, strlen(functionName));
    tmp->functionPtr = functionPtr;
    tmp->next = NULL;

    if(first == NULL) {
        first = tmp;
    }
    else {
        last->next = tmp;
    }
    last = tmp;
}

void ConfigModule::callFunction(unsigned int code)
{
    void (*callingFunction)();
    callingFunction = findFunction(code);
    callingFunction();
}

void (*ConfigModule::findFunction(unsigned int code))()
{
    Function *tmp = first;
    while(tmp != NULL) {
        if(tmp->functionCode == code)
            break;

        tmp = tmp->next;
    }
    return tmp->functionPtr;
}

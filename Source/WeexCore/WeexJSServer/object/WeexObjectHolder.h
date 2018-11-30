//
// Created by Darin on 10/04/2018.
//

#ifndef WEEXV8_WEEXOBJECTHOLDER_H
#define WEEXV8_WEEXOBJECTHOLDER_H

#include "WeexGlobalObject.h"

class WeexObjectHolder {

public:
    Strong<WeexGlobalObject> m_globalObject;
    std::map<std::string, WeexGlobalObject *> m_jsInstanceGlobalObjectMap;

    std::map<std::string, WeexGlobalObject *> m_jsAppGlobalObjectMap;

    ~WeexObjectHolder();


    explicit WeexObjectHolder(bool isMultiProgress);

    void initFromIPCArguments(IPCArguments *arguments, size_t startCount, bool forAppContext);

    void initFromParams(std::vector<INIT_FRAMEWORK_PARAMS *> &params, bool forAppContext);

    WeexGlobalObject *cloneWeexObject(bool initContext, bool forAppContext);


private:
    bool isMultiProgress;

};


#endif //WEEXV8_WEEXOBJECTHOLDER_H

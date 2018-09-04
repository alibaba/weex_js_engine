//
// Created by Darin on 29/05/2018.
//

#ifndef WEEXV8_EXEJSARGS_H
#define WEEXV8_EXEJSARGS_H


#include <IPC/IPCArguments.h>
#include <include/WeexApiHeader.h>
#include <WeexCore/WeexJSServer/utils/LogUtils.h>
#include <WeexCore/WeexJSServer/utils/Utils.h>

class ExeJsArgs {
public:
    std::vector<VALUE_WITH_TYPE *> params;

    explicit ExeJsArgs(std::vector<VALUE_WITH_TYPE *> &params);

    explicit ExeJsArgs(IPCArguments *arguments, size_t startCount);

    VALUE_WITH_TYPE *copyValueToSelf(VALUE_WITH_TYPE *value_with_type);

    VALUE_WITH_TYPE *getValueWithTypePtr();

    ~ExeJsArgs() {
        freeParams(params);
    }

private:
};


#endif //WEEXV8_EXEJSARGS_H

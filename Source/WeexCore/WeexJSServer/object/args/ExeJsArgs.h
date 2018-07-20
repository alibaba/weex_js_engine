//
// Created by Darin on 29/05/2018.
//

#ifndef WEEXV8_EXEJSARGS_H
#define WEEXV8_EXEJSARGS_H


#include <IPC/IPCArguments.h>
#include <include/WeexApiHeader.h>

class ExeJsArgs {
public:
    std::vector<VALUE_WITH_TYPE *> param;

    explicit ExeJsArgs(std::vector<VALUE_WITH_TYPE *> param);

    explicit ExeJsArgs(IPCArguments *arguments, size_t startCount);

    VALUE_WITH_TYPE *copyValueToSelf(VALUE_WITH_TYPE *value_with_type);

    VALUE_WITH_TYPE *getValueWithTypePtr();

private:
};


#endif //WEEXV8_EXEJSARGS_H

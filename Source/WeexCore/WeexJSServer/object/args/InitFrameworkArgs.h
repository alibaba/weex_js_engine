//
// Created by Darin on 29/05/2018.
//

#ifndef WEEXV8_INITFRAMEWORKARGS_H
#define WEEXV8_INITFRAMEWORKARGS_H

#include <IPC/IPCArguments.h>
#include "WTFString.h"
#include <vector>
#include <include/WeexApiHeader.h>

class InitFrameworkArgs {
public:
    std::vector<INIT_FRAMEWORK_PARAMS *> params;

    explicit InitFrameworkArgs(std::vector<INIT_FRAMEWORK_PARAMS *> params);

    explicit InitFrameworkArgs(IPCArguments *arguments, size_t startCount);

    ~InitFrameworkArgs();

};


#endif //WEEXV8_INITFRAMEWORKARGS_H

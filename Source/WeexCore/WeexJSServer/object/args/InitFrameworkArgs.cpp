//
// Created by Darin on 29/05/2018.
//

#include <WeexCore/WeexJSServer/utils/Utils.h>
#include "InitFrameworkArgs.h"

InitFrameworkArgs::InitFrameworkArgs(std::vector<INIT_FRAMEWORK_PARAMS *> &params) {
    for (auto param : params) {
        auto init_framework_params = (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));

        if (init_framework_params == nullptr) {
            return;
        }

        memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));
        init_framework_params->type = genWeexByteArraySS(param->type->content, param->type->length);
        init_framework_params->value = genWeexByteArraySS(param->value->content, param->value->length);

        this->params.push_back(init_framework_params);
    }

}

InitFrameworkArgs::InitFrameworkArgs(IPCArguments *arguments, size_t startCount) {
    size_t count = arguments->getCount();

    for (size_t i = startCount; i < count; i += 2) {
        if (arguments->getType(i) != IPCType::BYTEARRAY) {
            continue;
        }
        if (arguments->getType(1 + i) != IPCType::BYTEARRAY) {
            continue;
        }
        const IPCByteArray *ba = arguments->getByteArray(1 + i);

        const IPCByteArray *ba_type = arguments->getByteArray(i);

        auto init_framework_params = (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));

        if (init_framework_params == nullptr) {
            return;
        }

        memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));

        init_framework_params->type = IPCByteArrayToWeexByteArray(ba_type);
        init_framework_params->value = IPCByteArrayToWeexByteArray(ba);

        params.push_back(init_framework_params);
    }



}

InitFrameworkArgs::~InitFrameworkArgs() {
    for (auto param : params) {
        free(param->type);
        free(param->value);
        free(param);
    }
    params.clear();
}

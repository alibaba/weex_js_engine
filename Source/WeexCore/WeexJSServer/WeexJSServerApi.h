//
// Created by Darin on 13/02/2018.
//

#ifndef WEEXV8_WEEXJSSERVERAPI_H
#define WEEXV8_WEEXJSSERVERAPI_H

#include "WeexApiHeader.h"

extern "C" {


int initFramework(const char *script, std::vector<INIT_FRAMEWORK_PARAMS *> params);


int
initAppFramework(const char *instanceId, const char *appFramework, std::vector<INIT_FRAMEWORK_PARAMS *> params);

int createAppContext(const char *instanceId, const char *jsBundle);

char *exeJSOnAppWithResult(const char *instanceId, const char *jsBundle);

int callJSOnAppContext(const char *instanceId, const char *func, std::vector<VALUE_WITH_TYPE *> params);

int destroyAppContext(const char *instanceId);

int exeJsService(const char *source);

int exeCTimeCallback(const char *source);

int exeJS(const char *instanceId, const char *nameSpace, const char *func, std::vector<VALUE_WITH_TYPE *> params);

char *exeJSWithResult(const char *instanceId, const char *nameSpace, const char *func,
                      std::vector<VALUE_WITH_TYPE *> params);

int createInstance(const char *instanceId, const char *func, const char *script, const char *opts,
                   const char *initData, const char *extendsApi);

char *exeJSOnInstance(const char *instanceId, const char *script);

int destroyInstance(const char *instanceId);

int updateGlobalConfig(const char *config);

};

#endif //WEEXV8_WEEXJSSERVERAPI_H

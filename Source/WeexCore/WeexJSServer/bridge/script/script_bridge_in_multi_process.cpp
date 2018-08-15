//
// Created by yxp on 2018/6/15.
//

#include "WeexCore/WeexJSServer/bridge/script/script_bridge_in_multi_process.h"
#include "IPC/IPCArguments.h"
#include "IPC/IPCException.h"
#include "IPC/IPCHandler.h"
#include "IPC/IPCMessageJS.h"
#include "IPC/IPCResult.h"
#include "IPC/IPCString.h"
#include "WeexCore/WeexJSServer/bridge/script/core_side_in_multi_process.h"
#include "WeexCore/WeexJSServer/bridge/script/core_side_in_simple.h"
#include "WeexCore/WeexJSServer/bridge/script/script_side_in_simple.h"
#include "WeexCore/WeexJSServer/ipc/ipc_server.h"
#include "WeexCore/WeexJSServer/utils/Utils.h"
#include "WeexCore/WeexJSServer/utils/WeexRuntime.h"

#include "WeexCore/WeexJSServer/object/WeexEnv.h"
#include "WeexCore/WeexJSServer/bridge/script/script_side_in_queue.h"


static WeexJSServer *server = nullptr;

static unsigned long parseUL(const char *s) {
    unsigned long val;
    errno = 0;
    val = strtoul(s, nullptr, 10);
    if (errno) {
        LOGE("failed to parse ul: %s %s", s, strerror(errno));
        _exit(1);
    }
    return val;
}

struct ThreadData {
    int fd;
    int fd_client;
    bool enableTrace;
};

static void *threadEntry(void *_td) {
    ThreadData *td = static_cast<ThreadData *>(_td);
    //  server = new weex::IPCServer(static_cast<int>(td->fd),
    //  static_cast<bool>(td->enableTrace));
    server = new WeexJSServer(static_cast<int>(td->fd), static_cast<int>(td->fd_client),
                              static_cast<bool>(td->enableTrace));
    // Register handler for bridge
    weex::bridge::js::ScriptBridgeInMultiProcess::RegisterIPCCallback(server->getHandler());
    nice(6);
    try {
        server->loop();
    } catch (IPCException &e) {
        LOGE("%s", e.msg());
        _exit(1);
    }
    return static_cast<void **>(nullptr);
}

extern "C" int serverMain(int argc, char **argv) {
    unsigned long fd;
    unsigned long fd_client = 0;
    unsigned long enableTrace;
    if (argc != 4) {
        LOGE("argc is not correct");
        _exit(1);
    }
    fd = parseUL(argv[1]);
    fd_client = parseUL(argv[2]);
    enableTrace = parseUL(argv[3]);
    pthread_attr_t threadAttr;
    pthread_attr_init(&threadAttr);
    pthread_attr_setstacksize(&threadAttr, 10 * 1024 * 1024);
    pthread_t thread;
    ThreadData td = {static_cast<int>(fd), static_cast<int>(fd_client), static_cast<bool>(enableTrace)};
    pthread_create(&thread, &threadAttr, threadEntry, &td);
    void *rdata;
    pthread_join(thread, &rdata);
    return 0;
}

namespace weex {
    namespace bridge {
        namespace js {

            static inline const char* GetUTF8StringFromIPCArg(IPCArguments* arguments, size_t index) {
                return arguments->getByteArray(index)->length == 0 ? nullptr : arguments->getByteArray(index)->content;
            }

            ScriptBridgeInMultiProcess *ScriptBridgeInMultiProcess::g_instance = NULL;

            ScriptBridgeInMultiProcess::ScriptBridgeInMultiProcess() {
                set_script_side(new ScriptSideInQueue());
                set_core_side(new CoreSideInMultiProcess());
            }

            ScriptBridgeInMultiProcess::~ScriptBridgeInMultiProcess() {}

            static void FillVectorOfValueWithType(std::vector<VALUE_WITH_TYPE *> &params,
                                                  IPCArguments *arguments, size_t start,
                                                  size_t end) {
                for (size_t i = start; i < end; ++i) {
                    auto value = new ValueWithType;
                    switch (arguments->getType(i)) {
                        case IPCType::DOUBLE: {
                            value->type = ParamsType::DOUBLE;
                            value->value.doubleValue = arguments->get<double>(i);
                        }
                            break;
                        case IPCType::STRING: {
                            const IPCString *ipcstr = arguments->getString(i);
                            size_t size = ipcstr->length * sizeof(uint16_t);
                            auto temp = (WeexString *) malloc(size + sizeof(WeexString));
                            memset(temp, 0, size + sizeof(WeexString));
                            temp->length = ipcstr->length;
                            memcpy(temp->content, ipcstr->content, size);
                            value->type = ParamsType::STRING;
                            value->value.string = temp;
                        }
                            break;
                        case IPCType::JSONSTRING: {
                            const IPCString *ipcstr = arguments->getString(i);
                            size_t size = ipcstr->length * sizeof(uint16_t);
                            auto temp = (WeexString *) malloc(size + sizeof(WeexString));
                            memset(temp, 0, size + sizeof(WeexString));
                            temp->length = ipcstr->length;
                            memcpy(temp->content, ipcstr->content, size);
                            value->type = ParamsType::JSONSTRING;
                            value->value.string = temp;
                        }
                            break;
                        case IPCType::BYTEARRAY: {
                            const IPCByteArray *array = arguments->getByteArray(i);
                            size_t size = array->length * sizeof(char);
                            auto temp = (WeexByteArray *) malloc(size + sizeof(WeexByteArray));
                            memset(temp, 0, size + sizeof(WeexByteArray));
                            temp->length = array->length;
                            memcpy(temp->content, array->content, size);
                            value->type = ParamsType::BYTEARRAY;
                            value->value.byteArray = temp;
                        }
                            break;
                        default: {
                            value->type = ParamsType::JSUNDEFINED;
                        }
                            break;
                    }
                    params.push_back(value);
                }
            }

            static void ClearVectorOfValueWithType(std::vector<VALUE_WITH_TYPE *> &params) {
                for (auto it = params.begin(); it != params.end(); ++it) {
                    switch ((*it)->type) {
                        case ParamsType::STRING:
                        case ParamsType::JSONSTRING:
                            free((*it)->value.string);
                            break;
                        case ParamsType::BYTEARRAY:
                            free((*it)->value.byteArray);
                            break;
                        default:
                            break;
                    }
                    delete *it;
                }
            }

            void ScriptBridgeInMultiProcess::RegisterIPCCallback(IPCHandler *handler) {
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::INITFRAMEWORK),
                                         InitFramework);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::INITAPPFRAMEWORK),
                                         InitAppFramework);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CREATEAPPCONTEXT),
                                         CreateAppContext);
                handler->registerHandler(
                        static_cast<uint32_t>(IPCJSMsg::EXECJSONAPPWITHRESULT),
                        ExecJSOnAppWithResult);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CALLJSONAPPCONTEXT),
                                         CallJSOnAppContext);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::DESTORYAPPCONTEXT),
                                         DestroyAppContext);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSSERVICE),
                                         ExecJSService);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::TAKEHEAPSNAPSHOT),
                                         TakeHeapSnapshot);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECTIMERCALLBACK),
                                         ExecTimerCallback);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJS), ExecJS);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSWITHRESULT),
                                         ExecJSWithResult);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CREATEINSTANCE),
                                         CreateInstance);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::DESTORYINSTANCE),
                                         DestroyInstance);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSONINSTANCE),
                                         ExecJSOnInstance);
                handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::UPDATEGLOBALCONFIG),
                                         UpdateGlobalConfig);
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::InitFramework(
                    IPCArguments *arguments) {
                //TODO change ScriptSide
                LOGE("ScriptBridgeInMultiProcess::InitFramework");
                static_cast<ScriptSideInQueue *>(Instance()->script_side())
                        ->setTaskQueue(new WeexTaskQueue());
                WeexEnv::getEnv()->setScriptBridge(Instance());

                // Source
                const char *source = GetUTF8StringFromIPCArg(arguments, 0);
                // Params
                size_t startCount = 1;
                size_t count = arguments->getCount();
                std::vector<INIT_FRAMEWORK_PARAMS *> params;
                for (size_t i = startCount; i < count; i += 2) {
                    if (arguments->getType(i) != IPCType::BYTEARRAY) {
                        continue;
                    }
                    if (arguments->getType(1 + i) != IPCType::BYTEARRAY) {
                        continue;
                    }
                    const IPCByteArray *ba = arguments->getByteArray(1 + i);
                    const IPCByteArray *ba_type = arguments->getByteArray(i);
                    auto init_framework_params =
                            (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));
                    if (init_framework_params == nullptr) {
                        break;
                    }
                    memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));
                    init_framework_params->type = IPCByteArrayToWeexByteArray(ba_type);
                    init_framework_params->value = IPCByteArrayToWeexByteArray(ba);
                    params.push_back(init_framework_params);
                }
                int val = Instance()->script_side()->InitFramework(source, params);
                freeInitFrameworkParams(params);
                return createInt32Result(val);
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::InitAppFramework(
                    IPCArguments *arguments) {
                const char *id = GetUTF8StringFromIPCArg(arguments, 0);
                const char *js = GetUTF8StringFromIPCArg(arguments, 1);
                LOGE("Weex jsserver IPCJSMsg::INITAPPFRAMEWORK id:%s", id);
                if (strcmp(id, "") == 0) {
                    return createInt32Result(static_cast<int32_t>(false));
                }
                // Params
                size_t startCount = 2;
                size_t count = arguments->getCount();
                std::vector<INIT_FRAMEWORK_PARAMS *> params;
                for (size_t i = startCount; i < count; i += 2) {
                    if (arguments->getType(i) != IPCType::BYTEARRAY) {
                        continue;
                    }
                    if (arguments->getType(1 + i) != IPCType::BYTEARRAY) {
                        continue;
                    }
                    const IPCByteArray *ba = arguments->getByteArray(1 + i);
                    const IPCByteArray *ba_type = arguments->getByteArray(i);
                    auto init_framework_params =
                            (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));
                    if (init_framework_params == nullptr) {
                        break;
                    }
                    memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));
                    init_framework_params->type = IPCByteArrayToWeexByteArray(ba_type);
                    init_framework_params->value = IPCByteArrayToWeexByteArray(ba);
                    params.push_back(init_framework_params);
                }
                return createInt32Result(
                        Instance()->script_side()->InitAppFramework(id, js, params));
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::CreateAppContext(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::CreateAppContext");
                // LOGE("Weex jsserver IPCJSMsg::CREATEAPPCONTEXT start");
                const char *instanceID = GetUTF8StringFromIPCArg(arguments, 0);
                const char *js = GetUTF8StringFromIPCArg(arguments, 1);
                // LOGE("Weex jsserver IPCJSMsg::CREATEAPPCONTEXT end");
                return createInt32Result(
                        Instance()->script_side()->CreateAppContext(instanceID, js));
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::ExecJSOnAppWithResult(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::ExecJSONAppWithResult");
                // LOGE("Weex jsserver IPCJSMsg::ExecJSONAppWithResult start");
                const char *instanceID = GetUTF8StringFromIPCArg(arguments, 0);
                const char *js = GetUTF8StringFromIPCArg(arguments, 1);
                char *result = const_cast<char *>(
                        Instance()->script_side()->ExecJSOnAppWithResult(instanceID, js));

                if (result == nullptr)
                    return createByteArrayResult(nullptr, 0);
                else
                    return createCharArrayResult(result);
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::CallJSOnAppContext(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::CallJSONAppContext");
                // LOGE("Weex jsserver IPCJSMsg::CallJSONAppContext start");
                const char *instanceId = GetUTF8StringFromIPCArg(arguments, 0);
                const char *func = GetUTF8StringFromIPCArg(arguments, 1);

                std::vector<VALUE_WITH_TYPE *> params;
                FillVectorOfValueWithType(params, arguments, 2, arguments->getCount());
                auto result =
                        Instance()->script_side()->CallJSOnAppContext(instanceId, func, params);
                ClearVectorOfValueWithType(params);
                return createInt32Result(result);
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::DestroyAppContext(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::DestroyAppContext");
                // LOGE("Weex jsserver IPCJSMsg::DestroyAppContext start");
                const char *instanceID = GetUTF8StringFromIPCArg(arguments, 0);
                if (strlen(instanceID) == 0) {
                    return createInt32Result(static_cast<int32_t>(false));
                }

                return createInt32Result(
                        Instance()->script_side()->DestroyAppContext(instanceID));
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::ExecJSService(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::ExecJSService");
                const char *source = GetUTF8StringFromIPCArg(arguments, 0);
                return createInt32Result(Instance()->script_side()->ExecJsService(source));
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::ExecTimerCallback(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::ExecTimerCallback");
                const char *source = GetUTF8StringFromIPCArg(arguments, 0);
                Instance()->script_side()->ExecTimeCallback(source);
                return createVoidResult();
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::ExecJS(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::ExecJS");
                const char *instanceId = GetUTF8StringFromIPCArg(arguments, 0);
                const char *namespaceStr = GetUTF8StringFromIPCArg(arguments, 1);
                const char *func = GetUTF8StringFromIPCArg(arguments, 2);

                std::vector<VALUE_WITH_TYPE *> params;
                FillVectorOfValueWithType(params, arguments, 3, arguments->getCount());
                auto result =
                        Instance()->script_side()->ExecJS(instanceId, namespaceStr, func, params);
                ClearVectorOfValueWithType(params);
                return createInt32Result(result);
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::ExecJSWithResult(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::ExecJSWithResult");
                const char *instanceId = GetUTF8StringFromIPCArg(arguments, 0);
                const char *namespaceStr = GetUTF8StringFromIPCArg(arguments, 1);
                const char *func = GetUTF8StringFromIPCArg(arguments, 2);

                std::vector<VALUE_WITH_TYPE *> params;
                FillVectorOfValueWithType(params, arguments, 3, arguments->getCount());
                WeexJSResult jsResult = Instance()->script_side()->ExecJSWithResult(
                        instanceId, namespaceStr, func, params);
                ClearVectorOfValueWithType(params);

                if (jsResult.length <= 0) {
                    return createByteArrayResult(nullptr, 0);
                }
                std::unique_ptr<IPCResult> ipcResult =
                        createByteArrayResult(jsResult.data, jsResult.length);
                WeexJSResultDataFree(jsResult);
                return ipcResult;
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::CreateInstance(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::CreateInstance");

                const char *instanceID = GetUTF8StringFromIPCArg(arguments, 0);
                const char *func = GetUTF8StringFromIPCArg(arguments, 1);
                const char *script = GetUTF8StringFromIPCArg(arguments, 2);
                const char *opts = GetUTF8StringFromIPCArg(arguments, 3);
                const char *initData = GetUTF8StringFromIPCArg(arguments, 4);
                const char *extendsApi = GetUTF8StringFromIPCArg(arguments, 5);

                return createInt32Result(Instance()->script_side()->CreateInstance(
                        instanceID, func, script, opts, initData, extendsApi));
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::DestroyInstance(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::DestroyInstance");
                const char *instanceID = GetUTF8StringFromIPCArg(arguments, 0);
                if (strlen(instanceID) == 0) {
                    LOGE("DestoryInstance instanceId is NULL");
                    return createInt32Result(static_cast<int32_t>(false));
                }

                return createInt32Result(
                        Instance()->script_side()->DestroyInstance(instanceID));
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::ExecJSOnInstance(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::ExecJSONInstance");
                const char *instanceID = GetUTF8StringFromIPCArg(arguments, 0);
                const char *script = GetUTF8StringFromIPCArg(arguments, 1);

                char *result = const_cast<char *>(
                        Instance()->script_side()->ExecJSOnInstance(instanceID, script));

                if (result == nullptr)
                    return createByteArrayResult(nullptr, 0);
                else
                    return createCharArrayResult(result);
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::UpdateGlobalConfig(
                    IPCArguments *arguments) {
                LOGE("ScriptBridgeInMultiProcess::UpdateGlobalConfig");
                const char *configString = GetUTF8StringFromIPCArg(arguments, 0);
                Instance()->script_side()->UpdateGlobalConfig(configString);
                return createVoidResult();
            }

            std::unique_ptr<IPCResult> ScriptBridgeInMultiProcess::TakeHeapSnapshot(
                    IPCArguments *arguments) {
                return createVoidResult();
            }

        }  // namespace js
    }  // namespace bridge
}  // namespace weex
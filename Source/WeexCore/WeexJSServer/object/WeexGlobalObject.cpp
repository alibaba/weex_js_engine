//
// Created by Darin on 11/02/2018.
//
#include <sys/stat.h>

#include "WeexGlobalObject.h"

#define WX_GLOBAL_CONFIG_KEY "global_switch_config"

using namespace JSC;

extern WEEX_CORE_JS_API_FUNCTIONS *weex_core_js_api_functions;

static EncodedJSValue JSC_HOST_CALL functionGCAndSweep(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallNative(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallNativeModule(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallNativeComponent(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallAddElement(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionSetTimeoutNative(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionNativeLog(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionNotifyTrimMemory(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionMarkupState(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionAtob(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionBtoa(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallCreateBody(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallUpdateFinish(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallCreateFinish(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallRefreshFinish(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallUpdateAttrs(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallUpdateStyle(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallRemoveElement(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallMoveElement(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallAddEvent(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionCallRemoveEvent(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionGCanvasLinkNative(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionSetIntervalWeex(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionClearIntervalWeex(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionT3DLinkNative(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionNativeLogContext(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionDisPatchMeaage(ExecState *);

static EncodedJSValue JSC_HOST_CALL functionPostMessage(ExecState *);

const ClassInfo WeexGlobalObject::s_info = {"global", &JSGlobalObject::s_info, nullptr,
                                            CREATE_METHOD_TABLE(WeexGlobalObject)};
const GlobalObjectMethodTable WeexGlobalObject::s_globalObjectMethodTable = {
        &supportsRichSourceInfo,
        &shouldInterruptScript,
        &javaScriptRuntimeFlags,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
};

WeexGlobalObject::WeexGlobalObject(VM &vm, Structure *structure)
        : JSGlobalObject(vm, structure, &s_globalObjectMethodTable) {
}


void WeexGlobalObject::initWXEnvironmentWithIPCArguments(IPCArguments *arguments, bool forAppContext, bool isSave) {
    size_t count = arguments->getCount();
    std::vector<INIT_FRAMEWORK_PARAMS *> params;

    for (size_t i = 1; i < count; i += 2) {
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
    initWxEnvironment(params, forAppContext, isSave);
}

void WeexGlobalObject::initWxEnvironment(std::vector<INIT_FRAMEWORK_PARAMS *> params, bool forAppContext, bool isSave) {
    VM & vm = this->vm();
    JSNonFinalObject *WXEnvironment = SimpleObject::create(vm, this);
    bool hasInitCrashHandler = false;
    for (int i = 0; i < params.size(); i++) {
        INIT_FRAMEWORK_PARAMS *param = params[i];

        String &&type = String::fromUTF8(param->type->content);
        String &&value = String::fromUTF8(param->value->content);
        if (isSave) {
            auto init_framework_params = (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));

            if (init_framework_params == nullptr) {
                return;
            }

            memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));
            init_framework_params->type = genWeexByteArraySS(param->type->content, param->type->length);
            init_framework_params->value = genWeexByteArraySS(param->value->content, param->value->length);

            m_initFrameworkParams.push_back(init_framework_params);
        }


        if (String("cacheDir") == type) {
            String path = value;
            path.append("/jsserver_crash");
            initCrashHandler(path.utf8().data());
            hasInitCrashHandler = true;
        }
        if (strstr(type.utf8().data(), WX_GLOBAL_CONFIG_KEY) != NULL) {
            const char *config = value.utf8().data();
            doUpdateGlobalSwitchConfig(config);
        }

        // --------------------------------------------------------
        // add for debug mode
        if (String("debugMode") == type && String("true") == value) {
            Weex::LogUtil::setDebugMode(true);
        }
        // --------------------------------------------------------

        //LOGE("initWxEnvironment and value is %s", value.utf8().data());
        addString(vm, WXEnvironment, param->type->content, WTFMove(value));
        //free(param);
    }

    if (!hasInitCrashHandler) {
        const char *path = getenv("CRASH_FILE_PATH");
        initCrashHandler(path);
    }
    if (forAppContext)
        addValue(vm, "__windmill_environment__", WXEnvironment);
    else
        addValue(vm, "WXEnvironment", WXEnvironment);
}

void WeexGlobalObject::initFunctionForContext() {
    VM & vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
            {"nativeLog",             JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeLogContext),  (intptr_t) (5)}},
            {"atob",                  JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionAtob),              (intptr_t) (1)}},
            {"btoa",                  JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionBtoa),              (intptr_t) (1)}},
            {"callGCanvasLinkNative", JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionGCanvasLinkNative), (intptr_t) (3)}},
            {"callT3DLinkNative",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionT3DLinkNative),     (intptr_t) (2)}},
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}

void WeexGlobalObject::initFunction() {
    VM & vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
            {"callNative",            JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallNative),          (intptr_t) (3)}},
            {"callNativeModule",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallNativeModule),    (intptr_t) (5)}},
            {"callNativeComponent",   JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallNativeComponent), (intptr_t) (5)}},
            {"callAddElement",        JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallAddElement),      (intptr_t) (5)}},
            {"setTimeoutNative",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionSetTimeoutNative),    (intptr_t) (2)}},
            {"nativeLog",             JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeLog),           (intptr_t) (5)}},
            {"notifyTrimMemory",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNotifyTrimMemory),    (intptr_t) (0)}},
            {"markupState",           JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionMarkupState),         (intptr_t) (0)}},
            {"atob",                  JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionAtob),                (intptr_t) (1)}},
            {"btoa",                  JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionBtoa),                (intptr_t) (1)}},
            {"callCreateBody",        JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallCreateBody),      (intptr_t) (3)}},
            {"callUpdateFinish",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallUpdateFinish),    (intptr_t) (3)}},
            {"callCreateFinish",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallCreateFinish),    (intptr_t) (3)}},
            {"callRefreshFinish",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallRefreshFinish),   (intptr_t) (3)}},
            {"callUpdateAttrs",       JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallUpdateAttrs),     (intptr_t) (4)}},
            {"callUpdateStyle",       JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallUpdateStyle),     (intptr_t) (4)}},
            {"callRemoveElement",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallRemoveElement),   (intptr_t) (3)}},
            {"callMoveElement",       JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallMoveElement),     (intptr_t) (5)}},
            {"callAddEvent",          JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallAddEvent),        (intptr_t) (4)}},
            {"callRemoveEvent",       JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionCallRemoveEvent),     (intptr_t) (4)}},
            {"callGCanvasLinkNative", JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionGCanvasLinkNative),   (intptr_t) (3)}},
            {"setIntervalWeex",       JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionSetIntervalWeex),     (intptr_t) (3)}},
            {"clearIntervalWeex",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionClearIntervalWeex),   (intptr_t) (1)}},
            {"callT3DLinkNative",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionT3DLinkNative),       (intptr_t) (2)}},
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}

void WeexGlobalObject::initFunctionForAppContext() {
    VM & vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
            {"nativeLog",            JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeLogContext), (intptr_t) (5)}},
            {"__dispatch_message__", JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionDisPatchMeaage),   (intptr_t) (3)}},
            {"postMessage",          JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionPostMessage),      (intptr_t) (1)}},
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}


EncodedJSValue JSC_HOST_CALL functionGCAndSweep(ExecState *exec) {
    JSLockHolder lock(exec);
    // exec->heap()->collectAllGarbage();
    return JSValue::encode(jsNumber(exec->heap()->sizeAfterLastFullCollection()));
}

EncodedJSValue JSC_HOST_CALL functionSetIntervalWeex(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionSetIntervalWeex");
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto taskChar = getCharStringFromState(state, 1);
        auto callbackChar = getCharStringFromState(state, 2);
        weex_core_js_api_functions->funcSetInterval(idChar,
                                                    taskChar,
                                                    callbackChar);

        deleteChar(idChar);

    } else {
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::SETINTERVAL));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsCString(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);
        try {
            std::unique_ptr<IPCBuffer> buffer = serializer->finish();
            std::unique_ptr<IPCResult> result = sender->send(buffer.get());
            if (result->getType() != IPCType::INT32) {
                LOGE("functionSetIntervalWeex: unexpected result: %d", result->getType());
                return JSValue::encode(jsNumber(0));
            }
            return JSValue::encode(jsNumber(result->get<int32_t>()));
        } catch (IPCException &e) {
            LOGE("functionSetIntervalWeex exception %s", e.msg());
        }
    }

    return JSValue::encode(jsNumber(0));
}

EncodedJSValue JSC_HOST_CALL functionClearIntervalWeex(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionClearIntervalWeex");
    if (weex_core_js_api_functions) {
        weex_core_js_api_functions->funcClearInterval(getCharStringFromState(state, 0),
                                                      getCharStringFromState(state, 1));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CLEARINTERVAL));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsCString(serializer, state, 1);
        try {
            std::unique_ptr<IPCBuffer> buffer = serializer->finish();
            std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        } catch (IPCException &e) {
            LOGE("functionClearIntervalWeex exception %s", e.msg());
        }
    }


    return JSValue::encode(jsBoolean(true));
}

EncodedJSValue JSC_HOST_CALL functionCallNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callNative");

    if (weex_core_js_api_functions) {
        weex_core_js_api_functions->funcCallNative(getCharStringFromState(state, 0),
                                                   getCharOrJSONStringFromState(state, 1),
                                                   getCharStringFromState(state, 2));
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVE));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsJByteArray(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallNative:unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }
}

EncodedJSValue JSC_HOST_CALL functionGCanvasLinkNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callGCanvasLinkNative");
    VM & vm = state->vm();
    if (weex_core_js_api_functions) {
        auto native = weex_core_js_api_functions->funcCallGCanvasLinkNative(getCharStringFromState(state, 0),
                                                                            state->argument(1).asInt32(),
                                                                            getCharStringFromState(state, 2));
        return JSValue::encode(String2JSValue(state, native));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLGCANVASLINK));

        //contextId args[0]
        getArgumentAsJString(serializer, state, 0);
        //type args[1]
        int32_t type = state->argument(1).asInt32();
        serializer->add(type);
        //arg args[2]
        getArgumentAsJString(serializer, state, 2);
        JSValue ret = jsUndefined();
        try {
            std::unique_ptr<IPCBuffer> buffer = serializer->finish();
            std::unique_ptr<IPCResult> result = sender->send(buffer.get());
            // LOGE("weexjsc functionGCanvasLinkNative");
            // if (result->getType() == IPCType::VOID) {
            //     return JSValue::encode(ret);
            // } else if (result->getStringLength() > 0) {
            //     WTF::String retWString = jString2String(result->getStringContent(), result->getStringLength());
            //     LOGE("weexjsc functionGCanvasLinkNative result length > 1 retWString:%s", retWString.utf8().data());
            //     ret = String2JSValue(state, retWString);

            // }
            if (result->getType() != IPCType::VOID) {
                if (result->getStringLength() > 0) {
                    ret = jString2JSValue(state, result->getStringContent(), result->getStringLength());
                }
            }
        } catch (IPCException &e) {
            LOGE("functionGCanvasLinkNative exception: %s", e.msg());
            _exit(1);
        }

        return JSValue::encode(ret);
    }


}

EncodedJSValue JSC_HOST_CALL functionT3DLinkNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionT3DLinkNative");
    VM & vm = state->vm();

    if (weex_core_js_api_functions) {
        const char *native = weex_core_js_api_functions->funcT3dLinkNative(state->argument(0).asInt32(),
                                                                           getCharStringFromState(state, 1));

        return JSValue::encode(String2JSValue(state, native));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLT3DLINK));
        //type args[1]
        int32_t type = state->argument(0).asInt32();
        serializer->add(type);
        //arg args[2]
        getArgumentAsJString(serializer, state, 1);
        JSValue ret = jsUndefined();
        try {
            std::unique_ptr<IPCBuffer> buffer = serializer->finish();
            std::unique_ptr<IPCResult> result = sender->send(buffer.get());
            // LOGE("weexjsc functionT3DLinkNative");
            // if (result->getType() == IPCType::VOID) {
            //     return JSValue::encode(ret);
            // } else if (result->getStringLength() > 0) {
            //     WTF::String retWString = jString2String(result->getStringContent(), result->getStringLength());
            //     LOGE("weexjsc functionT3DLinkNative result length > 1 retWString:%s", retWString.utf8().data());
            //     ret = String2JSValue(state, retWString);

            // }
            if (result->getType() != IPCType::VOID) {
                if (result->getStringLength() > 0) {
                    ret = jString2JSValue(state, result->getStringContent(), result->getStringLength());
                }
            }
        } catch (IPCException &e) {
            LOGE("functionT3DLinkNative exception: %s", e.msg());
            _exit(1);
        }

        return JSValue::encode(ret);
    }
}

EncodedJSValue JSC_HOST_CALL functionCallNativeModule(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callNativeModule");
    VM & vm = state->vm();
    std::unique_ptr<IPCResult> result;
    if (weex_core_js_api_functions) {
        result = weex_core_js_api_functions->funcCallNativeModule(getCharStringFromState(state, 0),
                                                                  getCharStringFromState(state, 1),
                                                                  getCharStringFromState(state, 2),
                                                                  getCharJSONStringFromState(state, 3),
                                                                  getCharJSONStringFromState(state, 4));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVEMODULE));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);

        //module args[1]
        getArgumentAsCString(serializer, state, 1);

        //method args[2]
        getArgumentAsCString(serializer, state, 2);

        // arguments args[3]
        getArgumentAsJByteArrayJSON(serializer, state, 3);
        //arguments args[4]
        getArgumentAsJByteArrayJSON(serializer, state, 4);
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        result = sender->send(buffer.get());
    }
    JSValue ret;
    switch (result->getType()) {
        case IPCType::DOUBLE:
            ret = jsNumber(result->get<double>());
            break;
        case IPCType::STRING:
            ret = jString2JSValue(state, result->getStringContent(), result->getStringLength());
            break;
        case IPCType::JSONSTRING: {
            String val = jString2String(result->getStringContent(), result->getStringLength());
            ret = parseToObject(state, val);
        }
            break;
        case IPCType::BYTEARRAY: {
            // ret = wson::toJSValue(state, (void*)result->getByteArrayContent(), result->getByteArrayLength());
        }
            break;
        default:
            ret = jsUndefined();
    }
    return JSValue::encode(ret);


}

EncodedJSValue JSC_HOST_CALL functionCallNativeComponent(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callNativeComponent");

    if (weex_core_js_api_functions) {
        weex_core_js_api_functions->funcCallNativeComponent(getCharStringFromState(state, 0),
                                                            getCharStringFromState(state, 1),
                                                            getCharStringFromState(state, 2),
                                                            getCharJSONStringFromState(state, 3),
                                                            getCharJSONStringFromState(state, 4));
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVECOMPONENT));

        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);

        //module args[1]
        getArgumentAsCString(serializer, state, 1);

        //method args[2]
        getArgumentAsCString(serializer, state, 2);

        // arguments args[3]
        getArgumentAsJByteArrayJSON(serializer, state, 3);

        //arguments args[4]
        getArgumentAsJByteArrayJSON(serializer, state, 4);
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallNativeComponent: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }


}

EncodedJSValue JSC_HOST_CALL functionCallAddElement(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callAddElement");

    if (weex_core_js_api_functions) {
        weex_core_js_api_functions->funcCallAddElement(getCharStringFromState(state, 0),
                                                       getCharStringFromState(state, 1),
                                                       getCharOrJSONStringFromState(state, 2),
                                                       getCharStringFromState(state, 3));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLADDELEMENT) | MSG_FLAG_ASYNC);
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //dom node args[2]
        getArgumentAsJByteArray(serializer, state, 2);
        //index  args[3]
        getArgumentAsCString(serializer, state, 3);
        //callback  args[4]
        getArgumentAsCString(serializer, state, 4);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
    }

    return JSValue::encode(jsNumber(0));
}

EncodedJSValue JSC_HOST_CALL functionCallCreateBody(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callCreateBody");

    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto taskChar = getCharOrJSONStringFromState(state, 1);

        LOGE("functionCallCreateBody is running and id is %s and task is %s", idChar, taskChar);

        weex_core_js_api_functions->funcCallCreateBody(idChar,
                                                       taskChar);
        deleteChar(idChar);
        deleteChar(taskChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLCREATEBODY));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsJByteArray(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallNative: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }
}

EncodedJSValue JSC_HOST_CALL functionCallUpdateFinish(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallUpdateFinish");

    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto taskChar = getCharOrJSONStringFromState(state, 1);
        auto callBackChar = getCharStringFromState(state, 2);
        weex_core_js_api_functions->funcCallUpdateFinish(idChar,
                                                         taskChar,
                                                         callBackChar);
        deleteChar(idChar);
        deleteChar(taskChar);
        deleteChar(callBackChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLUPDATEFINISH));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsJByteArray(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallUpdateFinish: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }

}

EncodedJSValue JSC_HOST_CALL functionCallCreateFinish(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallCreateFinish");

    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        weex_core_js_api_functions->funcCallCreateFinish(idChar);
        deleteChar(idChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLCREATEFINISH));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsJByteArray(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallCreateFinish: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }
}

EncodedJSValue JSC_HOST_CALL functionCallRefreshFinish(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallRefreshFinish");
    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto taskChar = getCharOrJSONStringFromState(state, 1);
        auto callBackChar = getCharStringFromState(state, 2);
        weex_core_js_api_functions->funcCallRefreshFinish(idChar,
                                                          taskChar,
                                                          callBackChar);
        deleteChar(idChar);
        deleteChar(taskChar);
        deleteChar(callBackChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLREFRESHFINISH));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //task args[1]
        getArgumentAsJByteArray(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallRefreshFinish: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }
}


EncodedJSValue JSC_HOST_CALL functionCallUpdateAttrs(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallUpdateAttrs");
    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto taskChar = getCharStringFromState(state, 1);
        auto callbackChar = getCharOrJSONStringFromState(state, 2);
        weex_core_js_api_functions->funcCallUpdateAttrs(idChar,
                                                        taskChar,
                                                        callbackChar);
        deleteChar(idChar);
        deleteChar(taskChar);
        deleteChar(callbackChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLUPDATEATTRS));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //task args[2]
        getArgumentAsJByteArray(serializer, state, 2);
        //callback args[3]
        getArgumentAsCString(serializer, state, 3);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallUpdateAttrs: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }
}

EncodedJSValue JSC_HOST_CALL functionCallUpdateStyle(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallUpdateStyle");

    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto taskChar = getCharStringFromState(state, 1);
        auto dataChar = getCharOrJSONStringFromState(state, 2);
        weex_core_js_api_functions->funcCallUpdateStyle(idChar,
                                                        taskChar,
                                                        dataChar);
        deleteChar(idChar);
        deleteChar(taskChar);
        deleteChar(dataChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLUPDATESTYLE));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //task args[2]
        getArgumentAsJByteArray(serializer, state, 2);
        //callback args[3]
        getArgumentAsCString(serializer, state, 3);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallUpdateStyle: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }
}

EncodedJSValue JSC_HOST_CALL functionCallRemoveElement(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallRemoveElement");

    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto dataChar = getCharStringFromState(state, 1);
        weex_core_js_api_functions->funcCallRemoveElement(idChar,
                                                          dataChar);
        deleteChar(idChar);
        deleteChar(dataChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLREMOVEELEMENT));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallRemoveElement: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }

}

EncodedJSValue JSC_HOST_CALL functionCallMoveElement(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallMoveElement");

    if (weex_core_js_api_functions) {
        auto index = getCharStringFromState(state, 3);
        int index_int = atoi(index);
        auto idChar = getCharStringFromState(state, 0);
        auto refChar = getCharStringFromState(state, 1);
        auto dataChar = getCharStringFromState(state, 2);
        weex_core_js_api_functions->funcCallMoveElement(idChar,
                                                        refChar,
                                                        dataChar,
                                                        index_int);
        deleteChar(idChar);
        deleteChar(refChar);
        deleteChar(dataChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLMOVEELEMENT));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);
        //callback args[3]
        getArgumentAsCString(serializer, state, 3);
        //callback args[4]
        getArgumentAsCString(serializer, state, 4);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallMoveElement: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }

}

EncodedJSValue JSC_HOST_CALL functionCallAddEvent(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallAddEvent");
    //TODO 消息发送和接收的个数不一样..
    if (weex_core_js_api_functions) {
        auto idChar = getCharStringFromState(state, 0);
        auto refChar = getCharStringFromState(state, 1);
        auto dataChar = getCharStringFromState(state, 2);
        weex_core_js_api_functions->funcCallAddEvent(idChar,
                                                     refChar,
                                                     dataChar);
        deleteChar(idChar);
        deleteChar(refChar);
        deleteChar(dataChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLADDEVENT));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);
        //callback args[3]
        getArgumentAsCString(serializer, state, 3);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallAddEvent: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }

}

EncodedJSValue JSC_HOST_CALL functionCallRemoveEvent(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallRemoveEvent");

    //TODO 消息发送和接收的个数不一样..
    if (weex_core_js_api_functions) {
        weex_core_js_api_functions->funcCallRemoveEvent(getCharStringFromState(state, 0),
                                                        getCharStringFromState(state, 1),
                                                        getCharStringFromState(state, 2));
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CALLREMOVEEVENT));
        //instacneID args[0]
        getArgumentAsCString(serializer, state, 0);
        //instacneID args[1]
        getArgumentAsCString(serializer, state, 1);
        //callback args[2]
        getArgumentAsCString(serializer, state, 2);
        //callback args[3]
        getArgumentAsCString(serializer, state, 3);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallRemoveEvent: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }

}

EncodedJSValue JSC_HOST_CALL functionSetTimeoutNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "setTimeoutNative");

    if (weex_core_js_api_functions) {
        auto callbackChar = getCharStringFromState(state, 0);
        auto timeChar = getCharStringFromState(state, 1);
        weex_core_js_api_functions->funcSetTimeout(callbackChar,
                                                   timeChar);
        deleteChar(callbackChar);
        deleteChar(timeChar);
        return JSValue::encode(jsNumber(0));
    } else {
        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::SETTIMEOUT));
        //callbackId
        getArgumentAsCString(serializer, state, 0);

        //time
        getArgumentAsCString(serializer, state, 1);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("functionCallNativeComponent: unexpected result: %d", result->getType());
            return JSValue::encode(jsNumber(0));
        }
        return JSValue::encode(jsNumber(result->get<int32_t>()));
    }


}

EncodedJSValue JSC_HOST_CALL functionNativeLog(ExecState *state) {
    bool result = false;
    StringBuilder sb;
    for (int i = 0; i < state->argumentCount(); i++) {
        sb.append(state->argument(i).toWTFString(state));
    }

    if (!sb.isEmpty()) {
        if (weex_core_js_api_functions) {
            weex_core_js_api_functions->funcCallNativeLog(sb.toString().utf8().data());
        } else {
            WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
            WeexJSServer *server = globalObject->m_server;
            IPCSender *sender = server->getSender();
            IPCSerializer *serializer = server->getSerializer();

            serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::NATIVELOG) | MSG_FLAG_ASYNC);
            // String s = sb.toString();
            // addString(serializer, s);
            CString data = sb.toString().utf8();
            serializer->add(data.data(), data.length());
            std::unique_ptr<IPCBuffer> buffer = serializer->finish();
            sender->send(buffer.get());
        }
    }
    return JSValue::encode(jsBoolean(true));
}

EncodedJSValue JSC_HOST_CALL functionNativeLogContext(ExecState *state) {
    //bool result = false;
    StringBuilder sb;
    for (int i = 0; i < state->argumentCount(); i++) {
        sb.append(state->argument(i).toWTFString(state));
    }

    if (!sb.isEmpty()) {
        if (weex_core_js_api_functions) {
            weex_core_js_api_functions->funcCallNativeLog(sb.toString().utf8().data());
        } else {
            WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
            WeexJSServer *server = globalObject->m_server;
            IPCSender *sender = server->getSender();
            IPCSerializer *serializer = server->getSerializer();

            serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::NATIVELOG) | MSG_FLAG_ASYNC);
            // String s = sb.toString();
            // addString(serializer, s);
            CString data = sb.toString().utf8();
            serializer->add(data.data(), data.length());
            std::unique_ptr<IPCBuffer> buffer = serializer->finish();
            sender->send(buffer.get());
        }
    }
    return JSValue::encode(jsBoolean(true));
}

EncodedJSValue JSC_HOST_CALL functionPostMessage(ExecState *state) {
    LOGE("functionPostMessage");
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    String id(globalObject->id.c_str());
    if (weex_core_js_api_functions) {
        auto dataChar = getCharJSONStringFromState(state, 0);
        weex_core_js_api_functions->funcCallHandlePostMessage(id.utf8().data(),
                                                              dataChar);
        deleteChar(dataChar);
    } else {
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::POSTMESSAGE));
        getArgumentAsJByteArrayJSON(serializer, state, 0);

        addString(serializer, id);
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
    }


    return JSValue::encode(jsNumber(0));
}

EncodedJSValue JSC_HOST_CALL functionDisPatchMeaage(ExecState *state) {
    LOGE("functionDisPatchMeaage");
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    String id(globalObject->id.c_str());
    if (weex_core_js_api_functions) {
        auto clientIdChar = getCharStringFromState(state, 0);
        auto dataChar = getCharJSONStringFromState(state, 1);
        auto callBackChar = getCharStringFromState(state, 2);
        weex_core_js_api_functions->funcCallDIspatchMessage(clientIdChar,
                                                            dataChar,
                                                            callBackChar,
                                                            id.utf8().data());
        deleteChar(clientIdChar);
        deleteChar(dataChar);
        deleteChar(callBackChar);
    } else {
        WeexJSServer *server = globalObject->m_server;
        IPCSender *sender = server->getSender();
        IPCSerializer *serializer = server->getSerializer();
        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::DISPATCHMESSAGE));
        // clientid
        getArgumentAsJString(serializer, state, 0);
        // data
        getArgumentAsJByteArrayJSON(serializer, state, 1);
        // callback
        getArgumentAsJString(serializer, state, 2);

        addString(serializer, id);

        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
    }


    return JSValue::encode(jsNumber(0));
}

EncodedJSValue JSC_HOST_CALL functionNotifyTrimMemory(ExecState *state) {
    return functionGCAndSweep(state);
}

EncodedJSValue JSC_HOST_CALL functionMarkupState(ExecState *state) {
    markupStateInternally();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionAtob(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "atob");
    JSValue ret = jsUndefined();
    JSValue val = state->argument(0);

    if (!val.isUndefined()) {
        String original = val.toWTFString(state);
        // std::string input_str(original.utf8().data());
        // std::string output_str;
        // Base64Decode(input_str, &output_str);
        // WTF::String s(output_str.c_str());
        // ret = jsNontrivialString(&state->vm(), WTFMove(s));
        Vector<char> out;
        if (base64Decode(original, out, Base64ValidatePadding | Base64IgnoreSpacesAndNewLines)) {
            WTF::String output = String(out.data(), out.size());
            ret = jsNontrivialString(&state->vm(), WTFMove(output));
        }
    } else {
        //ret = "";
    }
    return JSValue::encode(ret);
}

EncodedJSValue JSC_HOST_CALL functionBtoa(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "btoa");

    JSValue ret = jsUndefined();
    JSValue val = state->argument(0);
    String original = val.toWTFString(state);
    String out;
    if (original.isNull())
        out = String("");

    if (original.containsOnlyLatin1()) {
        out = base64Encode(original.latin1());
    }
    ret = jsNontrivialString(&state->vm(), WTFMove(out));
    return JSValue::encode(ret);
}

//
// Created by Darin on 11/02/2018.
//
#include <sys/stat.h>
#include <WeexCore/WeexJSServer/task/TimerTask.h>

#include "WeexGlobalObject.h"
#include "core/bridge/script_bridge.h"
#include "WeexEnv.h"
#include "WeexCore/WeexJSServer/utils/LogUtils.h"

#define WX_GLOBAL_CONFIG_KEY "global_switch_config"
//#define GET_CHARFROM_UNIPTR(str) (str) == nullptr ? nullptr : (reinterpret_cast<const char*>((str).get()))
using namespace JSC;

static bool isGlobalConfigStartUpSet = false;

//extern WEEX_CORE_JS_API_FUNCTIONS *weex_core_js_api_functions;

#define JSFUNCTION static EncodedJSValue JSC_HOST_CALL

JSFUNCTION functionGCAndSweep(ExecState *);

JSFUNCTION functionCallNative(ExecState *);

JSFUNCTION functionCallNativeModule(ExecState *);

JSFUNCTION functionCallNativeComponent(ExecState *);

JSFUNCTION functionCallAddElement(ExecState *);

JSFUNCTION functionSetTimeoutNative(ExecState *);

JSFUNCTION functionNativeLog(ExecState *);

JSFUNCTION functionNotifyTrimMemory(ExecState *);

JSFUNCTION functionMarkupState(ExecState *);

JSFUNCTION functionAtob(ExecState *);

JSFUNCTION functionBtoa(ExecState *);

JSFUNCTION functionCallCreateBody(ExecState *);

JSFUNCTION functionCallUpdateFinish(ExecState *);

JSFUNCTION functionCallCreateFinish(ExecState *);

JSFUNCTION functionCallRefreshFinish(ExecState *);

JSFUNCTION functionCallUpdateAttrs(ExecState *);

JSFUNCTION functionCallUpdateStyle(ExecState *);

JSFUNCTION functionCallRemoveElement(ExecState *);

JSFUNCTION functionCallMoveElement(ExecState *);

JSFUNCTION functionCallAddEvent(ExecState *);

JSFUNCTION functionCallRemoveEvent(ExecState *);

JSFUNCTION functionGCanvasLinkNative(ExecState *);

JSFUNCTION functionSetIntervalWeex(ExecState *);

JSFUNCTION functionClearIntervalWeex(ExecState *);

JSFUNCTION functionT3DLinkNative(ExecState *);

JSFUNCTION functionNativeLogContext(ExecState *);

JSFUNCTION functionDisPatchMeaage(ExecState *);

JSFUNCTION functionPostMessage(ExecState *);

JSFUNCTION functionNativeSetTimeout(ExecState *);


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
        : JSGlobalObject(vm, structure, &s_globalObjectMethodTable), script_bridge_() {
}

void WeexGlobalObject::SetScriptBridge(WeexCore::ScriptBridge *script_bridge) {
    script_bridge_.reset(script_bridge);
}

void WeexGlobalObject::initWxEnvironment(std::vector<INIT_FRAMEWORK_PARAMS *> &params, bool forAppContext, bool isSave) {
    VM &vm = this->vm();
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

        if (!isGlobalConfigStartUpSet) {
            if (strncmp(type.utf8().data(), WX_GLOBAL_CONFIG_KEY, strlen(WX_GLOBAL_CONFIG_KEY)) == 0) {
                const char *config = value.utf8().data();
                doUpdateGlobalSwitchConfig(config);
            }
            isGlobalConfigStartUpSet = true;
        }

        // --------------------------------------------------------
        // add for debug mode
        if (String("debugMode") == type && String("true") == value) {
            Weex::LogUtil::setDebugMode(true);
        }
        // --------------------------------------------------------

        //LOGE("initWxEnvironment and value is %s", value.utf8().data());
        addString(vm, WXEnvironment, param->type->content, WTFMove(value));
        //free(params);
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
    VM &vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
            {"nativeLog",             JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeLogContext),  (intptr_t) (5)}},
            {"atob",                  JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionAtob),              (intptr_t) (1)}},
            {"btoa",                  JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionBtoa),              (intptr_t) (1)}},
            {"callGCanvasLinkNative", JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionGCanvasLinkNative), (intptr_t) (3)}},
            {"callT3DLinkNative",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionT3DLinkNative),     (intptr_t) (2)}},
            {"setNativeTimeout",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeSetTimeout),  (intptr_t) (2)}},
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}

void WeexGlobalObject::initFunction() {
    VM &vm = this->vm();
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
            {"setNativeTimeout",      JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeSetTimeout),    (intptr_t) (2)}},
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}

void WeexGlobalObject::initFunctionForAppContext() {
    VM &vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
            {"nativeLog",            JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeLogContext), (intptr_t) (5)}},
            {"__dispatch_message__", JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionDisPatchMeaage),   (intptr_t) (3)}},
            {"postMessage",          JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionPostMessage),      (intptr_t) (1)}},
            {"setNativeTimeout",     JSC::Function, NoIntrinsic, {(intptr_t) static_cast<NativeFunction>(functionNativeSetTimeout), (intptr_t) (2)}},
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}


JSFUNCTION functionGCAndSweep(ExecState *exec) {
    JSLockHolder lock(exec);
    // exec->heap()->collectAllGarbage();
    return JSValue::encode(jsNumber(exec->heap()->sizeAfterLastFullCollection()));
}

JSFUNCTION functionSetIntervalWeex(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionSetIntervalWeex");
    LOGE("functionSetIntervalWeex");
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    JSValue id_js = state->argument(0);
    String id_str = id_js.toWTFString(state);
    JSValue task_js = state->argument(1);
    String task_str = task_js.toWTFString(state);
    JSValue callback_js = state->argument(2);
    String callback_str = callback_js.toWTFString(state);
    auto result = globalObject->js_bridge()->core_side()->SetInterval(id_str.utf8().data(),
                                                                      task_str.utf8().data(),
                                                                      callback_str.utf8().data());
    return JSValue::encode(jsNumber(result));
}

JSFUNCTION functionClearIntervalWeex(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionClearIntervalWeex");
    LOGE("functionClearIntervalWeex");
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    JSValue id_js = state->argument(0);
    String id_str = id_js.toWTFString(state);
    JSValue callback_js = state->argument(1);
    String callback_str = callback_js.toWTFString(state);

    globalObject->js_bridge()->core_side()->ClearInterval(id_str.utf8().data(), callback_str.utf8().data());

    return JSValue::encode(jsBoolean(true));
}

JSFUNCTION functionCallNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callNative");
    LOGE("functionCallNative");

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    JSValue id_js = state->argument(0);
    String id_str = id_js.toWTFString(state);
    JSValue task_js = state->argument(1);
    String task_str = task_js.toWTFString(state);
    JSValue callback_js = state->argument(2);
    String callback_str = callback_js.toWTFString(state);

    globalObject->js_bridge()->core_side()->CallNative(id_str.utf8().data(), task_str.utf8().data(),
                                                       callback_str.utf8().data());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionGCanvasLinkNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callGCanvasLinkNative");
    LOGE("functionGCanvasLinkNative");

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    JSValue id_js = state->argument(0);
    String id_str = id_js.toWTFString(state);
    int type = state->argument(1).asInt32();
    JSValue arg_js = state->argument(2);
    String arg_str = arg_js.toWTFString(state);

    auto result = globalObject->js_bridge()->core_side()->CallGCanvasLinkNative(id_str.utf8().data(),
                                                                                type, arg_str.utf8().data());
    return JSValue::encode(String2JSValue(state, result));
}

JSFUNCTION functionT3DLinkNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionT3DLinkNative");
    LOGE("functionT3DLinkNative");

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    int type = state->argument(0).asInt32();
    JSValue arg_js = state->argument(1);
    String arg_str = arg_js.toWTFString(state);

    auto result = globalObject->js_bridge()->core_side()->CallT3DLinkNative(type, arg_str.utf8().data());
    return JSValue::encode(String2JSValue(state, result));
}

JSFUNCTION functionCallNativeModule(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callNativeModule");
    LOGE("functionCallNativeModule");


    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    Args instanceId;
    Args moduleChar;
    Args methodChar;
    Args arguments;
    Args options;
    getStringArgsFromState(state, 0, instanceId);
    getStringArgsFromState(state, 1, moduleChar);
    getStringArgsFromState(state, 2, methodChar);
    getWsonOrJsonArgsFromState(state, 3, arguments);
    getWsonOrJsonArgsFromState(state, 4, options);

    auto result = globalObject->js_bridge()->core_side()->CallNativeModule(instanceId.getValue(),
                                                                           moduleChar.getValue(),
                                                                           methodChar.getValue(),
                                                                           arguments.getValue(),
                                                                           arguments.getLength(),
                                                                           options.getValue(),
                                                                           options.getLength());
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
            ret = wson::toJSValue(state, (void *) result->getByteArrayContent(), result->getByteArrayLength());
        }
            break;
        default:
            ret = jsUndefined();
    }
    return JSValue::encode(ret);


}

JSFUNCTION functionCallNativeComponent(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callNativeComponent");
    LOGE("functionCallNativeComponent");
    Args instanceId;
    Args moduleChar;
    Args methodChar;
    Args arguments;
    Args options;
    getStringArgsFromState(state, 0, instanceId);
    getStringArgsFromState(state, 1, moduleChar);
    getStringArgsFromState(state, 2, methodChar);
    getWsonOrJsonArgsFromState(state, 3, arguments);
    getWsonOrJsonArgsFromState(state, 4, options);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    globalObject->js_bridge()->core_side()->CallNativeComponent(instanceId.getValue(),
                                                                moduleChar.getValue(),
                                                                methodChar.getValue(),
                                                                arguments.getValue(),
                                                                arguments.getLength(),
                                                                options.getValue(),
                                                                options.getLength());
    return JSValue::encode(jsNumber(0));


}

JSFUNCTION functionCallAddElement(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callAddElement");
    LOGE("functionCallAddElement");
    Args instanceId;
    Args parentRefChar;
    Args domStr;
    Args index_cstr;

    getStringArgsFromState(state, 0, instanceId);
    getStringArgsFromState(state, 1, parentRefChar);
    getWsonArgsFromState(state, 2, domStr);
    getStringArgsFromState(state, 3, index_cstr);


    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    globalObject->js_bridge()->core_side()->AddElement(instanceId.getValue(),
                                                       parentRefChar.getValue(),
                                                       domStr.getValue(),
                                                       domStr.getLength(),
                                                       index_cstr.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallCreateBody(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "callCreateBody");
    LOGE("functionCallCreateBody");
    Args pageId;
    Args domStr;
    getStringArgsFromState(state, 0, pageId);
    getWsonArgsFromState(state, 1, domStr);


    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());

    globalObject->js_bridge()->core_side()->CreateBody(pageId.getValue(), domStr.getValue(), domStr.getLength());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallUpdateFinish(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallUpdateFinish");
    LOGE("functionCallUpdateFinish");

    Args idChar;
    Args taskChar;
    Args callBackChar;
    getStringArgsFromState(state, 0, idChar);
    getWsonArgsFromState(state, 1, taskChar);
    getWsonArgsFromState(state, 2, callBackChar);
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    auto result = globalObject->js_bridge()->core_side()->UpdateFinish(idChar.getValue(), taskChar.getValue(),
                                                                       taskChar.getLength(), callBackChar.getValue(),
                                                                       callBackChar.getLength());
    return JSValue::encode(jsNumber(result));
}

JSFUNCTION functionCallCreateFinish(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallCreateFinish");
    LOGE("functionCallCreateFinish");

    Args idChar;
    getStringArgsFromState(state, 0, idChar);
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->CreateFinish(idChar.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallRefreshFinish(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallRefreshFinish");
    LOGE("functionCallRefreshFinish");

    Args idChar;
    Args taskChar;
    Args callBackChar;
    getStringArgsFromState(state, 0, idChar);
    getStringArgsFromState(state, 1, taskChar);
    getStringArgsFromState(state, 2, callBackChar);
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    int result = globalObject->js_bridge()->core_side()->RefreshFinish(idChar.getValue(),
                                                                       taskChar.getValue(),
                                                                       callBackChar.getValue());
    return JSValue::encode(jsNumber(result));
}


JSFUNCTION functionCallUpdateAttrs(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallUpdateAttrs");
    LOGE("functionCallUpdateAttrs");
    Args instanceId;
    Args ref;
    Args domAttrs;
    getStringArgsFromState(state, 0, instanceId);
    getStringArgsFromState(state, 1, ref);
    getWsonArgsFromState(state, 2, domAttrs);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->UpdateAttrs(instanceId.getValue(),
                                                        ref.getValue(),
                                                        domAttrs.getValue(), domAttrs.getLength());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallUpdateStyle(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallUpdateStyle");
    LOGE("functionCallUpdateStyle");
    Args instanceId;
    Args ref;
    Args domStyles;
    getStringArgsFromState(state, 0, instanceId);
    getStringArgsFromState(state, 1, ref);
    getWsonArgsFromState(state, 2, domStyles);


    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->UpdateStyle(instanceId.getValue(),
                                                        ref.getValue(),
                                                        domStyles.getValue(), domStyles.getLength());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallRemoveElement(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallRemoveElement");
    LOGE("functionCallRemoveElement");

    Args idChar;
    Args dataChar;
    getStringArgsFromState(state, 0, idChar);
    getStringArgsFromState(state, 1, dataChar);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->RemoveElement(idChar.getValue(), dataChar.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallMoveElement(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallMoveElement");
    LOGE("functionCallMoveElement");

    Args idChar;
    Args refChar;
    Args dataChar;
    Args indexChar;
    getStringArgsFromState(state, 0, idChar);
    getStringArgsFromState(state, 1, refChar);
    getStringArgsFromState(state, 2, dataChar);
    getStringArgsFromState(state, 3, indexChar);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->MoveElement(idChar.getValue(),
                                                        refChar.getValue(),
                                                        dataChar.getValue(),
                                                        atoi(indexChar.getValue()));
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallAddEvent(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallAddEvent");
    LOGE("functionCallAddEvent");


    Args idChar;
    Args refChar;
    Args eventChar;
    getStringArgsFromState(state, 0, idChar);
    getStringArgsFromState(state, 1, refChar);
    getStringArgsFromState(state, 2, eventChar);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->AddEvent(idChar.getValue(),
                                                     refChar.getValue(),
                                                     eventChar.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionCallRemoveEvent(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "functionCallRemoveEvent");
    LOGE("functionCallRemoveEvent");

    Args idChar;
    Args refChar;
    Args eventChar;
    getStringArgsFromState(state, 0, idChar);
    getStringArgsFromState(state, 1, refChar);
    getStringArgsFromState(state, 2, eventChar);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->RemoveEvent(idChar.getValue(),
                                                        refChar.getValue(),
                                                        eventChar.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionSetTimeoutNative(ExecState *state) {
    base::debug::TraceScope traceScope("weex", "setTimeoutNative");
    LOGE("functionSetTimeoutNative");

    Args callbackChar;
    Args timeChar;
    getStringArgsFromState(state, 0, callbackChar);
    getStringArgsFromState(state, 1, timeChar);

    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->SetTimeout(callbackChar.getValue(), timeChar.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionNativeLog(ExecState *state) {
    LOGE("functionNativeLog");
    bool result = false;
    StringBuilder sb;
    for (int i = 0; i < state->argumentCount(); i++) {
        sb.append(state->argument(i).toWTFString(state));
    }

    if (!sb.isEmpty()) {

        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        globalObject->js_bridge()->core_side()->NativeLog(sb.toString().utf8().data());
    }
    return JSValue::encode(jsBoolean(true));
}

JSFUNCTION functionNativeLogContext(ExecState *state) {
    LOGE("functionNativeLogContext");
    //bool result = false;
    StringBuilder sb;
    for (int i = 0; i < state->argumentCount(); i++) {
        sb.append(state->argument(i).toWTFString(state));
    }

    if (!sb.isEmpty()) {

        WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
        globalObject->js_bridge()->core_side()->NativeLog(sb.toString().utf8().data());
    }
    return JSValue::encode(jsBoolean(true));
}

JSFUNCTION functionPostMessage(ExecState *state) {
    LOGE("functionPostMessage");

    Args id;
    Args dataChar;
    getStringArgsFromState(state, 0, id);
    getJSONArgsFromState(state, 1, dataChar);
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    globalObject->js_bridge()->core_side()->PostMessage(id.getValue(), dataChar.getValue());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionDisPatchMeaage(ExecState *state) {
    LOGE("functionDisPatchMeaage");

    Args clientIdChar;
    Args dataChar;
    Args callBackChar;
    getStringArgsFromState(state, 0, clientIdChar);
    getJSONArgsFromState(state, 1, dataChar);
    getStringArgsFromState(state, 2, callBackChar);
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    String id(globalObject->id.c_str());
    globalObject->js_bridge()->core_side()->DispatchMessage(clientIdChar.getValue(), dataChar.getValue(),
                                                            callBackChar.getValue(), id.utf8().data());
    return JSValue::encode(jsNumber(0));
}

JSFUNCTION functionNotifyTrimMemory(ExecState *state) {
    return functionGCAndSweep(state);
}

JSFUNCTION functionMarkupState(ExecState *state) {
    markupStateInternally();
    return JSValue::encode(jsUndefined());
}

JSFUNCTION functionAtob(ExecState *state) {
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

JSFUNCTION functionBtoa(ExecState *state) {
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

JSFUNCTION functionNativeSetTimeout(ExecState *state) {
    LOGE("functionNativeSetTimeout");
    WeexGlobalObject *globalObject = static_cast<WeexGlobalObject *>(state->lexicalGlobalObject());
    size_t i = state->argumentCount();
    if (i < 2)
        return JSValue::encode(jsNumber(0));
    const JSValue &value = state->argument(0);
    const JSValue &jsValue = state->argument(1);
    TimerQueue *timerQueue =WeexEnv::getEnv()->timerQueue();
    if (timerQueue != nullptr) {
        TimerTask *task = new TimerTask(globalObject->id.c_str(), value,
                                        static_cast<uint64_t>(jsValue.asInt32()), false);
        timerQueue->addTimerTask(task);
        return JSValue::encode(jsNumber(task->timeId));;
    }

    return JSValue::encode(jsNumber(0));
}
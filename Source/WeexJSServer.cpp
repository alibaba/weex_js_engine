#include "config.h"

#include "ArrayBuffer.h"
#include "ArrayPrototype.h"
#include "BuiltinExecutableCreator.h"
#include "BuiltinNames.h"
#include "ButterflyInlines.h"
#include "CodeBlock.h"
#include "Completion.h"
#include "ConfigFile.h"
#include "DOMJITGetterSetter.h"
#include "DOMJITPatchpoint.h"
#include "DOMJITPatchpointParams.h"
#include "Disassembler.h"
#include "Exception.h"
#include "ExceptionHelpers.h"
#include "GetterSetter.h"
#include "HeapProfiler.h"
#include "HeapSnapshotBuilder.h"
#include "HeapTimer.h"
#include "ICUCompatible.h"
#include "InitializeThreading.h"
#include "Interpreter.h"
#include "JIT.h"
#include "JSArray.h"
#include "JSArrayBuffer.h"
#include "JSCInlines.h"
#include "JSFunction.h"
#include "JSInternalPromise.h"
#include "JSInternalPromiseDeferred.h"
#include "JSLock.h"
#include "JSModuleLoader.h"
#include "JSNativeStdFunction.h"
#include "JSONObject.h"
#include "JSProxy.h"
#include "JSSourceCode.h"
#include "JSString.h"
#include "JSTypedArrays.h"
#include "JSWebAssemblyCallee.h"
#include "LLIntData.h"
#include "LLIntThunks.h"
#include "ObjectConstructor.h"
#include "ParserError.h"
#include "ProfilerDatabase.h"
#include "ProtoCallFrame.h"
#include "ReleaseHeapAccessScope.h"
#include "SamplingProfiler.h"
#include "ShadowChicken.h"
#include "StackVisitor.h"
#include "StrongInlines.h"
#include "StructureInlines.h"
#include "StructureRareDataInlines.h"
#include "SuperSampler.h"
#include "TestRunnerUtils.h"
#include "TypeProfilerLog.h"
#include "WasmFaultSignalHandler.h"
#include "WasmMemory.h"
#include "WasmPlan.h"
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <type_traits>
#include <wtf/CommaPrinter.h>
#include <wtf/CurrentTime.h>
#include <wtf/MainThread.h>
#include <wtf/NeverDestroyed.h>
#include <wtf/StringPrintStream.h>
#include <wtf/text/StringBuilder.h>
#include <wtf/unicode/WTFUTF8.h>
#include "Heap.h"

#if OS(WINDOWS)
#include <direct.h>
#else
#include <unistd.h>
#endif

#if HAVE(READLINE)
// readline/history.h has a Function typedef which conflicts with the WTF::Function template from WTF/Forward.h
// We #define it to something else to avoid this conflict.
#define Function ReadlineFunction
#include <readline/history.h>
#include <readline/readline.h>
#undef Function
#endif

#if HAVE(SYS_TIME_H)
#include <sys/time.h>
#endif

#if HAVE(SIGNAL_H)
#include <signal.h>
#endif

#if COMPILER(MSVC)
#include <crtdbg.h>
#include <mmsystem.h>
#include <windows.h>
#endif

#if PLATFORM(IOS) && CPU(ARM_THUMB2)
#include <arm/arch.h>
#include <fenv.h>
#endif

#if !defined(PATH_MAX)
#define PATH_MAX 4096
#endif

using namespace JSC;
using namespace WTF;

#include "Buffering/IPCBuffer.h"
#include "CrashHandler.h"
#include "IPCArguments.h"
#include "IPCByteArray.h"
#include "IPCHandler.h"
#include "IPCListener.h"
#include "IPCMessageJS.h"
#include "IPCResult.h"
#include "IPCSender.h"
#include "IPCString.h"
#include "IPCType.h"
#include "IPCFutexPageQueue.h"
#include "IPCException.h"
#include "LogUtils.h"
#include "Serializing/IPCSerializer.h"
#include "Trace.h"
#include "WeexJSServer.h"
#include "APICast.h"
#include "JSStringRef.h"
#include "JSContextRef.h"

#include "./base/base64/base64.h"

#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <wtf/text/Base64.h>
#define WX_GLOBAL_CONFIG_KEY "global_switch_config"
using namespace JSC;
using namespace WTF;

namespace {

static bool  config_use_wson  = true;

static EncodedJSValue JSC_HOST_CALL functionGCAndSweep(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCallNative(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCallNativeModule(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCallNativeComponent(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionCallAddElement(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionSetTimeoutNative(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionNativeLog(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionNotifyTrimMemory(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionMarkupState(ExecState*);
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
static EncodedJSValue JSC_HOST_CALL functionGCanvasLinkNative(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionSetIntervalWeex(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionClearIntervalWeex(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionT3DLinkNative(ExecState*);
static EncodedJSValue JSC_HOST_CALL functionNativeLogContext(ExecState*);

static void doUpdateGlobalSwitchConfig(const char* config){
    if(!config){
        return;
    }
    LOGE("doUpdateGlobalSwitchConfig %s", config);
    if(strstr(config, "wson_off") != NULL){
        config_use_wson = false;
    }else{
        config_use_wson = true;
    }
}

static bool ExecuteJavaScript(JSGlobalObject* globalObject,
    const String& source,
    const String& url,
    bool report_exceptions,
    const char* func);

static void ReportException(JSGlobalObject* globalObject,
    Exception* exception,
    const char* jinstanceid,
    const char* func);

class GlobalObject;
static void setJSFVersion(GlobalObject* globalObject);
static String exceptionToString(JSGlobalObject* globalObject, JSValue exception);
static void markupStateInternally();

static void initCrashHandler(const char* path)
{
    // const char* path = getenv("CRASH_FILE_PATH");
    LOGD("CRASH_FILE_PATH: %s:", path);
    if (path) {
        crash_handler::initializeCrashHandler(path);
        freopen(path, "w", stderr);
        chmod(path, 0600);
    }
}

static void initHeapTimer()
{
    HeapTimer::startTimerThread();
}

static void deinitHeapTimer()
{
    HeapTimer::stopTimerThread();
}

class GlobalObject : public JSGlobalObject {
private:
    GlobalObject(VM&, Structure*);

public:
    typedef JSGlobalObject Base;
    friend class WeexJSServer;
    WeexJSServer* m_server{ nullptr };
    std::map<std::string, std::string> m_initparam;

    static GlobalObject* create(VM& vm, Structure* structure)
    {
        GlobalObject* object = new (NotNull, allocateCell<GlobalObject>(vm.heap)) GlobalObject(vm, structure);
        object->finishCreation(vm);
        return object;
    }

    static const bool needsDestruction = false;

    DECLARE_INFO;
    static const GlobalObjectMethodTable s_globalObjectMethodTable;

    static Structure* createStructure(VM& vm, JSValue prototype)
    {
        return Structure::create(vm, 0, prototype, TypeInfo(GlobalObjectType, StructureFlags), info());
    }

    static RuntimeFlags javaScriptRuntimeFlags(const JSGlobalObject*) { return RuntimeFlags::createAllEnabled(); }

    void initWXEnvironment(IPCArguments* arguments);
    void initWXEnvironment(std::map<std::string, std::string> initMap);
    void initFunction();
    void initFunctionForContext();

protected:
    void finishCreation(VM& vm)
    {
        Base::finishCreation(vm);
        addFunction(vm, "gc", functionGCAndSweep, 0);
    }

    void addFunction(VM& vm, JSObject* object, const char* name, NativeFunction function, unsigned arguments)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        object->putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function));
    }

    void addFunction(VM& vm, const char* name, NativeFunction function, unsigned arguments)
    {
        addFunction(vm, this, name, function, arguments);
    }

    void addConstructableFunction(VM& vm, const char* name, NativeFunction function, unsigned arguments)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function, NoIntrinsic, function));
    }

    void addString(VM& vm, JSObject* object, const char* name, String&& value)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        JSString* jsString = jsNontrivialString(&vm, WTFMove(value));
        object->putDirect(vm, identifier, jsString);
    }

    void addValue(VM& vm, JSObject* object, const char* name, JSValue value)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        object->putDirect(vm, identifier, value);
    }

    void addValue(VM& vm, const char* name, JSValue value)
    {
        addValue(vm, this, name, value);
    }
};

class SimpleObject : public JSNonFinalObject {
public:
    SimpleObject(VM& vm, Structure* structure)
        : Base(vm, structure)
    {
    }

    typedef JSNonFinalObject Base;
    static const bool needsDestruction = false;

    static SimpleObject* create(VM& vm, JSGlobalObject* globalObject)
    {
        Structure* structure = createStructure(vm, globalObject, jsNull());
        SimpleObject* simpleObject = new (NotNull, allocateCell<SimpleObject>(vm.heap, sizeof(SimpleObject))) SimpleObject(vm, structure);
        simpleObject->finishCreation(vm);
        return simpleObject;
    }

    static void visitChildren(JSCell* cell, SlotVisitor& visitor)
    {
        SimpleObject* thisObject = jsCast<SimpleObject*>(cell);
        ASSERT_GC_OBJECT_INHERITS(thisObject, info());
        Base::visitChildren(thisObject, visitor);
    }

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

    DECLARE_INFO;
};

const ClassInfo SimpleObject::s_info = { "Object", &Base::s_info, nullptr, CREATE_METHOD_TABLE(SimpleObject) };

const ClassInfo GlobalObject::s_info = { "global", &JSGlobalObject::s_info, nullptr, CREATE_METHOD_TABLE(GlobalObject) };
const GlobalObjectMethodTable GlobalObject::s_globalObjectMethodTable = {
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

GlobalObject::GlobalObject(VM& vm, Structure* structure)
    : JSGlobalObject(vm, structure, &s_globalObjectMethodTable)
{
}

void GlobalObject::initWXEnvironment(std::map<std::string, std::string> initMap)
{
    VM& vm = this->vm();
    JSNonFinalObject* WXEnvironment = SimpleObject::create(vm, this);
    std::map<std::string, std::string> :: iterator it;
    for (it = initMap.begin(); it != initMap.end(); ++it) {
        // LOGE("initMap key:%s value:%s", it->first.c_str(), it->second.c_str());
        const char* key = it->first.c_str();
        String&& value = String::fromUTF8(it->second.c_str());
        addString(vm, WXEnvironment, key, WTFMove(value));
    }
    
    addValue(vm, "WXEnvironment", WXEnvironment);
}

void GlobalObject::initWXEnvironment(IPCArguments* arguments)
{
    size_t count = arguments->getCount();
    VM& vm = this->vm();
    JSNonFinalObject* WXEnvironment = SimpleObject::create(vm, this);

    bool hasInitCrashHandler = false;
    for (size_t i = 1; i < count; i += 2) {
        if (arguments->getType(i) != IPCType::BYTEARRAY) {
            continue;
        }
        if (arguments->getType(1 + i) != IPCType::BYTEARRAY) {
            continue;
        }
        const IPCByteArray* ba = arguments->getByteArray(1 + i);
        String&& value = String::fromUTF8(ba->content);
        // LOGE("initWXEnvironment value:%s", value.utf8().data());

        const IPCByteArray* ba_type = arguments->getByteArray(i);
        String&& type = String::fromUTF8(ba_type->content);
        if (String("cacheDir") == type) {
            // LOGE("initWXEnvironment type:%s", type.utf8().data());
            String path = value;
            path.append("/jsserver_crash");
            LOGD("initWXEnvironment value:%s", path.utf8().data());
            initCrashHandler(path.utf8().data());
            hasInitCrashHandler = true;
        }
        if(strstr(type.utf8().data(), WX_GLOBAL_CONFIG_KEY) != NULL){
             const char* config = value.utf8().data();
             doUpdateGlobalSwitchConfig(config);
        }
		// --------------------------------------------------------
        // add for debug mode
        if (String("debugMode") == type && String("true") == value) {
            Weex::LogUtil::setDebugMode(true);
        }
        // --------------------------------------------------------
        m_initparam[type.utf8().data()] = value.utf8().data();
        addString(vm, WXEnvironment, arguments->getByteArray(i)->content, WTFMove(value));
    }
    if (!hasInitCrashHandler) {
        const char* path = getenv("CRASH_FILE_PATH");
        initCrashHandler(path);
    }
    addValue(vm, "WXEnvironment", WXEnvironment);
}

void GlobalObject::initFunctionForContext() {
    VM& vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
        { "nativeLog", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionNativeLogContext), (intptr_t)(5) } },
        { "atob", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionAtob), (intptr_t)(1) } },
        { "btoa", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionBtoa), (intptr_t)(1) } },
        { "callGCanvasLinkNative", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionGCanvasLinkNative), (intptr_t)(3) } },
        { "callT3DLinkNative", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionT3DLinkNative), (intptr_t)(2) } },
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}

void GlobalObject::initFunction()
{
    VM& vm = this->vm();
    const HashTableValue JSEventTargetPrototypeTableValues[] = {
        { "callNative", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallNative), (intptr_t)(3) } },
        { "callNativeModule", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallNativeModule), (intptr_t)(5) } },
        { "callNativeComponent", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallNativeComponent), (intptr_t)(5) } },
        { "callAddElement", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallAddElement), (intptr_t)(5) } },
        { "setTimeoutNative", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionSetTimeoutNative), (intptr_t)(2) } },
        { "nativeLog", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionNativeLog), (intptr_t)(5) } },
        { "notifyTrimMemory", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionNotifyTrimMemory), (intptr_t)(0) } },
        { "markupState", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionMarkupState), (intptr_t)(0) } },
        { "atob", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionAtob), (intptr_t)(1) } },
        { "btoa", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionBtoa), (intptr_t)(1) } },
        { "callCreateBody", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallCreateBody), (intptr_t)(3) } },
        { "callUpdateFinish", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallUpdateFinish), (intptr_t)(3) } },
        { "callCreateFinish", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallCreateFinish), (intptr_t)(3) } },
        { "callRefreshFinish", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallRefreshFinish), (intptr_t)(3) } },
        { "callUpdateAttrs", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallUpdateAttrs), (intptr_t)(4) } },
        { "callUpdateStyle", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallUpdateStyle), (intptr_t)(4) } },
        { "callRemoveElement", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallRemoveElement), (intptr_t)(3) } },
        { "callMoveElement", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallMoveElement), (intptr_t)(5) } },
        { "callAddEvent", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallAddEvent), (intptr_t)(4) } },
        { "callRemoveEvent", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionCallRemoveEvent), (intptr_t)(4) } },
		{ "callGCanvasLinkNative", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionGCanvasLinkNative), (intptr_t)(3) } },
		{ "setIntervalWeex", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionSetIntervalWeex), (intptr_t)(3) } },
        { "clearIntervalWeex", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionClearIntervalWeex), (intptr_t)(1) } },
        { "callT3DLinkNative", JSC::Function, NoIntrinsic, { (intptr_t) static_cast<NativeFunction>(functionT3DLinkNative), (intptr_t)(2) } },
    };
    reifyStaticProperties(vm, JSEventTargetPrototypeTableValues, *this);
}

static void addString(IPCSerializer* serializer, const String& s)
{
    if (s.is8Bit()) {
        CString data = s.utf8();
        const char* cstr = data.data();
        size_t length = data.length();
        Vector<UChar, 1024> buffer(length);
        UChar* p = buffer.data();
        bool sourceIsAllASCII;
        if (WTF::Unicode::conversionOK == WTF::Unicode::convertUTF8ToUTF16(&cstr, cstr + length, &p, p + length, &sourceIsAllASCII)) {
            serializer->add(buffer.data(), std::distance(buffer.data(), p));
        } else {
            uint16_t tmp = 0;
            serializer->add(&tmp, 0);
        }
    } else {
        serializer->add(s.characters16(), s.length());
    }
}

static void getArgumentAsCString(IPCSerializer* serializer, ExecState* state, int argument) {
    JSValue val = state->argument(argument);
    String s = val.toWTFString(state);
    serializer->add(s.utf8().data(), s.length());
}
static void getArgumentAsJString(IPCSerializer* serializer, ExecState* state, int argument)
{
    JSValue val = state->argument(argument);
    String s = val.toWTFString(state);
    addString(serializer, s);
}

static void getArgumentAsJByteArrayJSON(IPCSerializer* serializer, ExecState* state, int argument)
{
    // jbyteArray ba = nullptr;
    if (argument >= state->argumentCount()) {
        serializer->addJSUndefined();
        return;
    }
    JSValue val = state->argument(argument);
    VM& vm = state->vm();
    if (val.isObject()) {
        auto scope = DECLARE_CATCH_SCOPE(vm);
        String str = JSONStringify(state, val, 0);
        JSC::VM& vm = state->vm();
        if (UNLIKELY(scope.exception())) {
            scope.clearException();
            serializer->addJSUndefined();
            return;
        }
        CString data = str.utf8();
        serializer->add(data.data(), data.length());
    }
}

String jString2String(const uint16_t* str, size_t length)
{
    UChar* dst;
    String s = String::createUninitialized(length, dst);
    memcpy(dst, str, length * sizeof(UChar));
    return s;
}

static JSValue jString2JSValue(ExecState* state, const uint16_t* str, size_t length)
{
    String s = jString2String(str, length);
    if (s.isEmpty()) {
        return jsEmptyString(&state->vm());
    } else if (s.length() == 1) {
        return jsSingleCharacterString(&state->vm(), s[0]);
    }
    return jsNontrivialString(&state->vm(), WTFMove(s));
}

static JSValue String2JSValue(ExecState* state, String s)
{
    if (s.isEmpty()) {
        return jsEmptyString(&state->vm());
    } else if (s.length() == 1) {
        return jsSingleCharacterString(&state->vm(), s[0]);
    }
    return jsNontrivialString(&state->vm(), WTFMove(s));
}

static JSValue parseToObject(ExecState* state, const String& data)
{
    VM& vm = state->vm();
    auto scope = DECLARE_CATCH_SCOPE(vm);
    JSValue ret = JSONParse(state, data);
    if (UNLIKELY(scope.exception())) {
        scope.clearException();
        return jsUndefined();
    }
    if (!ret)
        return jsUndefined();
    return ret;
}

static void getArgumentAsJByteArray(IPCSerializer* serializer, ExecState* state, int argument)
{
    // jbyteArray ba = nullptr;
    if (argument >= state->argumentCount()) {
        serializer->addJSUndefined();
        return;
    }
    JSValue val = state->argument(argument);
    if (val.isString()) {
        String str(val.toWTFString(state));
        CString data = str.utf8();
        serializer->add(data.data(), data.length());
    } else {
        getArgumentAsJByteArrayJSON(serializer, state, argument);
    }
}

EncodedJSValue JSC_HOST_CALL functionGCAndSweep(ExecState* exec)
{
    JSLockHolder lock(exec);
    // exec->heap()->collectAllGarbage();
    return JSValue::encode(jsNumber(exec->heap()->sizeAfterLastFullCollection()));
}

EncodedJSValue JSC_HOST_CALL functionSetIntervalWeex(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionSetIntervalWeex");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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
    } catch (IPCException& e) {
        LOGE("functionSetIntervalWeex exception %s", e.msg());
    }
    return JSValue::encode(jsNumber(0));
}

EncodedJSValue JSC_HOST_CALL functionClearIntervalWeex(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionClearIntervalWeex");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
    serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::CLEARINTERVAL));
    //instacneID args[0]
    getArgumentAsCString(serializer, state, 0);
    //task args[1]
    getArgumentAsCString(serializer, state, 1);
    try {
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sender->send(buffer.get());
    } catch (IPCException& e) {
        LOGE("functionClearIntervalWeex exception %s", e.msg());
    }
    return JSValue::encode(jsBoolean(true));
}

EncodedJSValue JSC_HOST_CALL functionCallNative(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "callNative");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionGCanvasLinkNative(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "callGCanvasLinkNative");

    VM& vm = state->vm();
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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
            if ( result->getStringLength() > 0) {
                ret = jString2JSValue(state, result->getStringContent(), result->getStringLength());
            }
        }
    } catch (IPCException& e) {
        LOGE("functionGCanvasLinkNative exception: %s", e.msg());
        _exit(1);
    }
    
    return JSValue::encode(ret);
}

EncodedJSValue JSC_HOST_CALL functionT3DLinkNative(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionT3DLinkNative");

    VM& vm = state->vm();
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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
            if ( result->getStringLength() > 0) {
                ret = jString2JSValue(state, result->getStringContent(), result->getStringLength());
            }
        }
    } catch (IPCException& e) {
        LOGE("functionT3DLinkNative exception: %s", e.msg());
        _exit(1);
    }
    
    return JSValue::encode(ret);
}

EncodedJSValue JSC_HOST_CALL functionCallNativeModule(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "callNativeModule");
    VM& vm = state->vm();

    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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
    std::unique_ptr<IPCResult> result = sender->send(buffer.get());
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
    } break;
    case IPCType::BYTEARRAY: {
        // ret = wson::toJSValue(state, (void*)result->getByteArrayContent(), result->getByteArrayLength());
    } break;
    default:
        ret = jsUndefined();
    }
    return JSValue::encode(ret);
}

EncodedJSValue JSC_HOST_CALL functionCallNativeComponent(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "callNativeComponent");

    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallAddElement(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "callAddElement");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

    return JSValue::encode(jsNumber(0));
}

EncodedJSValue JSC_HOST_CALL functionCallCreateBody(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "callCreateBody");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallUpdateFinish(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallUpdateFinish");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallCreateFinish(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallCreateFinish");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallRefreshFinish(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallRefreshFinish");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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


EncodedJSValue JSC_HOST_CALL functionCallUpdateAttrs(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallUpdateAttrs");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallUpdateStyle(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallUpdateStyle");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallRemoveElement(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallRemoveElement");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallMoveElement(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallMoveElement");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallAddEvent(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallAddEvent");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionCallRemoveEvent(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "functionCallRemoveEvent");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionSetTimeoutNative(ExecState* state)
{
    base::debug::TraceScope traceScope("weex", "setTimeoutNative");
    GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
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

EncodedJSValue JSC_HOST_CALL functionNativeLog(ExecState* state)
{
    bool result = false;
    StringBuilder sb;
    for (int i = 0; i < state->argumentCount(); i++) {
        sb.append(state->argument(i).toWTFString(state));
    }

    if (!sb.isEmpty()) {
        GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
        WeexJSServer* server = globalObject->m_server;
        IPCSender* sender = server->getSender();
        IPCSerializer* serializer = server->getSerializer();

        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::NATIVELOG) | MSG_FLAG_ASYNC);
        // String s = sb.toString();
        // addString(serializer, s);
        CString data = sb.toString().utf8();
        serializer->add(data.data(), data.length());
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        sender->send(buffer.get());
    }
    return JSValue::encode(jsBoolean(true));
}

EncodedJSValue JSC_HOST_CALL functionNativeLogContext(ExecState* state)
{
    bool result = false;

    StringBuilder sb;
    for (int i = 0; i < state->argumentCount(); i++) {
        sb.append(state->argument(i).toWTFString(state));
    }

    if (!sb.isEmpty()) {
        GlobalObject* globalObject = static_cast<GlobalObject*>(state->lexicalGlobalObject());
        WeexJSServer* server = globalObject->m_server;
        IPCSender* sender = server->getSender();
        IPCSerializer* serializer = server->getSerializer();

        serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::NATIVELOG) | MSG_FLAG_ASYNC);
        // String s = sb.toString();
        // addString(serializer, s);
        CString data = sb.toString().utf8();
        serializer->add(data.data(), data.length());
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        sender->send(buffer.get());
    }
    return JSValue::encode(jsBoolean(true));
}

EncodedJSValue JSC_HOST_CALL functionNotifyTrimMemory(ExecState* state)
{
    return functionGCAndSweep(state);
}

EncodedJSValue JSC_HOST_CALL functionMarkupState(ExecState* state)
{
    markupStateInternally();
    return JSValue::encode(jsUndefined());
}

EncodedJSValue JSC_HOST_CALL functionAtob(ExecState *state)
{
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

EncodedJSValue JSC_HOST_CALL functionBtoa(ExecState *state)
{
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

/**
 * this function is to execute a section of JavaScript content.
 */
bool ExecuteJavaScript(JSGlobalObject* globalObject,
    const String& source,
    const String& url,
    bool report_exceptions,
    const char* func)
{
    SourceOrigin sourceOrigin(String::fromUTF8("(weex)"));
    NakedPtr<Exception> evaluationException;
    JSValue returnValue = evaluate(globalObject->globalExec(), makeSource(source, sourceOrigin, url), JSValue(), evaluationException);
    if (report_exceptions && evaluationException) {
        ReportException(globalObject, evaluationException.get(), "", func);
    }
    if (evaluationException)
        return false;
    globalObject->vm().drainMicrotasks();
    return true;
}

void ReportException(JSGlobalObject* _globalObject, Exception* exception, const char* instanceid,
    const char* func)
{
    String exceptionInfo = exceptionToString(_globalObject, exception->value());
    CString data = exceptionInfo.utf8();
    GlobalObject* globalObject = static_cast<GlobalObject*>(_globalObject);
    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
    serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::REPORTEXCEPTION));
    serializer->add(instanceid, strlen(instanceid));
    serializer->add(func, strlen(func));
    serializer->add(data.data(), data.length());

    std::unique_ptr<IPCBuffer> buffer = serializer->finish();
    std::unique_ptr<IPCResult> result = sender->send(buffer.get());
    if (result->getType() != IPCType::VOID) {
        LOGE("REPORTEXCEPTION: unexpected result: %d", result->getType());
    }
}

void setJSFVersion(GlobalObject* globalObject)
{
    VM& vm = globalObject->vm();
    PropertyName getJSFMVersionProperty(Identifier::fromString(&vm, "getJSFMVersion"));
    ExecState* state = globalObject->globalExec();
    JSValue getJSFMVersionFunction = globalObject->get(state, getJSFMVersionProperty);
    MarkedArgumentBuffer args;
    CallData callData;
    CallType callType = getCallData(getJSFMVersionFunction, callData);
    NakedPtr<Exception> returnedException;
    JSValue version = call(globalObject->globalExec(), getJSFMVersionFunction, callType, callData, globalObject, args, returnedException);
    if (returnedException) {
        ReportException(globalObject, returnedException.get(), "", "");
    }
    String str = version.toWTFString(state);
    CString data = str.utf8();

    WeexJSServer* server = globalObject->m_server;
    IPCSender* sender = server->getSender();
    IPCSerializer* serializer = server->getSerializer();
    serializer->setMsg(static_cast<uint32_t>(IPCProxyMsg::SETJSFVERSION));
    serializer->add(data.data(), data.length());
    std::unique_ptr<IPCBuffer> buffer = serializer->finish();
    std::unique_ptr<IPCResult> result = sender->send(buffer.get());
    if (result->getType() != IPCType::VOID) {
        LOGE("setJSFVersion: unexpected result: %d", result->getType());
    }
}

static String exceptionToString(JSGlobalObject* globalObject, JSValue exception)
{
    StringBuilder sb;
    VM& vm = globalObject->vm();
    auto scope = DECLARE_CATCH_SCOPE(vm);

#define CHECK_EXCEPTION()           \
    do {                            \
        if (scope.exception()) {    \
            scope.clearException(); \
            return String();        \
        }                           \
    } while (false)

    sb.append(String::format("Exception: %s\n", exception.toWTFString(globalObject->globalExec()).utf8().data()));

    Identifier nameID = Identifier::fromString(globalObject->globalExec(), "name");
    CHECK_EXCEPTION();
    Identifier fileNameID = Identifier::fromString(globalObject->globalExec(), "sourceURL");
    CHECK_EXCEPTION();
    Identifier lineNumberID = Identifier::fromString(globalObject->globalExec(), "line");
    CHECK_EXCEPTION();
    Identifier stackID = Identifier::fromString(globalObject->globalExec(), "stack");
    CHECK_EXCEPTION();

    JSValue nameValue = exception.get(globalObject->globalExec(), nameID);
    CHECK_EXCEPTION();
    JSValue fileNameValue = exception.get(globalObject->globalExec(), fileNameID);
    CHECK_EXCEPTION();
    JSValue lineNumberValue = exception.get(globalObject->globalExec(), lineNumberID);
    CHECK_EXCEPTION();
    JSValue stackValue = exception.get(globalObject->globalExec(), stackID);
    CHECK_EXCEPTION();

    if (nameValue.toWTFString(globalObject->globalExec()) == "SyntaxError"
        && (!fileNameValue.isUndefinedOrNull() || !lineNumberValue.isUndefinedOrNull())) {
        sb.append(String::format(
            "at %s:%s\n",
            fileNameValue.toWTFString(globalObject->globalExec()).utf8().data(),
            lineNumberValue.toWTFString(globalObject->globalExec()).utf8().data()));
    }

    if (!stackValue.isUndefinedOrNull())
        sb.append(String::format("%s\n", stackValue.toWTFString(globalObject->globalExec()).utf8().data()));

#undef CHECK_EXCEPTION
    return sb.toString();
}
static void markupStateInternally()
{
}

class unique_fd {
public:
    explicit unique_fd(int fd);
    ~unique_fd();
    int get() const;

private:
    int m_fd;
};

unique_fd::unique_fd(int fd)
    : m_fd(fd)
{
}

unique_fd::~unique_fd()
{
    close(m_fd);
}

int unique_fd::get() const
{
    return m_fd;
}

#define FAIL_WITH_STRERROR(tag) \
    LOGE(" fails: %s.\n", strerror(errno)); \
    return false;

#define MAYBE_FAIL_WITH_ICU_ERROR(s) \
    if (status != U_ZERO_ERROR) {\
        LOGE("Couldn't initialize ICU (" "): %s (%s)" "\n", u_errorName(status), path.c_str()); \
        return false; \
    }

extern "C" {
void udata_setCommonData(const void *data, UErrorCode *pErrorCode);
}

static bool mapIcuData(const std::string& path)
{
    // Open the file and get its length.
    unique_fd fd(open(path.c_str(), O_RDONLY));
    if (fd.get() == -1) {
        FAIL_WITH_STRERROR("open");
    }
    struct stat sb;
    if (fstat(fd.get(), &sb) == -1) {
        FAIL_WITH_STRERROR("stat");
    }

    // Map it.
    void* data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd.get(), 0);
    if (data == MAP_FAILED) {
        FAIL_WITH_STRERROR("mmap");
    }

    // Tell the kernel that accesses are likely to be random rather than sequential.
    if (madvise(data, sb.st_size, MADV_RANDOM) == -1) {
        FAIL_WITH_STRERROR("madvise(MADV_RANDOM)");
    }

    UErrorCode status = U_ZERO_ERROR;

    // Tell ICU to use our memory-mapped data.
    udata_setCommonData(data, &status);
    MAYBE_FAIL_WITH_ICU_ERROR("udata_setCommonData");

    return true;
}

static bool initICUEnv()
{
    const char* path = getenv("ICU_DATA_PATH");
    if (!path) {
        return false;
    }
    if (!dlopen("libicuuc.so", RTLD_NOW)) {
        LOGE("load icuuc so");
        return false;
    }
    if (!dlopen("libicui18n.so", RTLD_NOW)) {
        LOGE("load icui18n so");
        return false;
    }
    if (!initICU()) {
        LOGE("initICU failed");
        return false;
    }
    return mapIcuData(std::string(path));
}
}

struct WeexJSServer::WeexJSServerImpl {
    WeexJSServerImpl(int fd, bool enableTrace);
    bool enableTrace;
    RefPtr<VM> globalVM;
    Strong<JSGlobalObject> globalObject;
    std::map<std::string, GlobalObject*> mJSGlobalObjectMap;
    std::map<std::string, std::string> mInitPrams;
    std::unique_ptr<IPCFutexPageQueue> futexPageQueue;
    std::unique_ptr<IPCSender> sender;
    std::unique_ptr<IPCHandler> handler;
    std::unique_ptr<IPCListener> listener;
    std::unique_ptr<IPCSerializer> serializer;
};

WeexJSServer::WeexJSServerImpl::WeexJSServerImpl(int _fd, bool _enableTrace)
    : enableTrace(_enableTrace)
{
    void* base = mmap(nullptr, IPCFutexPageQueue::ipc_size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
    if (base == MAP_FAILED) {
        int _errno = errno;
        close(_fd);
        throw IPCException("failed to map ashmem region: %s", strerror(_errno));
    }
    close(_fd);
    futexPageQueue.reset(new IPCFutexPageQueue(base, IPCFutexPageQueue::ipc_size, 1));
    handler = std::move(createIPCHandler());
    sender = std::move(createIPCSender(futexPageQueue.get(), handler.get()));
    listener = std::move(createIPCListener(futexPageQueue.get(), handler.get()));
    serializer = std::move(createIPCSerializer());
}

WeexJSServer::WeexJSServer(int fd, bool enableTrace)
    : m_impl(new WeexJSServerImpl(fd, enableTrace))
{
    IPCHandler* handler = m_impl->handler.get();
    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::INITFRAMEWORK), [this](IPCArguments* arguments) {

        base::debug::TraceEvent::StartATrace(nullptr);

        base::debug::TraceScope traceScope("weex", "initFramework");
        if (!initICUEnv()) {
            LOGE("failed to init ICUEnv");
            return createInt32Result(static_cast<int32_t>(false));
        }
        // initCrashHandler();
        Options::enableRestrictedOptions(true);

        // Initialize JSC before getting VM.
        WTF::initializeMainThread();
        initHeapTimer();
        JSC::initializeThreading();
#if ENABLE(WEBASSEMBLY)
        JSC::Wasm::enableFastMemory();
#endif

        m_impl->globalVM = std::move(VM::create(LargeHeap));
        VM& vm = *m_impl->globalVM.get();
        JSLockHolder locker(&vm);

        int result;

        GlobalObject* globalObject = GlobalObject::create(vm, GlobalObject::createStructure(vm, jsNull()));
        globalObject->m_server = this;
        m_impl->globalObject.set(vm, globalObject);
        globalObject->initWXEnvironment(arguments);
        m_impl->mInitPrams = globalObject->m_initparam;
        globalObject->initFunction();
        const IPCString* ipcSource = arguments->getString(0);
        String source = jString2String(ipcSource->content, ipcSource->length);
        if (!ExecuteJavaScript(globalObject, source, "(weex framework)", true, "initFramework")) {

            return createInt32Result(static_cast<int32_t>(false));
        }

        setJSFVersion(globalObject);
        return createInt32Result(static_cast<int32_t>(true));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSSERVICE), [this](IPCArguments* arguments) {
        base::debug::TraceScope traceScope("weex", "exeJSService");
        JSGlobalObject* globalObject = m_impl->globalObject.get();
        const IPCString* ipcSource = arguments->getString(0);
        String source = jString2String(ipcSource->content, ipcSource->length);
        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);
        if (!ExecuteJavaScript(globalObject, source, ("weex service"), true, "execjsservice")) {
            LOGE("jsLog JNI_Error >>> scriptStr :%s", source.utf8().data());
            return createInt32Result(static_cast<int32_t>(false));
        }
        return createInt32Result(static_cast<int32_t>(true));

    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::TAKEHEAPSNAPSHOT), [this](IPCArguments* arguments) {
        return createVoidResult();

    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECTIMERCALLBACK), [this](IPCArguments* arguments) {
        base::debug::TraceScope traceScope("weex", "EXECTIMERCALLBACK");
        LOGE("IPC EXECTIMERCALLBACK and ExecuteJavaScript");
        JSGlobalObject* globalObject = m_impl->globalObject.get();
        const IPCByteArray* ipcSource = arguments->getByteArray(0);
        String&& value = String::fromUTF8(ipcSource->content);
        String source = value;
        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);
        if (!ExecuteJavaScript(globalObject, source, ("weex service"), false, "timercallback")) {
            LOGE("jsLog EXECTIMERCALLBACK >>> scriptStr :%s", source.utf8().data());
        }
        return createVoidResult();
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJS), [this](IPCArguments* arguments) {
        
        const IPCString* ipcInstanceId = arguments->getString(0);
        const IPCString* ipcNamespaceStr = arguments->getString(1);
        const IPCString* ipcFunc = arguments->getString(2);
        String instanceId = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String namespaceStr = jString2String(ipcNamespaceStr->content, ipcNamespaceStr->length);
        String func = jString2String(ipcFunc->content, ipcFunc->length);
        // LOGE("EXECJS func:%s", func.utf8().data());
        JSGlobalObject* globalObject;
        // fix instanceof Object error
        // if function is callJs on instance, should us Instance object to call __WEEX_CALL_JAVASCRIPT__
        if (std::strcmp("callJS" ,func.utf8().data()) == 0) {
                globalObject = m_impl->mJSGlobalObjectMap[instanceId.utf8().data()];
            if (globalObject == NULL) {
                globalObject = m_impl->globalObject.get();
            } else {
                WTF::String funcWString("__WEEX_CALL_JAVASCRIPT__");
                func = funcWString;
            }
        } else {
            globalObject = m_impl->globalObject.get();
        }
        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);

        MarkedArgumentBuffer obj;
        base::debug::TraceScope traceScope("weex", "exeJS", "function", func.utf8().data());
        ExecState* state = globalObject->globalExec();
        size_t count = arguments->getCount();
        for (size_t i = 3; i < count; ++i) {
            switch (arguments->getType(i)) {
            case IPCType::DOUBLE:
                obj.append(jsNumber(arguments->get<double>(i)));
                break;
            case IPCType::STRING: {
                const IPCString* ipcstr = arguments->getString(i);
                obj.append(jString2JSValue(state, ipcstr->content, ipcstr->length));
            } break;
            case IPCType::JSONSTRING: {
                const IPCString* ipcstr = arguments->getString(i);

                String str = jString2String(ipcstr->content, ipcstr->length);

                JSValue o = parseToObject(state, str);
                obj.append(o);
            } break;
            case IPCType::BYTEARRAY: {
                // const IPCByteArray* array = arguments->getByteArray(i);
                // JSValue o = wson::toJSValue(state, (void*)array->content, array->length);
                // obj.append(o);
            } break;
            default:
                obj.append(jsUndefined());
                break;
            }
        }

        Identifier funcIdentifier = Identifier::fromString(&vm, func);

        JSValue function;
        JSValue result;
        if (namespaceStr.isEmpty()) {
            function = globalObject->get(state, funcIdentifier);
        } else {
            Identifier namespaceIdentifier = Identifier::fromString(&vm, namespaceStr);
            JSValue master = globalObject->get(state, namespaceIdentifier);
            if (!master.isObject()) {
                return createInt32Result(static_cast<int32_t>(false));
            }
            function = master.toObject(state)->get(state, funcIdentifier);
        }
        CallData callData;
        CallType callType = getCallData(function, callData);
        NakedPtr<Exception> returnedException;
        JSValue ret = call(state, function, callType, callData, globalObject, obj, returnedException);

        globalObject->vm().drainMicrotasks();

        if (returnedException) {
            ReportException(globalObject, returnedException.get(), instanceId.utf8().data(), func.utf8().data());
            return createInt32Result(static_cast<int32_t>(false));
        }
        return createInt32Result(static_cast<int32_t>(true));

    });


    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSWITHRESULT), [this](IPCArguments* arguments) {
        const IPCString* ipcInstanceId = arguments->getString(0);
        const IPCString* ipcNamespaceStr = arguments->getString(1);
        const IPCString* ipcFunc = arguments->getString(2);
        String instanceId = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String namespaceStr = jString2String(ipcNamespaceStr->content, ipcNamespaceStr->length);
        String func = jString2String(ipcFunc->content, ipcFunc->length);
        // LOGE("EXECJS func:%s", func.utf8().data());
        JSGlobalObject* globalObject;
        // fix instanceof Object error
        // if function is callJs should us Instance object to call __WEEX_CALL_JAVASCRIPT__
        if (std::strcmp("callJS" ,func.utf8().data()) == 0) {
            globalObject = m_impl->mJSGlobalObjectMap[instanceId.utf8().data()];
            if (globalObject == NULL) {
                globalObject = m_impl->globalObject.get();
            } else {
                WTF::String funcWString("__WEEX_CALL_JAVASCRIPT__");
                func = funcWString;
            }
        } else {
            globalObject = m_impl->globalObject.get();
        }
        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);
         
        MarkedArgumentBuffer obj;
        base::debug::TraceScope traceScope("weex", "exeJSWithResult", "function", func.utf8().data());
        ExecState* state = globalObject->globalExec();
        size_t count = arguments->getCount();
        for (size_t i = 3; i < count; ++i) {
            switch (arguments->getType(i)) {
            case IPCType::DOUBLE:
                obj.append(jsNumber(arguments->get<double>(i)));
                break;
            case IPCType::STRING: {
                const IPCString* ipcstr = arguments->getString(i);
                obj.append(jString2JSValue(state, ipcstr->content, ipcstr->length));
            } break;
            case IPCType::JSONSTRING: {
                const IPCString* ipcstr = arguments->getString(i);

                String str = jString2String(ipcstr->content, ipcstr->length);

                JSValue o = parseToObject(state, str);
                obj.append(o);
            } break;
            case IPCType::BYTEARRAY: {
                // const IPCByteArray* array = arguments->getByteArray(i);
                // JSValue o = wson::toJSValue(state, (void*)array->content, array->length);
                // obj.append(o);
            } break;
            default:
                obj.append(jsUndefined());
                break;
            }
        }
        Identifier funcIdentifier = Identifier::fromString(&vm, func);
        JSValue function;
        JSValue result;
        if (namespaceStr.isEmpty()) {
            function = globalObject->get(state, funcIdentifier);
        } else {
            Identifier namespaceIdentifier = Identifier::fromString(&vm, namespaceStr);
            JSValue master = globalObject->get(state, namespaceIdentifier);
            if (!master.isObject()) {
                return createByteArrayResult(nullptr, 0);
            }
            function = master.toObject(state)->get(state, funcIdentifier);
        }
        CallData callData;
        CallType callType = getCallData(function, callData);
        NakedPtr<Exception> returnedException;
        JSValue ret = call(state, function, callType, callData, globalObject, obj, returnedException);
        globalObject->vm().drainMicrotasks();

        if (returnedException) {
            ReportException(globalObject, returnedException.get(), instanceId.utf8().data(), func.utf8().data());
            return createByteArrayResult(nullptr, 0);
        }
        if(ret.isUndefined() || ret.isNull() || !isJSArray(ret)){
            // createInstance return whole source object, which is big, only accept array result
            return createByteArrayResult(nullptr, 0);
        }
        /** most scene, return result is array of null */
        JSArray* array = asArray(ret);
        uint32_t length = array->length();
        bool isAllNull = true;
        for(uint32_t i=0; i<length; i++){
            JSValue ele = array->getIndex(state, i);
            if(!ele.isUndefinedOrNull()){
                isAllNull = false;
                break;
            }
        }
        if(isAllNull){
            return createByteArrayResult(nullptr, 0);
        }
        
        String string = JSONStringify(state, ret, 0);
        CString cstring = string.utf8();
        return createByteArrayResult(cstring.data(), cstring.length());
    });
	
	handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CREATEINSTANCE), [this](IPCArguments* arguments) {
        JSGlobalObject* impl_globalObject = m_impl->globalObject.get();
        const IPCString* ipcInstanceId = arguments->getString(0);
        const IPCString* ipcFunc = arguments->getString(1);
        const IPCString* ipcScript = arguments->getString(2);
        const IPCString* ipcOpts = arguments->getString(3);
        const IPCString* ipcInitData = arguments->getString(4);
        const IPCString* ipcExtendApi = arguments->getString(5);
        
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String func = jString2String(ipcFunc->content, ipcFunc->length);
        String script = jString2String(ipcScript->content, ipcScript->length);
        String opts = jString2String(ipcOpts->content, ipcOpts->length);
        String initData = jString2String(ipcInitData->content, ipcInitData->length);
        String extendsApi = jString2String(ipcExtendApi->content, ipcExtendApi->length);

        JSGlobalObject* globalObject;
        if (instanceID == "") {
            globalObject = impl_globalObject;
        } else {
            GlobalObject* temp_object  = m_impl->mJSGlobalObjectMap[instanceID.utf8().data()];

            if (temp_object == NULL) {
                // new a global object
                // --------------------------------------------------
                VM& vm =  *m_impl->globalVM.get();
                JSLockHolder locker(&vm);
                temp_object = GlobalObject::create(vm,
                    GlobalObject::createStructure(vm, jsNull()));
                temp_object->m_server = this;
                temp_object->initWXEnvironment(m_impl->mInitPrams);
                temp_object->initFunctionForContext();
                // --------------------------------------------------

                // use impl global object run createInstanceContext
                // --------------------------------------------------
                // RAx or vue
                JSGlobalObject* globalObject_local = impl_globalObject;
                PropertyName createInstanceContextProperty(Identifier::fromString(&vm, "createInstanceContext"));
                ExecState* state = globalObject_local->globalExec();
                JSValue createInstanceContextFunction = globalObject_local->get(state, createInstanceContextProperty);
                MarkedArgumentBuffer args;
                args.append(String2JSValue(state, instanceID));
                JSValue optsObject = parseToObject(state, opts);
                args.append(optsObject);
                JSValue initDataObject = parseToObject(state, initData);
                args.append(initDataObject);
                // args.append(String2JSValue(state, ""));
                CallData callData;
                CallType callType = getCallData(createInstanceContextFunction, callData);
                NakedPtr<Exception> returnedException;
                JSValue ret = call(state, createInstanceContextFunction, callType, callData,
                        globalObject_local, args, returnedException);
                if (returnedException) {
                    // ReportException(globalObject, returnedException.get(), nullptr, "");
                    String exceptionInfo = exceptionToString(globalObject_local, returnedException->value());
                    LOGE("getJSGlobalObject returnedException %s", exceptionInfo.utf8().data());
                }
                // --------------------------------------------------

                // String str = getArgumentAsString(state, ret); 
                //(ret.toWTFString(state));

                // use it to set Vue prototype to instance context 
                JSObject* object = ret.toObject(state, temp_object);
                JSObjectRef ref = toRef(object);
                JSGlobalContextRef contextRef = toGlobalRef(state);
                JSValueRef vueRef = JSObjectGetProperty(contextRef, ref, JSStringCreateWithUTF8CString("Vue"), nullptr);
                if (vueRef != nullptr) {
                    JSObjectRef vueObject = JSValueToObject(contextRef, vueRef, nullptr);
                    if (vueObject != nullptr) {
                        JSGlobalContextRef instanceContextRef = toGlobalRef(temp_object->globalExec());
                        JSObjectSetPrototype(instanceContextRef, vueObject,
                            JSObjectGetPrototype(instanceContextRef, JSContextGetGlobalObject(instanceContextRef)));
                    }
                }
                //-------------------------------------------------

                temp_object->resetPrototype(vm, ret);
                m_impl->mJSGlobalObjectMap[instanceID.utf8().data()] = temp_object;

                // -----------------------------------------
                // ExecState* exec =temp_object->globalExec();
                // JSLockHolder temp_locker(exec);
                // VM& temp_vm = exec->vm();
                // gcProtect(exec->vmEntryGlobalObject());
                // temp_vm.ref();
                // ------------------------------------------
            }
            globalObject = temp_object;
        }

        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);

        // if extend api is not null should exec befor createInstanceContext, such as rax-api
        if (!extendsApi.isEmpty() && extendsApi.length() > 0) {
            if (!ExecuteJavaScript(globalObject, extendsApi, ("weex run raxApi"), true, "runRaxApi")) {
                LOGE("before createInstanceContext run rax api Error");
                return createInt32Result(static_cast<int32_t>(false));
            }
        }
        if (!ExecuteJavaScript(globalObject, script, ("weex createInstanceContext"), true, "createInstanceContext")) {
            LOGE("createInstanceContext and ExecuteJavaScript Error");
            return createInt32Result(static_cast<int32_t>(false));
        }
        return createInt32Result(static_cast<int32_t>(true));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::DESTORYINSTANCE), [this](IPCArguments* arguments) {
        const IPCString* ipcInstanceId = arguments->getString(0);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        if (instanceID.isEmpty()) {
            LOGE("DestoryInstance instanceId is NULL");
            return createInt32Result(static_cast<int32_t>(false));
        }
        // LOGE("IPCJSMsg::DESTORYINSTANCE");
        GlobalObject* globalObject = m_impl->mJSGlobalObjectMap[instanceID.utf8().data()];
        if (globalObject == NULL) {
            return createInt32Result(static_cast<int32_t>(true));
        }
        // LOGE("DestoryInstance map 11 length:%d", m_impl->mJSGlobalObjectMap.size());
        m_impl->mJSGlobalObjectMap.erase(instanceID.utf8().data());
        // LOGE("DestoryInstance map 22 length:%d", m_impl->mJSGlobalObjectMap.size());
        
        // release JSGlobalContextRelease
        // when instanceID % 20 == 0 GC
        bool needGc = false;
        if (instanceID.length() > 0) {
            int index = atoi(instanceID.utf8().data());
            if(index > 0 && index % 20 == 0) {
                // LOGE("needGc is true, instanceID.utf8().data():%s index:%d", instanceID.utf8().data(), index);
                needGc = true;
            }
        }
        if (needGc) {
            ExecState* exec = globalObject->globalExec();
            JSLockHolder locker(exec);
            VM& vm = exec->vm();
            vm.heap.collectAllGarbage();
        }
        
        globalObject->m_server = nullptr;
        globalObject = NULL;
        
        return createInt32Result(static_cast<int32_t>(true));

    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSONINSTANCE), [this](IPCArguments* arguments) {
        const IPCString* ipcInstanceId = arguments->getString(0);
        const IPCString* ipcScript = arguments->getString(1);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String script = jString2String(ipcScript->content, ipcScript->length);
        JSGlobalObject* globalObject = m_impl->mJSGlobalObjectMap[instanceID.utf8().data()];
        if (globalObject == NULL) {
            globalObject = m_impl->globalObject.get();
        }
        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);
        SourceOrigin sourceOrigin(String::fromUTF8("(weex)"));
        NakedPtr<Exception> evaluationException;
        JSValue returnValue = evaluate(globalObject->globalExec(), makeSource(script, sourceOrigin, "execjs on instance context"), JSValue(), evaluationException);
        globalObject->vm().drainMicrotasks();
        if (evaluationException) {
            // String exceptionInfo = exceptionToString(globalObject, evaluationException.get()->value());
            // LOGE("EXECJSONINSTANCE exception:%s", exceptionInfo.utf8().data());
            ReportException(globalObject, evaluationException.get(), instanceID.utf8().data(), "execJSOnInstance");
            return createVoidResult();
        }
        // WTF::String str = returnValue.toWTFString(globalObject->globalExec());
        const char* data = returnValue.toWTFString(globalObject->globalExec()).utf8().data();
        char *buf = new char[strlen(data)+1];
        strcpy(buf, data);
        return createCharArrayResult(buf);
    });
    
    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::UPDATEGLOBALCONFIG), [this](IPCArguments* arguments) {
        JSGlobalObject* globalObject = m_impl->globalObject.get();
        VM& vm = globalObject->vm();
        JSLockHolder locker(&vm);
        const IPCString* ipcConfig = arguments->getString(0);
        String configString = jString2String(ipcConfig->content, ipcConfig->length);
        const char* config = configString.utf8().data();
        doUpdateGlobalSwitchConfig(config);
        return createVoidResult();
    }); 
}

WeexJSServer::~WeexJSServer()
{
}

void WeexJSServer::loop()
{
    m_impl->listener->listen();
}

IPCSender* WeexJSServer::getSender()
{
    return m_impl->sender.get();
}

IPCSerializer* WeexJSServer::getSerializer()
{
    return m_impl->serializer.get();
}

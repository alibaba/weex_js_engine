#include "Buffering/IPCBuffer.h"
#include "IPCArguments.h"
#include "IPCByteArray.h"
#include "IPCException.h"
#include "IPCHandler.h"
#include "IPCMessageJS.h"
#include "IPCResult.h"
#include "IPCSender.h"
#include "IPCString.h"
#include "LogUtils.h"
#include "Serializing/IPCSerializer.h"
#include "Trace.h"
#include "WeexJSConnection.h"
#include <jni.h>
#include <unistd.h>

namespace {

class ScopedJStringUTF8 {
public:
    ScopedJStringUTF8(JNIEnv* env, jstring);
    ~ScopedJStringUTF8();
    const char* getChars();

private:
    JNIEnv* m_env;
    jstring m_jstring;
    const char* m_chars;
};

class ScopedJString {
public:
    ScopedJString(JNIEnv* env, jstring);
    ~ScopedJString();
    const jchar* getChars();
    size_t getCharsLength();

private:
    JNIEnv* m_env;
    jstring m_jstring;
    const uint16_t* m_chars;
    size_t m_len;
};

ScopedJStringUTF8::ScopedJStringUTF8(JNIEnv* env, jstring _jstring)
    : m_env(env)
    , m_jstring(_jstring)
    , m_chars(nullptr)
{
}

ScopedJStringUTF8::~ScopedJStringUTF8()
{
    if (m_chars)
        m_env->ReleaseStringUTFChars(m_jstring, m_chars);
}

const char* ScopedJStringUTF8::getChars()
{
    if (m_chars)
        return m_chars;
    m_chars = m_env->GetStringUTFChars(m_jstring, nullptr);
    return m_chars;
}

ScopedJString::ScopedJString(JNIEnv* env, jstring _jstring)
    : m_env(env)
    , m_jstring(_jstring)
    , m_chars(nullptr)
    , m_len(0)
{
}

ScopedJString::~ScopedJString()
{
    if (m_chars)
        m_env->ReleaseStringChars(m_jstring, m_chars);
}

const jchar*
ScopedJString::getChars()
{
    if (m_chars)
        return m_chars;
    m_chars = m_env->GetStringChars(m_jstring, nullptr);
    m_len = m_env->GetStringLength(m_jstring);
    return m_chars;
}

size_t
ScopedJString::getCharsLength()
{
    if (m_chars)
        return m_len;
    m_len = m_env->GetStringLength(m_jstring);
    return m_len;
}
}

static JNIEnv* getJNIEnv();

static jclass jBridgeClazz;
static jclass jWXJSObject;
static jclass jWXLogUtils;
static jmethodID jCallAddElementMethodId;
static jmethodID jDoubleValueMethodId;
static jmethodID jSetTimeoutNativeMethodId;
static jmethodID jCallNativeMethodId;
static jmethodID jCallNativeModuleMethodId;
static jmethodID jCallNativeComponentMethodId;
static jmethodID jLogMethodId;
static jobject jThis;
static jobject jScript;
static jobject jParams;
static JavaVM* sVm = NULL;
static IPCSender* sSender;
static std::unique_ptr<IPCHandler> sHandler;
static std::unique_ptr<WeexJSConnection> sConnection;

JNIEnv* getJNIEnv()
{
    JNIEnv* env = NULL;
    if ((sVm)->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_FALSE;
    }
    return env;
}

static void reportException(const char* instanceID, const char* func, const char* exception_string)
{
    JNIEnv* env = getJNIEnv();
    jstring jExceptionString = env->NewStringUTF(exception_string);
    jstring jInstanceId = env->NewStringUTF(instanceID);
    jstring jFunc = env->NewStringUTF(func);
    jmethodID tempMethodId = env->GetMethodID(jBridgeClazz,
        "reportJSException",
        "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V");
    env->CallVoidMethod(jThis, tempMethodId, jInstanceId, jFunc, jExceptionString);
    env->DeleteLocalRef(jExceptionString);
    env->DeleteLocalRef(jInstanceId);
    env->DeleteLocalRef(jFunc);
}

static jstring getArgumentAsJString(JNIEnv* env, IPCArguments* arguments, int argument)
{
    jstring ret = nullptr;
    if (arguments->getType(argument) == IPCType::STRING) {
        const IPCString* s = arguments->getString(argument);
        ret = env->NewString(s->content, s->length);
    }
    return ret;
}

static jbyteArray getArgumentAsJByteArray(JNIEnv* env, IPCArguments* arguments, size_t argument)
{
    jbyteArray ba = nullptr;
    if (argument >= arguments->getCount())
        return nullptr;
    if (arguments->getType(argument) == IPCType::BYTEARRAY) {
        const IPCByteArray* ipcBA = arguments->getByteArray(argument);
        int strLen = ipcBA->length;
        ba = env->NewByteArray(strLen);
        env->SetByteArrayRegion(ba, 0, strLen,
            reinterpret_cast<const jbyte*>(ipcBA->content));
    }
    return ba;
}

static std::unique_ptr<IPCResult> handleSetJSVersion(IPCArguments* arguments)
{
    JNIEnv* env = getJNIEnv();
    jmethodID tempMethodId = env->GetMethodID(jBridgeClazz,
        "setJSFrmVersion",
        "(Ljava/lang/String;)V");
    const IPCByteArray* ba = arguments->getByteArray(0);
    LOGA("init JSFrm version %s", ba->content);
    jstring jversion = env->NewStringUTF(ba->content);
    env->CallVoidMethod(jThis, tempMethodId, jversion);
    env->DeleteLocalRef(jversion);
    return createVoidResult();
}

static std::unique_ptr<IPCResult> handleReportException(IPCArguments* arguments)
{
    const char* instanceID = nullptr;
    const char* func = nullptr;
    const char* exceptionInfo = nullptr;
    if (arguments->getType(0) == IPCType::BYTEARRAY) {
        const IPCByteArray* instanceIDBA = arguments->getByteArray(0);
        instanceID = instanceIDBA->content;
    }
    if (arguments->getType(1) == IPCType::BYTEARRAY) {
        const IPCByteArray* funcBA = arguments->getByteArray(1);
        func = funcBA->content;
    }
    if (arguments->getType(2) == IPCType::BYTEARRAY) {
        const IPCByteArray* exceptionInfoBA = arguments->getByteArray(2);
        exceptionInfo = exceptionInfoBA->content;
    }
    LOGE(" ReportException : %s", exceptionInfo);
    reportException(instanceID, func, exceptionInfo);
    return createVoidResult();
}

static std::unique_ptr<IPCResult> handleCallNative(IPCArguments* arguments)
{
    base::debug::TraceScope traceScope("weex", "callNative");

    JNIEnv* env = getJNIEnv();
    //instacneID args[0]
    jstring jInstanceId = getArgumentAsJString(env, arguments, 0);
    //task args[1]
    jbyteArray jTaskString = getArgumentAsJByteArray(env, arguments, 1);
    //callback args[2]
    jstring jCallback = getArgumentAsJString(env, arguments, 2);

    if (jCallNativeMethodId == NULL) {
        jCallNativeMethodId = env->GetMethodID(jBridgeClazz,
            "callNative",
            "(Ljava/lang/String;[BLjava/lang/String;)I");
    }

    int flag = env->CallIntMethod(jThis, jCallNativeMethodId, jInstanceId, jTaskString, jCallback);
    if (flag == -1) {
        LOGE("instance destroy JFM must stop callNative");
    }
    env->DeleteLocalRef(jTaskString);
    env->DeleteLocalRef(jInstanceId);
    env->DeleteLocalRef(jCallback);

    return createInt32Result(flag);
}

static std::unique_ptr<IPCResult> handleCallNativeModule(IPCArguments* arguments)
{
    base::debug::TraceScope traceScope("weex", "callNativeModule");

    JNIEnv* env = getJNIEnv();
    //instacneID args[0]
    jstring jInstanceId = getArgumentAsJString(env, arguments, 0);

    //module args[1]
    jstring jmodule = getArgumentAsJString(env, arguments, 1);

    //method args[2]
    jstring jmethod = getArgumentAsJString(env, arguments, 2);

    // arguments args[3]
    jbyteArray jArgString = getArgumentAsJByteArray(env, arguments, 3);
    //arguments args[4]
    jbyteArray jOptString = getArgumentAsJByteArray(env, arguments, 4);

    if (jCallNativeModuleMethodId == NULL) {
        jCallNativeModuleMethodId = env->GetMethodID(jBridgeClazz,
            "callNativeModule",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B[B)Ljava/lang/Object;");
    }

    jobject result = env->CallObjectMethod(jThis, jCallNativeModuleMethodId, jInstanceId, jmodule, jmethod, jArgString, jOptString);
    std::unique_ptr<IPCResult> ret;

    jfieldID jTypeId = env->GetFieldID(jWXJSObject, "type", "I");
    jint jTypeInt = env->GetIntField(result, jTypeId);
    jfieldID jDataId = env->GetFieldID(jWXJSObject, "data", "Ljava/lang/Object;");
    jobject jDataObj = env->GetObjectField(result, jDataId);
    if (jTypeInt == 1) {
        if (jDoubleValueMethodId == NULL) {
            jclass jDoubleClazz = env->FindClass("java/lang/Double");
            jDoubleValueMethodId = env->GetMethodID(jDoubleClazz, "doubleValue", "()D");
            env->DeleteLocalRef(jDoubleClazz);
        }
        jdouble jDoubleObj = env->CallDoubleMethod(jDataObj, jDoubleValueMethodId);
        ret = std::move(createDoubleResult(jDoubleObj));

    } else if (jTypeInt == 2) {
        jstring jDataStr = (jstring)jDataObj;
        ret = std::move(createStringResult(env, jDataStr));
    } else if (jTypeInt == 3) {
        jstring jDataStr = (jstring)jDataObj;
        ret = std::move(createJSONStringResult(env, jDataStr));
    }
    env->DeleteLocalRef(jDataObj);
    env->DeleteLocalRef(jInstanceId);
    env->DeleteLocalRef(jmodule);
    env->DeleteLocalRef(jmethod);
    env->DeleteLocalRef(jArgString);
    env->DeleteLocalRef(jOptString);
    return ret;
}

static std::unique_ptr<IPCResult> handleCallNativeComponent(IPCArguments* arguments)
{
    base::debug::TraceScope traceScope("weex", "callNativeComponent");
    JNIEnv* env = getJNIEnv();

    //instacneID args[0]
    jstring jInstanceId = getArgumentAsJString(env, arguments, 0);

    //module args[1]
    jstring jcomponentRef = getArgumentAsJString(env, arguments, 1);

    //method args[2]
    jstring jmethod = getArgumentAsJString(env, arguments, 2);

    // arguments args[3]
    jbyteArray jArgString = getArgumentAsJByteArray(env, arguments, 3);

    //arguments args[4]
    jbyteArray jOptString = getArgumentAsJByteArray(env, arguments, 4);

    if (jCallNativeComponentMethodId == NULL) {
        jCallNativeComponentMethodId = env->GetMethodID(jBridgeClazz,
            "callNativeComponent",
            "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;[B[B)V");
    }

    env->CallVoidMethod(jThis, jCallNativeComponentMethodId, jInstanceId, jcomponentRef, jmethod, jArgString, jOptString);

    env->DeleteLocalRef(jInstanceId);
    env->DeleteLocalRef(jcomponentRef);
    env->DeleteLocalRef(jmethod);
    env->DeleteLocalRef(jArgString);
    env->DeleteLocalRef(jOptString);
    return createInt32Result(static_cast<int32_t>(true));
}

static std::unique_ptr<IPCResult> handleCallAddElement(IPCArguments* arguments)
{
    base::debug::TraceScope traceScope("weex", "callAddElement");
    JNIEnv* env = getJNIEnv();
    //instacneID args[0]
    jstring jInstanceId = getArgumentAsJString(env, arguments, 0);
    //instacneID args[1]
    jstring jref = getArgumentAsJString(env, arguments, 1);
    //dom node args[2]
    jbyteArray jdomString = getArgumentAsJByteArray(env, arguments, 2);
    //index  args[3]
    jstring jindex = getArgumentAsJString(env, arguments, 3);
    //callback  args[4]
    jstring jCallback = getArgumentAsJString(env, arguments, 4);
    if (jCallAddElementMethodId == NULL) {
        jCallAddElementMethodId = env->GetMethodID(jBridgeClazz,
            "callAddElement",
            "(Ljava/lang/String;Ljava/lang/String;[BLjava/lang/String;Ljava/lang/String;)I");
    }

    int flag = env->CallIntMethod(jThis, jCallAddElementMethodId, jInstanceId, jref, jdomString, jindex,
        jCallback);
    if (flag == -1) {
        LOGE("instance destroy JFM must stop callNative");
    }
    env->DeleteLocalRef(jInstanceId);
    env->DeleteLocalRef(jref);
    env->DeleteLocalRef(jdomString);
    env->DeleteLocalRef(jindex);
    env->DeleteLocalRef(jCallback);
    return createInt32Result(flag);
}

static std::unique_ptr<IPCResult> handleSetTimeout(IPCArguments* arguments)
{
    base::debug::TraceScope traceScope("weex", "setTimeoutNative");
    JNIEnv* env = getJNIEnv();
    //callbackId
    jstring jCallbackID = getArgumentAsJString(env, arguments, 0);

    //time
    jstring jTime = getArgumentAsJString(env, arguments, 1);

    if (jSetTimeoutNativeMethodId == NULL) {
        jSetTimeoutNativeMethodId = env->GetMethodID(jBridgeClazz,
            "setTimeoutNative",
            "(Ljava/lang/String;Ljava/lang/String;)V");
    }
    env->CallVoidMethod(jThis, jSetTimeoutNativeMethodId, jCallbackID, jTime);
    env->DeleteLocalRef(jCallbackID);
    env->DeleteLocalRef(jTime);
    return createInt32Result(static_cast<int32_t>(true));
}

static std::unique_ptr<IPCResult> handleCallNativeLog(IPCArguments* arguments)
{
    JNIEnv* env = getJNIEnv();
    bool result = false;
    jstring str_msg = getArgumentAsJString(env, arguments, 0);
    env = getJNIEnv();
    if (jWXLogUtils != NULL) {
        if (jLogMethodId == NULL) {
            jLogMethodId = env->GetStaticMethodID(jWXLogUtils, "d", "(Ljava/lang/String;Ljava/lang/String;)V");
        }
        if (jLogMethodId != NULL) {
            jstring str_tag = env->NewStringUTF("jsLog");
            // str_msg = env->NewStringUTF(s);
            env->CallStaticVoidMethod(jWXLogUtils, jLogMethodId, str_tag, str_msg);
            result = true;
            env->DeleteLocalRef(str_msg);
            env->DeleteLocalRef(str_tag);
        }
    }
    return createInt32Result(static_cast<int32_t>(true));
}

static void initHandler(IPCHandler* handler)
{
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::SETJSFVERSION), handleSetJSVersion);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::REPORTEXCEPTION), handleReportException);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVE), handleCallNative);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVEMODULE), handleCallNativeModule);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLNATIVECOMPONENT), handleCallNativeComponent);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::CALLADDELEMENT), handleCallAddElement);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::SETTIMEOUT), handleSetTimeout);
    handler->registerHandler(static_cast<uint32_t>(IPCProxyMsg::NATIVELOG), handleCallNativeLog);
}

static void addString(JNIEnv* env, IPCSerializer* serializer, jstring str)
{
    ScopedJString scopedString(env, str);
    const uint16_t* chars = scopedString.getChars();
    size_t charsLength = scopedString.getCharsLength();
    serializer->add(chars, charsLength);
}

static void addJSONString(JNIEnv* env, IPCSerializer* serializer, jstring str)
{
    ScopedJString scopedString(env, str);
    const uint16_t* chars = scopedString.getChars();
    size_t charsLength = scopedString.getCharsLength();
    serializer->addJSON(chars, charsLength);
}

static jint native_execJSService(JNIEnv* env,
    jobject object,
    jstring script)
{
    if (script == nullptr)
        return false;
    try {
        base::debug::TraceScope traceScope("weex", "exeJSService");
        std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
        serializer->setMsg(static_cast<uint32_t>(IPCJSMsg::EXECJSSERVICE));
        addString(env, serializer.get(), script);
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sSender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("execJSService Unexpected result type");
            return false;
        }
        return result->get<jint>();
    } catch (IPCException& e) {
        LOGE("%s", e.msg());
        return false;
    }
    return true;
}

static void native_takeHeapSnapshot(JNIEnv* env,
    jobject object,
    jstring name)
{
}

static void initFromParam(JNIEnv* env, jstring script, jobject params, IPCSerializer* serializer)
{
    {
        ScopedJString scopedString(env, script);
        const jchar* chars = scopedString.getChars();
        int charLength = scopedString.getCharsLength();
        serializer->add(chars, charLength);
    }
#define ADDSTRING(name)                                     \
    {                                                       \
        const char* myname = #name;                         \
        serializer->add(myname, strlen(myname));            \
        ScopedJStringUTF8 scopedString(env, (jstring)name); \
        const char* chars = scopedString.getChars();        \
        int charLength = strlen(chars);                     \
        serializer->add(chars, charLength);                 \
    }
    jclass c_params = env->GetObjectClass(params);

    jmethodID m_platform = env->GetMethodID(c_params, "getPlatform", "()Ljava/lang/String;");
    jobject platform = env->CallObjectMethod(params, m_platform);
    ADDSTRING(platform);

    jmethodID m_osVersion = env->GetMethodID(c_params, "getOsVersion", "()Ljava/lang/String;");
    jobject osVersion = env->CallObjectMethod(params, m_osVersion);
    ADDSTRING(osVersion);

    jmethodID m_appVersion = env->GetMethodID(c_params, "getAppVersion", "()Ljava/lang/String;");
    jobject appVersion = env->CallObjectMethod(params, m_appVersion);
    ADDSTRING(appVersion);

    jmethodID m_weexVersion = env->GetMethodID(c_params, "getWeexVersion", "()Ljava/lang/String;");
    jobject weexVersion = env->CallObjectMethod(params, m_weexVersion);
    ADDSTRING(weexVersion);

    jmethodID m_deviceModel = env->GetMethodID(c_params, "getDeviceModel", "()Ljava/lang/String;");
    jobject deviceModel = env->CallObjectMethod(params, m_deviceModel);
    ADDSTRING(deviceModel);

    jmethodID m_appName = env->GetMethodID(c_params, "getAppName", "()Ljava/lang/String;");
    jobject appName = env->CallObjectMethod(params, m_appName);
    ADDSTRING(appName);

    jmethodID m_deviceWidth = env->GetMethodID(c_params, "getDeviceWidth", "()Ljava/lang/String;");
    jobject deviceWidth = env->CallObjectMethod(params, m_deviceWidth);
    ADDSTRING(deviceWidth);

    jmethodID m_deviceHeight = env->GetMethodID(c_params, "getDeviceHeight",
        "()Ljava/lang/String;");
    jobject deviceHeight = env->CallObjectMethod(params, m_deviceHeight);
    ADDSTRING(deviceHeight);

    jmethodID m_options = env->GetMethodID(c_params, "getOptions", "()Ljava/lang/Object;");
    jobject options = env->CallObjectMethod(params, m_options);
    jclass jmapclass = env->FindClass("java/util/HashMap");
    jmethodID jkeysetmid = env->GetMethodID(jmapclass, "keySet", "()Ljava/util/Set;");
    jmethodID jgetmid = env->GetMethodID(jmapclass, "get", "(Ljava/lang/Object;)Ljava/lang/Object;");
    jobject jsetkey = env->CallObjectMethod(options, jkeysetmid);
    jclass jsetclass = env->FindClass("java/util/Set");
    jmethodID jtoArraymid = env->GetMethodID(jsetclass, "toArray", "()[Ljava/lang/Object;");
    jobjectArray jobjArray = (jobjectArray)env->CallObjectMethod(jsetkey, jtoArraymid);
    env->DeleteLocalRef(jsetkey);
    if (jobjArray != NULL) {
        jsize arraysize = env->GetArrayLength(jobjArray);
        for (int i = 0; i < arraysize; i++) {
            jstring jkey = (jstring)env->GetObjectArrayElement(jobjArray, i);
            jstring jvalue = (jstring)env->CallObjectMethod(options, jgetmid, jkey);
            if (jkey != NULL) {
                // const char* c_key = env->GetStringUTFChars(jkey, NULL);
                // addString(vm, WXEnvironment, c_key, jString2String(env, jvalue));
                // serializer->add(c_key, strlen(c_key));
                // env->DeleteLocalRef(jkey);
                // if (jvalue != NULL) {
                //     env->DeleteLocalRef(jvalue);
                // }
                ScopedJStringUTF8 c_key(env, jkey);
                ScopedJStringUTF8 c_value(env, jvalue);
                const char* c_key_chars = c_key.getChars();
                int c_key_len = strlen(c_key_chars);
                const char* c_value_chars = c_value.getChars();
                int c_value_len = strlen(c_value_chars);
                serializer->add(c_key_chars, c_key_len);
                serializer->add(c_value_chars, c_value_len);
            }
        }
        env->DeleteLocalRef(jobjArray);
    }
    env->DeleteLocalRef(options);
}

static jint doInitFramework(JNIEnv* env,
    jobject object,
    jstring script,
    jobject params)
{
    try {
        base::debug::TraceScope traceScope("weex", "initFramework");
        sHandler = std::move(createIPCHandler());
        sConnection.reset(new WeexJSConnection());
        sSender = sConnection->start(sHandler.get());
        initHandler(sHandler.get());
        using base::debug::TraceEvent;
        TraceEvent::StartATrace(env);
        std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
        serializer->setMsg(static_cast<uint32_t>(IPCJSMsg::INITFRAMEWORK));
        initFromParam(env, script, params, serializer.get());
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sSender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("initFramework Unexpected result type");
            return false;
        }
        return result->get<jint>();
    } catch (IPCException& e) {
        LOGE("%s", e.msg());
        return false;
    }
    return true;
}

static jint native_initFramework(JNIEnv* env,
    jobject object,
    jstring script,
    jobject params)
{
    jThis = env->NewGlobalRef(object);
    jScript = env->NewGlobalRef(script);
    jParams = env->NewGlobalRef(params);
    return doInitFramework(env, jThis, static_cast<jstring>(jScript), jParams);
}

/**
 * Called to execute JavaScript such as . createInstance(),destroyInstance ext.
 *
 */
static jint native_execJS(JNIEnv* env,
    jobject jthis,
    jstring jinstanceid,
    jstring jnamespace,
    jstring jfunction,
    jobjectArray jargs)
{
    if (jfunction == NULL || jinstanceid == NULL) {
        LOGE("native_execJS function is NULL");
        return false;
    }

    int length = 0;
    if (jargs != NULL) {
        length = env->GetArrayLength(jargs);
    }
    try {
        base::debug::TraceScope traceScope("weex", "execJS");
        std::unique_ptr<IPCSerializer> serializer(createIPCSerializer());
        serializer->setMsg(static_cast<uint32_t>(IPCJSMsg::EXECJS));
        addString(env, serializer.get(), jinstanceid);
        if (jnamespace)
            addString(env, serializer.get(), jnamespace);
        else {
            uint16_t tmp = 0;
            serializer->add(&tmp, 0);
        }
        addString(env, serializer.get(), jfunction);

        for (int i = 0; i < length; i++) {
            jobject jArg = env->GetObjectArrayElement(jargs, i);

            jfieldID jTypeId = env->GetFieldID(jWXJSObject, "type", "I");
            jint jTypeInt = env->GetIntField(jArg, jTypeId);

            jfieldID jDataId = env->GetFieldID(jWXJSObject, "data", "Ljava/lang/Object;");
            jobject jDataObj = env->GetObjectField(jArg, jDataId);
            if (jTypeInt == 1) {
                if (jDoubleValueMethodId == NULL) {
                    jclass jDoubleClazz = env->FindClass("java/lang/Double");
                    jDoubleValueMethodId = env->GetMethodID(jDoubleClazz, "doubleValue", "()D");
                    env->DeleteLocalRef(jDoubleClazz);
                }
                jdouble jDoubleObj = env->CallDoubleMethod(jDataObj, jDoubleValueMethodId);
                serializer->add(jDoubleObj);

            } else if (jTypeInt == 2) {
                jstring jDataStr = (jstring)jDataObj;
                addString(env, serializer.get(), jDataStr);
            } else if (jTypeInt == 3) {
                jstring jDataStr = (jstring)jDataObj;
                addJSONString(env, serializer.get(), jDataStr);
            } else {
                serializer->addJSUndefined();
            }
            env->DeleteLocalRef(jDataObj);
            env->DeleteLocalRef(jArg);
        }
        std::unique_ptr<IPCBuffer> buffer = serializer->finish();
        std::unique_ptr<IPCResult> result = sSender->send(buffer.get());
        if (result->getType() != IPCType::INT32) {
            LOGE("execJS Unexpected result type");
            return false;
        }
        return result->get<jint>();
    } catch (IPCException& e) {
        LOGE("%s", e.msg());
        doInitFramework(env, jThis, static_cast<jstring>(jScript), jParams);
        return false;
    }
    return true;
}

static const char* gBridgeClassPathName = "com/taobao/weex/bridge/WXBridge";
static JNINativeMethod gMethods[] = {
    { "initFramework",
        "(Ljava/lang/String;Lcom/taobao/weex/bridge/WXParams;)I",
        (void*)native_initFramework },
    { "execJS",
        "(Ljava/lang/String;Ljava/lang/String;"
        "Ljava/lang/String;[Lcom/taobao/weex/bridge/WXJSObject;)I",
        (void*)native_execJS },
    { "takeHeapSnapshot",
        "(Ljava/lang/String;)V",
        (void*)native_takeHeapSnapshot },
    { "execJSService",
        "(Ljava/lang/String;)I",
        (void*)native_execJSService }
};

static int
registerBridgeNativeMethods(JNIEnv* env, JNINativeMethod* methods, int numMethods)
{
    if (jBridgeClazz == NULL) {
        LOGE("registerBridgeNativeMethods failed to find bridge class.");
        return JNI_FALSE;
    }
    if ((env)->RegisterNatives(jBridgeClazz, methods, numMethods) < 0) {
        LOGE("registerBridgeNativeMethods failed to register native methods for bridge class.");
        return JNI_FALSE;
    }

    return JNI_TRUE;
}

static bool registerNatives(JNIEnv* env)
{
    if (JNI_TRUE != registerBridgeNativeMethods(env, gMethods, sizeof(gMethods) / sizeof(gMethods[0])))
        return false;
    return true;
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    LOGD("begin JNI_OnLoad");
    JNIEnv* env;
    /* Get environment */
    if ((vm)->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return JNI_FALSE;
    }
    // FIXME: Move to other place
    // if (!initICU()) {
    //     return JNI_FALSE;
    // }

    sVm = vm;
    jclass tempClass = env->FindClass(
        "com/taobao/weex/bridge/WXBridge");
    jBridgeClazz = (jclass)env->NewGlobalRef(tempClass);

    tempClass = env->FindClass("com/taobao/weex/bridge/WXJSObject");
    jWXJSObject = (jclass)env->NewGlobalRef(tempClass);

    tempClass = env->FindClass("com/taobao/weex/utils/WXLogUtils");
    jWXLogUtils = (jclass)env->NewGlobalRef(tempClass);

    env->DeleteLocalRef(tempClass);
    if (!registerNatives(env)) {
        return JNI_FALSE;
    }

    LOGD("end JNI_OnLoad");
    return JNI_VERSION_1_4;
}

void JNI_OnUnload(JavaVM* vm, void* reserved)
{
    LOGD("beigin JNI_OnUnload");
    JNIEnv* env;

    /* Get environment */
    if ((vm)->GetEnv((void**)&env, JNI_VERSION_1_4) != JNI_OK) {
        return;
    }
    env->DeleteGlobalRef(jBridgeClazz);
    env->DeleteGlobalRef(jWXJSObject);
    env->DeleteGlobalRef(jWXLogUtils);
    if (jThis)
        env->DeleteGlobalRef(jThis);
    if (jScript)
        env->DeleteGlobalRef(jScript);
    if (jParams)
        env->DeleteGlobalRef(jParams);
    // FIXME: move to other place
    // deinitHeapTimer();

    sConnection.reset();
    sHandler.reset();
    using base::debug::TraceEvent;
    TraceEvent::StopATrace(env);
    LOGD(" end JNI_OnUnload");
}

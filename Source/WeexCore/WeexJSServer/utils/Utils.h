//
// Created by Darin on 11/02/2018.
//

#ifndef WEEXV8_UTILS_H
#define WEEXV8_UTILS_H

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
#include "CallFrame.h"
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

#include "IPC/Buffering/IPCBuffer.h"
#include "CrashHandler.h"
#include "IPC/IPCArguments.h"
#include "IPC/IPCByteArray.h"
#include "IPC/IPCHandler.h"
#include "IPC/IPCListener.h"
#include "IPC/IPCMessageJS.h"
#include "IPC/IPCResult.h"
#include "IPC/IPCSender.h"
#include "IPC/IPCString.h"
#include "IPC/IPCType.h"
#include "IPC/IPCFutexPageQueue.h"
#include "IPC/IPCException.h"
#include "LogUtils.h"
#include "IPC/Serializing/IPCSerializer.h"
#include "Trace.h"
#include "./base/base64/base64.h"
#include "Base64.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <dlfcn.h>
#include <cstring>
#include "include/WeexApiHeader.h"

#include "WeexCore/WeexJSServer/object/SimpleObject.h"
#include "WeexCore/WeexJSServer/object/Args.h"
#include "WeexCore/WeexJSServer/wson/wsonjsc.h"

using namespace JSC;
using namespace WeexCore;

#define TIMESPCE 1000
#define MICROSEC  ((uint64_t) 1e6)
extern bool config_use_wson;

String jString2String(const uint16_t *str, size_t length);


String weexString2String(const WeexString *weexString);

String char2String(const char *string);

void addString(IPCSerializer *serializer, const String &s);

void getArgumentAsJString(IPCSerializer *serializer, ExecState *state, int argument);

WeexByteArray *getArgumentAsWeexByteArrayJSON(ExecState *state, int argument);
WeexString *genWeexStringSS(const uint16_t *str, size_t length);
WeexByteArray *genWeexByteArraySS(const char *str, size_t strLen);

JSValue jString2JSValue(ExecState *state, const uint16_t *str, size_t length);

JSValue String2JSValue(ExecState *state, String s);

JSValue parseToObject(ExecState *state, const String &data);

void getArgumentAsJByteArrayJSON(IPCSerializer *serializer, ExecState *state, int argument);

void getArgumentAsJByteArray(IPCSerializer *serializer, ExecState *state, int argument);
void freeInitFrameworkParams(std::vector<INIT_FRAMEWORK_PARAMS *> &params);
void freeParams(std::vector<VALUE_WITH_TYPE *> &params);
std::unique_ptr<char[]> getCharJSONStringFromState(ExecState *state, int argument);

std::unique_ptr<char[]> getCharOrJSONStringFromState(ExecState *state, int argument);

std::unique_ptr<char[]> getCharStringFromState(ExecState *state, int argument);

/** auto choose use wson or json */
void getWsonOrJsonArgsFromState(ExecState *state, int argument, Args& args);

/** force string */
void getStringArgsFromState(ExecState *state, int argument, Args& args);

/**
 * get wson args with wson parser
 */
void getWsonArgsFromState(ExecState *state, int argument, Args& args);

/**
 * get json args with JSONStringify
 */
void getJSONArgsFromState(ExecState *state, int argument, Args& args);

/**
 * add to ipc argument
 * 
 */
void addObjectArgsToIPC(IPCSerializer *serializer, Args& args);
void addStringArgsToIPC(IPCSerializer *serializer, Args& args);


std::unique_ptr<char[]> newCharString(const char *str, size_t length);
void printLogOnFileWithNameS(const char * name,const char *log);
void getArgumentAsCString(IPCSerializer *serializer, ExecState *state, int argument);


void initCrashHandler(const char *path);

void initHeapTimer();

void deinitHeapTimer();

void markupStateInternally();

void doUpdateGlobalSwitchConfig(const char *config);

class WeexGlobalObject;

void setJSFVersion(WeexGlobalObject *globalObject);

String exceptionToString(JSGlobalObject *globalObject, JSValue exception);

void ReportException(JSGlobalObject *globalObject,
                     Exception *exception,
                     const char *jinstanceid,
                     const char *func);

bool ExecuteJavaScript(JSGlobalObject *globalObject,
                       const String &source,
                       const String &url,
                       bool report_exceptions,
                       const char *func,
                       const char *instanceId = "");

uint64_t microTime();
int __atomic_inc(volatile int *ptr);
int genTaskId() ;
WeexByteArray *IPCByteArrayToWeexByteArray(const IPCByteArray *byteArray);


namespace WEEXICU {

    class unique_fd {
    public:
        explicit unique_fd(int fd);

        ~unique_fd();

        int get() const;

    private:
        int m_fd;
    };

    static std::string __attribute__((noinline)) findPath();

    static void findPath(std::string &executablePath, std::string &icuDataPath) {
        unsigned long target = reinterpret_cast<unsigned long>(__builtin_return_address(0));
        FILE *f = fopen("/proc/self/maps", "r");
        if (!f) {
            return;
        }
        char buffer[256];
        char *line;
        while ((line = fgets(buffer, 256, f))) {
            if (icuDataPath.empty() && strstr(line, "icudt")) {
                icuDataPath.assign(strstr(line, "/"));
                icuDataPath = icuDataPath.substr(0, icuDataPath.length() - 1);
                continue;
            }
            char *end;
            unsigned long val;
            errno = 0;
            val = strtoul(line, &end, 16);
            if (errno)
                continue;
            if (val > target)
                continue;
            end += 1;
            errno = 0;
            val = strtoul(end, &end, 16);
            if (errno)
                continue;
            if (val > target) {
                char *s = strstr(end, "/");
                if (s != nullptr)
                    executablePath.assign(s);
                std::size_t found = executablePath.rfind('/');
                if (found != std::string::npos) {
                    executablePath = executablePath.substr(0, found);
                }
            }
            if (!executablePath.empty()
                && !icuDataPath.empty()) {
                break;
            }
        }
        fclose(f);
        return;
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

    static bool mapIcuData(const std::string &path) {
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
        void *data = mmap(NULL, sb.st_size, PROT_READ, MAP_SHARED, fd.get(), 0);
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

    static bool initICUEnv(bool multiProcess) {
        static bool isInit = false;
        if (isInit)
            return true;

        char *path;
        if (!multiProcess) {
            std::string executablePath;
            std::string icuDataPath;
            findPath(executablePath, icuDataPath);
            path = new char[icuDataPath.length() + 1];
            std::strcpy(path, icuDataPath.c_str());
        } else {
            path = getenv("ICU_DATA_PATH");
        }
        LOGE("initICUEnv patch:%s", path);

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
        if (strlen(path) > 0) {
            isInit = true;
            return mapIcuData(std::string(path));
        }
        return false;
    }

}

#endif //WEEXV8_UTILS_H

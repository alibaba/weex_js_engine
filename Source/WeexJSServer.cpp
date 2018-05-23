#include <WeexCore/WeexJSServer/utils/WeexRuntime.h>

using namespace JSC;
using namespace WTF;
using namespace WEEXICU;

extern bool config_use_wson;

struct WeexJSServer::WeexJSServerImpl {
    WeexJSServerImpl(int fd, bool enableTrace);

    bool enableTrace;
    std::unique_ptr<WeexRuntime> weexRuntime;
    std::unique_ptr<IPCFutexPageQueue> futexPageQueue;
    std::unique_ptr<IPCSender> sender;
    std::unique_ptr<IPCHandler> handler;
    std::unique_ptr<IPCListener> listener;
    std::unique_ptr<IPCSerializer> serializer;

    std::map<std::string, std::string> mInitAppContextPrams;
};

WeexJSServer::WeexJSServerImpl::WeexJSServerImpl(int _fd, bool _enableTrace)
        : enableTrace(_enableTrace) {
    void *base = mmap(nullptr, IPCFutexPageQueue::ipc_size, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, 0);
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
    weexRuntime.reset(new WeexRuntime(true));
}

WeexJSServer::WeexJSServer(int fd, bool enableTrace)
        : m_impl(new WeexJSServerImpl(fd, enableTrace)) {
    IPCHandler *handler = m_impl->handler.get();
    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::INITFRAMEWORK), [this](IPCArguments *arguments) {
        m_impl->weexRuntime->setWeexJSServer(this);
        return createInt32Result(m_impl->weexRuntime->initFramework(arguments));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::INITAPPFRAMEWORK), [this](IPCArguments *arguments) {
        const IPCString *ipcInstanceId = arguments->getString(0);
        String id = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        const IPCString *appFrameWork = arguments->getString(1);
        String js = jString2String(appFrameWork->content, appFrameWork->length);
        LOGE("Weex jsserver IPCJSMsg::INITAPPFRAMEWORK id:%s", id.utf8().data());
        if (id.isEmpty() || id == "") {
            return createInt32Result(static_cast<int32_t>(false));
        }

        return createInt32Result(m_impl->weexRuntime->initAppFrameworkMultiProcess(id, js, arguments));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CREATEAPPCONTEXT), [this](IPCArguments *arguments) {
        // LOGE("Weex jsserver IPCJSMsg::CREATEAPPCONTEXT start");
        const IPCString *ipcInstanceId = arguments->getString(0);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        const IPCString *jsBundle = arguments->getString(1);
        String js = jString2String(jsBundle->content, jsBundle->length);
        // LOGE("Weex jsserver IPCJSMsg::CREATEAPPCONTEXT end");
        return createInt32Result(m_impl->weexRuntime->createAppContext(instanceID, js));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSONAPPWITHRESULT), [this](IPCArguments *arguments) {
        // LOGE("Weex jsserver IPCJSMsg::EXECJSONAPPWITHRESULT start");
        const IPCString *ipcInstanceId = arguments->getString(0);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        const IPCString *jsBundle = arguments->getString(1);
        String js = jString2String(jsBundle->content, jsBundle->length);
        char *result = m_impl->weexRuntime->exeJSOnAppWithResult(instanceID, js);

        if (result == nullptr)
            return createByteArrayResult(nullptr, 0);
        else return createCharArrayResult(result);
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CALLJSONAPPCONTEXT), [this](IPCArguments *arguments) {
        // LOGE("Weex jsserver IPCJSMsg::CALLJSONAPPCONTEXT start");
        return createInt32Result(m_impl->weexRuntime->callJSOnAppContext(arguments));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::DESTORYAPPCONTEXT), [this](IPCArguments *arguments) {
        // LOGE("Weex jsserver IPCJSMsg::DESTORYAPPCONTEXT start");
        const IPCString *ipcInstanceId = arguments->getString(0);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        if (instanceID.isEmpty()) {
            return createInt32Result(static_cast<int32_t>(false));
        }

        return createInt32Result(m_impl->weexRuntime->destroyAppContext(instanceID));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSSERVICE), [this](IPCArguments *arguments) {

        const IPCString *ipcSource = arguments->getString(0);
        String source = jString2String(ipcSource->content, ipcSource->length);
        return createInt32Result(m_impl->weexRuntime->exeJsService(source));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::TAKEHEAPSNAPSHOT), [this](IPCArguments *arguments) {
        return createVoidResult();

    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECTIMERCALLBACK), [this](IPCArguments *arguments) {


        const IPCByteArray *ipcSource = arguments->getByteArray(0);
        String &&value = String::fromUTF8(ipcSource->content);
        String source = value;
        m_impl->weexRuntime->exeCTimeCallback(source);
        return createVoidResult();
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJS), [this](IPCArguments *arguments) {

        const IPCString *ipcInstanceId = arguments->getString(0);
        const IPCString *ipcNamespaceStr = arguments->getString(1);
        const IPCString *ipcFunc = arguments->getString(2);
        String instanceId = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String namespaceStr = jString2String(ipcNamespaceStr->content, ipcNamespaceStr->length);
        String func = jString2String(ipcFunc->content, ipcFunc->length);

        return createInt32Result(m_impl->weexRuntime->exeJS(instanceId, namespaceStr, func, arguments));
    });


    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSWITHRESULT), [this](IPCArguments *arguments) {
        const IPCString *ipcInstanceId = arguments->getString(0);
        const IPCString *ipcNamespaceStr = arguments->getString(1);
        const IPCString *ipcFunc = arguments->getString(2);
        String instanceId = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String namespaceStr = jString2String(ipcNamespaceStr->content, ipcNamespaceStr->length);
        String func = jString2String(ipcFunc->content, ipcFunc->length);

        WeexJSResult jsResult = m_impl->weexRuntime->exeJSWithResult(instanceId, namespaceStr, func, arguments);

        if (jsResult.length <= 0){
            return createByteArrayResult(nullptr, 0);
        }
        std::unique_ptr<IPCResult> ipcResult = createByteArrayResult(jsResult.data, jsResult.length);
        WeexJSResultDataFree(jsResult);
        return ipcResult;
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::CREATEINSTANCE), [this](IPCArguments *arguments) {

        const IPCString *ipcInstanceId = arguments->getString(0);
        const IPCString *ipcFunc = arguments->getString(1);
        const IPCString *ipcScript = arguments->getString(2);
        const IPCString *ipcOpts = arguments->getString(3);
        const IPCString *ipcInitData = arguments->getString(4);
        const IPCString *ipcExtendApi = arguments->getString(5);

        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String func = jString2String(ipcFunc->content, ipcFunc->length);
        String script = jString2String(ipcScript->content, ipcScript->length);
        String opts = jString2String(ipcOpts->content, ipcOpts->length);
        String initData = jString2String(ipcInitData->content, ipcInitData->length);
        String extendsApi = jString2String(ipcExtendApi->content, ipcExtendApi->length);

        return createInt32Result(
                m_impl->weexRuntime->createInstance(instanceID, func, script, opts, initData, extendsApi));
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::DESTORYINSTANCE), [this](IPCArguments *arguments) {
        const IPCString *ipcInstanceId = arguments->getString(0);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        if (instanceID.isEmpty()) {
            LOGE("DestoryInstance instanceId is NULL");
            return createInt32Result(static_cast<int32_t>(false));
        }

        return createInt32Result(m_impl->weexRuntime->destroyInstance(instanceID));

    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::EXECJSONINSTANCE), [this](IPCArguments *arguments) {
        const IPCString *ipcInstanceId = arguments->getString(0);
        const IPCString *ipcScript = arguments->getString(1);
        String instanceID = jString2String(ipcInstanceId->content, ipcInstanceId->length);
        String script = jString2String(ipcScript->content, ipcScript->length);

        char *result = m_impl->weexRuntime->exeJSOnInstance(instanceID, script);

        if (result == nullptr)
            return createByteArrayResult(nullptr, 0);
        else return createCharArrayResult(result);
    });

    handler->registerHandler(static_cast<uint32_t>(IPCJSMsg::UPDATEGLOBALCONFIG), [this](IPCArguments *arguments) {
        const IPCString *ipcConfig = arguments->getString(0);
        String configString = jString2String(ipcConfig->content, ipcConfig->length);
        m_impl->weexRuntime->updateGlobalConfig(configString);
        return createVoidResult();
    });
}

WeexJSServer::~WeexJSServer() {
}

void WeexJSServer::loop() {
    m_impl->listener->listen();
}

IPCSender *WeexJSServer::getSender() {
    return m_impl->sender.get();
}

IPCSerializer *WeexJSServer::getSerializer() {
    return m_impl->serializer.get();
}

#include <WeexCore/WeexJSServer/utils/WeexRuntime.h>
#include <core/manager/weex_core_manager.h>
#include <WeexCore/WeexJSServer/bridge/platform/platform_side_multi_process.h>
#include <WeexCore/WeexJSServer/bridge/platform/platform_bridge_in_multi_process.h>

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

WeexJSServer::WeexJSServer(int fd_server, int fd_client,  bool enableTrace)
        : m_impl(new WeexJSServerImpl(fd_server, enableTrace)) {
    IPCHandler *handler = m_impl->handler.get();
    this->m_ClientFd = fd_client;
    // TODO
    static_cast<weex::PlatformSideInMultiProcess*>(weex::PlatformBridgeInMultiProcess::Instance()->platform_side())->set_server(this);
    weex::PlatformBridgeInMultiProcess::Instance()->RegisterIPCCallback(handler);
    WeexCore::WeexCoreManager::getInstance()->setPlatformBridge(weex::PlatformBridgeInMultiProcess::Instance());
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

IPCHandler *WeexJSServer::getHandler() {
    return m_impl->handler.get();
}
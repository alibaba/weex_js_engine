#include <WeexCore/WeexJSServer/utils/WeexRuntime.h>
#include <core/manager/weex_core_manager.h>
#include <WeexCore/WeexJSServer/bridge/platform/platform_side_multi_process.h>
#include <WeexCore/WeexJSServer/bridge/platform/platform_bridge_in_multi_process.h>
#include <WeexCore/WeexJSServer/object/WeexEnv.h>

using namespace JSC;
using namespace WTF;
using namespace WEEXICU;

extern bool config_use_wson;

struct WeexJSServer::WeexJSServerImpl {
    WeexJSServerImpl(int serverFd, int clientFd, bool enableTrace);

    std::unique_ptr<IPCFutexPageQueue> futexPageQueue;
    std::unique_ptr<IPCSender> sender;
    std::unique_ptr<IPCHandler> handler;
    std::unique_ptr<IPCListener> listener;
    std::unique_ptr<IPCSerializer> serializer;
};

WeexJSServer::WeexJSServerImpl::WeexJSServerImpl(int serverFd, int clientFd, bool enableTrace) {
    WeexEnv::env()->setIpcServerFd(serverFd);
    WeexEnv::env()->setIpcClientFd(clientFd);
    WeexEnv::env()->setEnableTrace(enableTrace);

    int _fd = serverFd;
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
}

WeexJSServer::WeexJSServer(int serverFd, int clientFd, bool enableTrace)
        : m_impl(new WeexJSServerImpl(serverFd, clientFd, enableTrace)) {


    IPCHandler *handler = m_impl->handler.get();
    // TODO
    static_cast<weex::PlatformSideInMultiProcess *>(weex::PlatformBridgeInMultiProcess::Instance()->platform_side())->set_server(
            this);
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
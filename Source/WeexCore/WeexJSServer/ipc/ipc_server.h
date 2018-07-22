#ifndef WEEXJSSERVER_H
#define WEEXJSSERVER_H
#include <memory>
class IPCSender;
class IPCSerializer;
class IPCHandler;
class WeexJSServer {
public:
    WeexJSServer(int serverFd, int clientFd, bool enableTrace);
    ~WeexJSServer();
    void loop();
    IPCSender* getSender();
    IPCSerializer* getSerializer();
    IPCHandler* getHandler();
private:
    struct WeexJSServerImpl;
    std::unique_ptr<WeexJSServerImpl> m_impl;
};
#endif /* WEEXJSSERVER_H */

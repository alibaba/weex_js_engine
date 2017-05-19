#ifndef IPCSENDER_H
#define IPCSENDER_H
#include <memory>
class IPCBuffer;
class IPCResult;
class IPCHandler;

class IPCSender {
public:
    virtual ~IPCSender() = default;
    virtual std::unique_ptr<IPCResult> send(IPCBuffer* buffer) = 0;
};
std::unique_ptr<IPCSender> createIPCSender(int fd, IPCHandler* handler, bool ownFd);
#endif /* IPCSENDER_H */

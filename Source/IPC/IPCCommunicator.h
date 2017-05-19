#ifndef IPCCOMMUNICATOR_H
#define IPCCOMMUNICATOR_H
#include <memory>

class IPCResult;
class IPCArguments;
class IPCBuffer;
class IPCCommunicator {
protected:
    explicit IPCCommunicator(int fd);
    virtual ~IPCCommunicator();

    std::unique_ptr<IPCResult> assembleResult();
    std::unique_ptr<IPCArguments> assembleArguments();
    std::unique_ptr<IPCBuffer> generateResultBuffer(IPCResult*);
    void doSendBufferOnly(IPCBuffer* buffer);
    uint32_t doReadPackage();
    void releaseFd();

    inline const char* getBlob()
    {
        return m_package.get() + sizeof(uint32_t);
    }

    inline void clearBlob()
    {
        m_package.reset();
    }

private:
    void doSendBufferOnly(const void* data, size_t s);
    void doRecvBufferOnly(void* data, size_t s);
    int m_fd;
    std::unique_ptr<char[]> m_package;
};
#endif /* IPCCOMMUNICATOR_H */

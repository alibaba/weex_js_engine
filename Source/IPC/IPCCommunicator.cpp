#include "IPCCommunicator.h"

#include "Buffering/IPCBuffer.h"
#include "IPCArguments.h"
#include "IPCByteArray.h"
#include "IPCCheck.h"
#include "IPCException.h"
#include "IPCResult.h"
#include "IPCString.h"
#include "Serializing/IPCSerializer.h"
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <vector>

namespace {
class BufferAssembler
    : public IPCResult,
      public IPCArguments {
public:
    void readFromBuffer(const char*);
    void readTypes(const char*&);
    void readData(const char*&);
    // IPCResult
    const void* getData() override;
    IPCType getType() override;
    const uint16_t* getStringContent() override;
    size_t getStringLength() override;
    const char* getByteArrayContent() override;
    size_t getByteArrayLength() override;

    // IPCArguments
    const void* getData(int index) override;
    IPCType getType(int index) override;
    size_t getCount() override;

private:
    std::vector<uint32_t> m_types;
    std::vector<std::unique_ptr<char[]>> m_datas;
};

void BufferAssembler::readFromBuffer(const char* blob)
{
    readTypes(blob);
    readData(blob);
}

void BufferAssembler::readTypes(const char*& blob)
{
    while (true) {
        uint32_t type;
        type = *reinterpret_cast<const uint32_t*>(blob);
        blob += sizeof(uint32_t);
        if (type == static_cast<uint32_t>(IPCType::END))
            break;
        m_types.emplace_back(type);
    }
}

void BufferAssembler::readData(const char*& blob)
{
    for (uint32_t type : m_types) {
        switch (static_cast<IPCType>(type)) {
        case IPCType::INT32: {
            int32_t n;
            memcpy(&n, blob, sizeof(n));
            blob += sizeof(n);
            std::unique_ptr<char[]> data(new char[sizeof(n)]);
            *reinterpret_cast<int32_t*>(data.get()) = n;
            m_datas.emplace_back(std::move(data));
        } break;
        case IPCType::INT64: {
            int64_t n;
            memcpy(&n, blob, sizeof(n));
            blob += sizeof(n);
            std::unique_ptr<char[]> data(new char[sizeof(n)]);
            *reinterpret_cast<int64_t*>(data.get()) = n;
            m_datas.emplace_back(std::move(data));
        } break;
        case IPCType::FLOAT: {
            float n;
            memcpy(&n, blob, sizeof(n));
            blob += sizeof(n);
            std::unique_ptr<char[]> data(new char[sizeof(n)]);
            *reinterpret_cast<float*>(data.get()) = n;
            m_datas.emplace_back(std::move(data));
        } break;
        case IPCType::DOUBLE: {
            double n;
            memcpy(&n, blob, sizeof(n));
            blob += sizeof(n);
            std::unique_ptr<char[]> data(new char[sizeof(n)]);
            *reinterpret_cast<double*>(data.get()) = n;
            m_datas.emplace_back(std::move(data));
        } break;
        case IPCType::JSONSTRING:
        case IPCType::STRING: {
            uint32_t n;
            memcpy(&n, blob, sizeof(n));
            blob += sizeof(n);
            size_t byteLength = n * sizeof(uint16_t);
            std::unique_ptr<char[]> data(new char[byteLength + sizeof(IPCString)]);
            IPCString* s = reinterpret_cast<IPCString*>(data.get());
            s->length = n;
            memcpy(s->content, blob, byteLength);
            blob += byteLength;
            m_datas.emplace_back(std::move(data));
        } break;
        case IPCType::BYTEARRAY: {
            uint32_t n;
            memcpy(&n, blob, sizeof(n));
            blob += sizeof(n);
            size_t byteLength = n;
            std::unique_ptr<char[]> data(new char[byteLength + sizeof(IPCByteArray)]);
            IPCByteArray* s = reinterpret_cast<IPCByteArray*>(data.get());
            s->length = n;
            memcpy(s->content, blob, byteLength);
            blob += byteLength;
            s->content[byteLength] = '\0';
            m_datas.emplace_back(std::move(data));
        } break;
        case IPCType::JSUNDEFINED:
        case IPCType::VOID:
            m_datas.emplace_back();
            break;
        case IPCType::END:
        default:
            IPC_UNREACHABLE();
        }
    }
}

// IPCResult
const void* BufferAssembler::getData()
{
    return getData(0);
}

IPCType BufferAssembler::getType()
{
    return getType(0);
}

const uint16_t* BufferAssembler::getStringContent()
{
    IPC_DCHECK((getType(0) == IPCType::STRING) || (getType(0) == IPCType::JSONSTRING));
    const IPCString* s = reinterpret_cast<const IPCString*>(getData(0));
    return s->content;
}

size_t BufferAssembler::getStringLength()
{
    IPC_DCHECK((getType(0) == IPCType::STRING) || (getType(0) == IPCType::JSONSTRING));
    const IPCString* s = reinterpret_cast<const IPCString*>(getData(0));
    return s->length;
}

const char* BufferAssembler::getByteArrayContent()
{
    IPC_DCHECK(getType(0) == IPCType::BYTEARRAY);
    const IPCByteArray* s = reinterpret_cast<const IPCByteArray*>(getData(0));
    return s->content;
}

size_t BufferAssembler::getByteArrayLength()
{
    IPC_DCHECK(getType(0) == IPCType::BYTEARRAY);
    const IPCByteArray* s = reinterpret_cast<const IPCByteArray*>(getData(0));
    return s->length;
}

// IPCArguments
const void* BufferAssembler::getData(int index)
{
    return m_datas.at(index).get();
}

IPCType BufferAssembler::getType(int index)
{
    return static_cast<IPCType>(m_types.at(index));
}

size_t BufferAssembler::getCount()
{
    return m_types.size();
}
}

IPCCommunicator::IPCCommunicator(int fd)
    : m_fd(fd)
{
}

IPCCommunicator::~IPCCommunicator()
{
    if (m_fd != -1)
        close(m_fd);
}

std::unique_ptr<IPCResult> IPCCommunicator::assembleResult()
{
    std::unique_ptr<BufferAssembler> bufferAssembler(new BufferAssembler());
    bufferAssembler->readFromBuffer(getBlob());
    return std::unique_ptr<IPCResult>(bufferAssembler.release());
}

std::unique_ptr<IPCArguments> IPCCommunicator::assembleArguments()
{
    std::unique_ptr<BufferAssembler> bufferAssembler(new BufferAssembler());
    bufferAssembler->readFromBuffer(getBlob());
    return std::unique_ptr<IPCArguments>(bufferAssembler.release());
}

std::unique_ptr<IPCBuffer> IPCCommunicator::generateResultBuffer(IPCResult* result)
{
    std::unique_ptr<IPCSerializer> serializer = createIPCSerializer();
    serializer->setMsg(MSG_END);
    switch (result->getType()) {
    case IPCType::INT32:
        serializer->add(result->get<int32_t>());
        break;
    case IPCType::INT64:
        serializer->add(result->get<int64_t>());
        break;
    case IPCType::FLOAT:
        serializer->add(result->get<float>());
        break;
    case IPCType::DOUBLE:
        serializer->add(result->get<double>());
        break;
    case IPCType::BYTEARRAY:
        // FIXME: Not implementd.
        break;
    case IPCType::JSONSTRING:
        serializer->addJSON(result->getStringContent(), result->getStringLength());
        break;
    case IPCType::STRING:
        serializer->add(result->getStringContent(), result->getStringLength());
        break;
    case IPCType::JSUNDEFINED:
        serializer->addJSUndefined();
        break;
    case IPCType::VOID:
        serializer->addVoid();
        break;
    default:
        IPC_UNREACHABLE();
    }
    return serializer->finish();
}

void IPCCommunicator::doSendBufferOnly(IPCBuffer* buffer)
{
    const char* data = static_cast<const char*>(buffer->get());
    uint32_t length = buffer->length();
    doSendBufferOnly(&length, sizeof(length));
    doSendBufferOnly(data, length);
}

uint32_t IPCCommunicator::doReadPackage()
{
    uint32_t msg;
    uint32_t length;
    doRecvBufferOnly(&length, sizeof(length));
    m_package.reset(new char[length]);
    if (length < 2 * sizeof(uint32_t)) {
        clearBlob();
        throw IPCException("Not a vaild msg");
    }
    doRecvBufferOnly(m_package.get(), length);
    return *reinterpret_cast<uint32_t*>(m_package.get());
}

void IPCCommunicator::releaseFd()
{
    m_fd = -1;
}

void IPCCommunicator::doSendBufferOnly(const void* _data, size_t length)
{
    const char* data = static_cast<const char*>(_data);
    while (length > 0) {
        ssize_t byteTransfered;
        byteTransfered = write(m_fd, data, length);
        if (byteTransfered <= 0 && errno != EAGAIN) {
            throw IPCException("send msg failed: %s", strerror(errno));
        }
        data += byteTransfered;
        length -= byteTransfered;
    }
}

void IPCCommunicator::doRecvBufferOnly(void* _data, size_t length)
{
    char* data = static_cast<char*>(_data);
    while (length > 0) {
        ssize_t byteTransfered;
        byteTransfered = read(m_fd, data, length);
        if (byteTransfered <= 0 && errno != EAGAIN) {
            throw IPCException("recv msg failed: %s", strerror(errno));
        }
        data += byteTransfered;
        length -= byteTransfered;
    }
}

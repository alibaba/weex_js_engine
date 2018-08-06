//
// Created by Darin on 06/07/2018.
//

#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <memory>
#include <WeexCore/WeexJSServer/utils/LogUtils.h>
#include "WeexIPCClient.h"

WeexIPCClient::WeexIPCClient(int fd) {
    void *base = mmap(nullptr, IPCFutexPageQueue::ipc_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (base == MAP_FAILED) {
        int _errno = errno;
        close(fd);
        throw IPCException("failed to map ashmem region: %s", strerror(_errno));
    }


    futexPageQueue.reset(new IPCFutexPageQueue(base, IPCFutexPageQueue::ipc_size, 1));
    handler = std::move(createIPCHandler());
    sender = std::move(createIPCSender(futexPageQueue.get(), handler.get()));
    serializer = std::move(createIPCSerializer());
    close(fd);
}


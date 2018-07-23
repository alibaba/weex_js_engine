//
// Created by Darin on 06/07/2018.
//

#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <IPC/IPCException.h>
#include <IPC/IPCHandler.h>
#include <IPCFutexPageQueue.h>
#include <memory>
#include <WeexCore/WeexJSServer/utils/LogUtils.h>
#include "WeexIPCClient.h"

WeexIPCClient::WeexIPCClient(int fd) {
    void *base = mmap(nullptr, 4 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    LOGE("ipc client and fd is %d", fd);
    if (base == MAP_FAILED) {
        int _errno = errno;
        close(fd);
        LOGE("failed to map ashmem region: %s %d", strerror(_errno), fd);
        throw IPCException("failed to map ashmem region: %s", strerror(_errno));
    }

    futexPageQueue = std::make_unique<IPCFutexPageQueue>(base, 4 * 1024 * 1024, 1);
    handler = std::move(createIPCHandler());
    sender = std::move(createIPCSender(futexPageQueue.get(), handler.get()));
    serializer = std::move(createIPCSerializer());
    close(fd);
}


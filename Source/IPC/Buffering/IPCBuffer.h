#ifndef IPCBUFFER_H
#define IPCBUFFER_H
#include <stddef.h>

class IPCBuffer {
public:
    virtual ~IPCBuffer() = default;
    virtual const void* get() = 0;
    virtual size_t length() = 0;
};

#endif /* IPCBUFFER_H */

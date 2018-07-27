//
// Created by Darin on 06/07/2018.
//

#ifndef WEEXV8_WEEXIPCCLIENT_H
#define WEEXV8_WEEXIPCCLIENT_H


#include <IPC/IPCFutexPageQueue.h>
#include <IPC/IPCSender.h>
#include <IPC/IPCListener.h>
#include <IPC/Serializing/IPCSerializer.h>
#include <IPC/IPCException.h>
#include <IPC/IPCHandler.h>

class IPCSender;
class IPCSerializer;
class IPCHandler;
class IPCFutexPageQueue;

class WeexIPCClient {

public:
    explicit WeexIPCClient(int fd);

    inline IPCSerializer *getSerializer() {
        return serializer.get();;
    }

    inline IPCSender *getSender() {
        return sender.get();
    };

public:
    std::unique_ptr<IPCFutexPageQueue> futexPageQueue;
    std::unique_ptr<IPCSender> sender;
    std::unique_ptr<IPCHandler> handler;
    std::unique_ptr<IPCSerializer> serializer;

};


#endif //WEEXV8_WEEXIPCCLIENT_H

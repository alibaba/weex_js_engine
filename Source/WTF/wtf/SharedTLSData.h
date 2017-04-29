#if defined(WTF_THREAD_KEY_COMBINE)
#ifndef SharedTLSData_h
#define SharedTLSData_h

#include <wtf/Noncopyable.h>

#include "FastMalloc.h"
#include "MainThread.h"
#include "StdLibExtras.h"
#include "ThreadSpecific.h"

namespace WTF {

typedef void (*Destructor)(void*);

class SharedTLSData {
    WTF_MAKE_NONCOPYABLE(SharedTLSData);

public:
    WTF_EXPORT_PRIVATE SharedTLSData();
    WTF_EXPORT_PRIVATE ~SharedTLSData();

private:
    bool m_isSetGCThreadType : 1;
    bool m_isSetThreadDataRefPtr : 1;
    bool m_isSetIsCompilationThread : 1;
    bool m_isSetWTFThreadData : 1;
    bool m_isSetIdentifierData : 1;
    bool m_isSetMachineThreads : 1;

    /**
    * for isGCThread of main thread (MainThread.cpp)
    * storage for std::optional<GCThreadType>
    */
    std::optional<GCThreadType> m_gCThreadType;

    /**
    * for threadData of ParkingLot (ParkingLot.cpp)
    * pointer to RefPtr<ThreadData>, really storage for RefPtr<ThreadData>
    */
    void* m_ptrThreadDataRefPtr;

    /**
    * for s_isCompilationThread of CompilationThread (CompilationThread.cpp)
    * storage for bool
    */
    bool m_isCompilationThread;

    /**
    * for staticData of WTFThreadData (WTFThreadData.h)
    * pointer to WTFThreadData, really storage for WTFThreadData
    */
    void* m_wtfThreadData;

    /**
    * for ThreadIdentifierData::m_key (ThreadIdentifierDataPthreads.cpp)
    * storage for a pointer : ThreadIdentifierData*
    */
    void* m_identifierData; // ThreadIdentifierData*

    /**
    * for m_threadSpecificForMachineThreads of MachineThreads
    * (MachineStackMarker.cpp) storage for a pointer : MachineThreads*
    */
    void* m_machineThreads;

public:
    static void initSharedTLSData();

    /**
    * gCThreadType
    */
    std::optional<GCThreadType>& getGCThreadType();
    bool isSetGCThreadType();
    void setGCThreadType(GCThreadType&);

    /**
    * ptrThreadDataRefPtr
    */
    template <typename T>
    T& getThreadDataRefPtr();
    bool isSetThreadDataRefPtr();
    template <typename T>
    void setThreadDataRefPtr(T&);

    /**
    * isCompilationThread
    */
    bool& getIsCompilationThread();
    bool isSetIsCompilationThread();
    void setIsCompilationThread(bool);

    /**
    * wtfThreadData
    */
    template <typename T>
    T& getWTFThreadData();
    /*bool isSet_wtfThreadData();
    template<typename T> void set_wtfThreadData(T&);*/

    /**
    * identifierData
    */
    template <typename T>
    T* getIdentifierData();
    bool isSetIdentifierData();
    template <typename T>
    void setIdentifierData(T*);
    template <typename T>
    void destructIdentifierData(Destructor destructor);

    /**
    * machineThreads
    */
    void* getMachineThreads();
    bool isSetMachineThreads();
    void setMachineThreads(void*);
    void destructMachineThreads(Destructor);
};

//------------------------------------------------------------------------------
template <typename T>
T& SharedTLSData::getThreadDataRefPtr()
{
    if (!m_ptrThreadDataRefPtr) {
        m_ptrThreadDataRefPtr
            = static_cast<void*>(fastZeroedMalloc(sizeof(T)));
        new (NotNull, m_ptrThreadDataRefPtr) T;
    }

    return *((T*)m_ptrThreadDataRefPtr);
}

template <typename T>
void SharedTLSData::setThreadDataRefPtr(T& thData)
{
    T& oldData = getThreadDataRefPtr<T>();
    oldData = thData;
    m_isSetThreadDataRefPtr = true;
}

//------------------------------------------------------------------------------
template <typename T>
T& SharedTLSData::getWTFThreadData()
{
    if (!m_wtfThreadData) {
        m_wtfThreadData = static_cast<void*>(fastZeroedMalloc(sizeof(T)));
        new (NotNull, m_wtfThreadData) T;
    }

    return *((T*)m_wtfThreadData);
}

//------------------------------------------------------------------------------
template <typename T>
T* SharedTLSData::getIdentifierData()
{
    return (T*)m_identifierData;
}

template <typename T>
void SharedTLSData::setIdentifierData(T* data)
{
    m_identifierData = data;
    m_isSetIdentifierData = true;
}

template <typename T>
void SharedTLSData::destructIdentifierData(Destructor destructor)
{
    (*destructor)(m_identifierData);
}

extern ThreadSpecific<SharedTLSData, CanBeGCThread::True>* s_sharedTLSData;
}

#endif // SharedTLSData_h

#endif // WTF_THREAD_KEY_COMBINE

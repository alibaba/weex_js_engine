#include "config.h"
#include "SharedTLSData.h"

#include "ThreadSpecific.h"

#include <mutex>
#include <thread>

namespace WTF {

ThreadSpecific<SharedTLSData, CanBeGCThread::True>* s_sharedTLSData = 0;

void SharedTLSData::initSharedTLSData()
{
    static std::once_flag initializeSharedThreadsOnceFlag;
    std::call_once(initializeSharedThreadsOnceFlag, [] {
        s_sharedTLSData = new ThreadSpecific<SharedTLSData, CanBeGCThread::True>();
    });
}

SharedTLSData::SharedTLSData()
    : m_isSetGCThreadType(false)
    , m_isSetThreadDataRefPtr(false)
    , m_isSetIsCompilationThread(false)
    , m_isSetWTFThreadData(false)
    , m_isSetIdentifierData(false)
    , m_isSetMachineThreads(false)
    , m_ptrThreadDataRefPtr(0)
    , m_wtfThreadData(0)
    , m_identifierData(0)
    , m_machineThreads(0)
{
}

SharedTLSData::~SharedTLSData()
{
}

//--------------------------------gCThreadType----------------------------------
std::optional<GCThreadType>& SharedTLSData::getGCThreadType()
{
    return m_gCThreadType;
}

bool SharedTLSData::isSetGCThreadType()
{
    return m_isSetGCThreadType;
}

void SharedTLSData::setGCThreadType(GCThreadType& gCType)
{
    m_gCThreadType = gCType;
    m_isSetGCThreadType = true;
}

//-----------------------------ptrThreadDataRefPtr------------------------------
bool SharedTLSData::isSetThreadDataRefPtr()
{
    return m_isSetThreadDataRefPtr;
}

//-----------------------------isCompilationThread------------------------------
bool& SharedTLSData::getIsCompilationThread()
{
    return m_isCompilationThread;
}

bool SharedTLSData::isSetIsCompilationThread()
{
    return m_isSetIsCompilationThread;
}

void SharedTLSData::setIsCompilationThread(bool b)
{
    m_isCompilationThread = b;
    m_isSetIsCompilationThread = true;
}

//------------------------------identifierData----------------------------------
bool SharedTLSData::isSetIdentifierData()
{
    return m_isSetIdentifierData;
}

//----------------------------machineThreads------------------------------------
void* SharedTLSData::getMachineThreads()
{
    return m_machineThreads;
}

bool SharedTLSData::isSetMachineThreads()
{
    return m_isSetMachineThreads;
}

void SharedTLSData::setMachineThreads(void* data)
{
    m_machineThreads = data;
    m_isSetMachineThreads = true;
}

void SharedTLSData::destructMachineThreads(Destructor destructor)
{
    (*destructor)(m_machineThreads);
}
}

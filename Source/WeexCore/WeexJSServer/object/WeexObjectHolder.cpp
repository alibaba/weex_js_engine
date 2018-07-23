//
// Created by Darin on 10/04/2018.
//

#include "WeexObjectHolder.h"
#include "WeexCore/WeexJSServer/object/WeexEnv.h"

void WeexObjectHolder::initFromIPCArguments(IPCArguments *arguments, size_t startCount, bool forAppContext) {
    size_t count = arguments->getCount();
    std::vector<INIT_FRAMEWORK_PARAMS *> params;

    for (size_t i = startCount; i < count; i += 2) {
        if (arguments->getType(i) != IPCType::BYTEARRAY) {
            continue;
        }
        if (arguments->getType(1 + i) != IPCType::BYTEARRAY) {
            continue;
        }
        const IPCByteArray *ba = arguments->getByteArray(1 + i);

        const IPCByteArray *ba_type = arguments->getByteArray(i);

        auto init_framework_params = (INIT_FRAMEWORK_PARAMS *) malloc(sizeof(INIT_FRAMEWORK_PARAMS));

        if (init_framework_params == nullptr) {
            return;
        }

        memset(init_framework_params, 0, sizeof(INIT_FRAMEWORK_PARAMS));

        init_framework_params->type = IPCByteArrayToWeexByteArray(ba_type);
        init_framework_params->value = IPCByteArrayToWeexByteArray(ba);

        params.push_back(init_framework_params);
    }

    initFromParams(params, forAppContext);
}

void WeexObjectHolder::initFromParams(std::vector<INIT_FRAMEWORK_PARAMS *> params, bool forAppContext) {
    if (!WEEXICU::initICUEnv(WeexEnv::env()->multiProcess())) {
        LOGE("failed to init ICUEnv single process");
        // return false;
    }

    Options::enableRestrictedOptions(true);
// Initialize JSC before getting VM.
    WTF::initializeMainThread();
    initHeapTimer();
    JSC::initializeThreading();
#if ENABLE(WEBASSEMBLY)
    JSC::Wasm::enableFastMemory();
#endif
    m_globalVM = std::move(VM::create(LargeHeap));
    VM &vm = *m_globalVM.get();
    JSLockHolder locker(&vm);
    WeexGlobalObject *globalObject = WeexGlobalObject::create(vm, WeexGlobalObject::createStructure(vm, jsNull()));
    globalObject->initWxEnvironment(params, forAppContext, true);
    if (forAppContext)
        globalObject->initFunctionForAppContext();
    else
        globalObject->initFunction();

    m_globalObject.set(vm, globalObject);
    wson::init(&vm);
}

WeexObjectHolder::WeexObjectHolder() {
}

WeexGlobalObject *WeexObjectHolder::cloneWeexObject(bool initContext, bool forAppContext) {
    VM &vm = *(m_globalVM.get());
    JSLockHolder locker(&vm);
    auto *temp_object = WeexGlobalObject::create(vm,
                                                 WeexGlobalObject::createStructure(vm, jsNull()));
    temp_object->initWxEnvironment(m_globalObject->m_initFrameworkParams, forAppContext, false);

    if (forAppContext)
        temp_object->initFunctionForAppContext();
    else if (initContext)
        temp_object->initFunctionForContext();
    else
        temp_object->initFunction();

//    temp_object->m_server = m_globalObject.get()->m_server;
    return temp_object;
}

WeexObjectHolder::~WeexObjectHolder() {
    VM &vm = *(m_globalVM.get());
    wson::destory();
    JSLockHolder locker(&vm);
    vm.heap.collectAllGarbage();
    m_globalVM = nullptr;
}


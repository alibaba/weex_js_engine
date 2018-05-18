//
// Created by Darin on 11/02/2018.
//

#ifndef WEEXV8_WEEXGLOBALOBJECT_H
#define WEEXV8_WEEXGLOBALOBJECT_H

#include <map>
#include "WeexJSServer.h"
#include "../utils/Utils.h"
#include "WeexApiHeader.h"
#include "APICast.h"
#include "JSStringRef.h"
#include "JSContextRef.h"

using namespace JSC;

class WeexGlobalObject: public JSGlobalObject {
private:
    WeexGlobalObject(VM&, Structure*);

public:
    typedef JSGlobalObject Base;
    friend class WeexJSServer;
    WeexJSServer* m_server{ nullptr };
    std::vector<INIT_FRAMEWORK_PARAMS *> m_initFrameworkParams;
    std::string id = "";
    static WeexGlobalObject* create(VM& vm, Structure* structure)
    {
        WeexGlobalObject* object = new (NotNull, allocateCell<WeexGlobalObject>(vm.heap)) WeexGlobalObject(vm, structure);
        object->finishCreation(vm);
        return object;
    }

    static const bool needsDestruction = false;

    DECLARE_INFO;
    static const GlobalObjectMethodTable s_globalObjectMethodTable;

    static Structure* createStructure(VM& vm, JSValue prototype)
    {
        return Structure::create(vm, 0, prototype, TypeInfo(GlobalObjectType, StructureFlags), info());
    }

    static RuntimeFlags javaScriptRuntimeFlags(const JSGlobalObject*) { return RuntimeFlags::createAllEnabled(); }

//    void initWXEnvironment(IPCArguments* arguments);

    void initWXEnvironmentWithIPCArguments(IPCArguments *arguments, bool forAppContext, bool isSave);
    void initWxEnvironment(std::vector<INIT_FRAMEWORK_PARAMS *> params, bool forAppContext, bool isSave);
    void initFunctionForContext();
    void initFunctionForAppContext();
    void initFunction();
protected:
    void finishCreation(VM& vm)
    {
        Base::finishCreation(vm);
        //addFunction(vm, "gc", functionGCAndSweep, 0);
    }

    void addFunction(VM& vm, JSObject* object, const char* name, NativeFunction function, unsigned arguments)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        object->putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function));
    }

    void addFunction(VM& vm, const char* name, NativeFunction function, unsigned arguments)
    {
        addFunction(vm, this, name, function, arguments);
    }

    void addConstructableFunction(VM& vm, const char* name, NativeFunction function, unsigned arguments)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        putDirect(vm, identifier, JSFunction::create(vm, this, arguments, identifier.string(), function, NoIntrinsic, function));
    }

    void addString(VM& vm, JSObject* object, const char* name, String&& value)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        JSString* jsString = jsNontrivialString(&vm, WTFMove(value));
        object->putDirect(vm, identifier, jsString);
    }

    void addValue(VM& vm, JSObject* object, const char* name, JSValue value)
    {
        Identifier identifier = Identifier::fromString(&vm, name);
        object->putDirect(vm, identifier, value);
    }

    void addValue(VM& vm, const char* name, JSValue value)
    {
        addValue(vm, this, name, value);
    }
};


#endif //WEEXV8_WEEXGLOBALOBJECT_H

//
// Created by Darin on 11/02/2018.
//

#ifndef WEEXV8_SIMPLEOBJECT_H
#define WEEXV8_SIMPLEOBJECT_H
#include "../../../JavaScriptCore/runtime/JSArray.h"
#include "../../../JavaScriptCore/runtime/JSArrayBuffer.h"
#include "../../../JavaScriptCore/runtime/JSCInlines.h"
#include "../../../JavaScriptCore/runtime/JSFunction.h"
#include "../../../JavaScriptCore/runtime/JSInternalPromise.h"
#include "../../../JavaScriptCore/runtime/JSInternalPromiseDeferred.h"
#include "../../../JavaScriptCore/runtime/JSLock.h"
#include "../../../JavaScriptCore/runtime/JSModuleLoader.h"
#include "../../../JavaScriptCore/runtime/JSNativeStdFunction.h"
#include "../../../JavaScriptCore/runtime/JSONObject.h"
#include "../../../JavaScriptCore/runtime/JSProxy.h"
#include "../../../JavaScriptCore/runtime/JSSourceCode.h"
#include "../../../JavaScriptCore/runtime/JSString.h"
#include "../../../JavaScriptCore/runtime/JSTypedArrays.h"
#include "../../../JavaScriptCore/wasm/js/JSWebAssemblyCallee.h"

using namespace JSC;

class SimpleObject : public JSNonFinalObject {
public:
    SimpleObject(VM& vm, Structure* structure)
            : Base(vm, structure)
    {
    }

    typedef JSNonFinalObject Base;
    static const bool needsDestruction = false;

    static SimpleObject* create(VM& vm, JSGlobalObject* globalObject)
    {
        Structure* structure = createStructure(vm, globalObject, jsNull());
        SimpleObject* simpleObject = new (NotNull, allocateCell<SimpleObject>(vm.heap, sizeof(SimpleObject))) SimpleObject(vm, structure);
        simpleObject->finishCreation(vm);
        return simpleObject;
    }

    static void visitChildren(JSCell* cell, SlotVisitor& visitor)
    {
        SimpleObject* thisObject = jsCast<SimpleObject*>(cell);
        ASSERT_GC_OBJECT_INHERITS(thisObject, info());
        Base::visitChildren(thisObject, visitor);
    }

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

    DECLARE_INFO;
};


#endif //WEEXV8_SIMPLEOBJECT_H

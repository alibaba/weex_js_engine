//
// Created by Darin on 03/08/2018.
//

#ifndef WEEXV8_WEEXCONSOLEOBJECT_H
#define WEEXV8_WEEXCONSOLEOBJECT_H

#include "JSObject.h"

using namespace JSC;

class WeexConsoleObject : public JSNonFinalObject {
private:
    WeexConsoleObject(VM&, Structure*);

public:
    typedef JSNonFinalObject Base;

    static WeexConsoleObject* create(VM& vm, JSGlobalObject* globalObject, Structure* structure)
    {
        WeexConsoleObject* object = new (NotNull, allocateCell<WeexConsoleObject>(vm.heap)) WeexConsoleObject(vm, structure);
        object->finishCreation(vm, globalObject);
        return object;
    }

    DECLARE_INFO;

    static Structure* createStructure(VM& vm, JSGlobalObject* globalObject, JSValue prototype)
    {
        return Structure::create(vm, globalObject, prototype, TypeInfo(ObjectType, StructureFlags), info());
    }

protected:
    void finishCreation(VM&, JSGlobalObject*);
};


#endif //WEEXV8_WEEXCONSOLEOBJECT_H

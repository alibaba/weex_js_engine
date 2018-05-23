//
//  wsonjsc.hpp
//  JavaScriptCore
//
//  Created by furture on 2017/8/30.
//
//

#ifndef wsonjsc_h
#define wsonjsc_h
#include "config.h"
#include "JSCInlines.h"
#include "PropertyNameArray.h"
#include "IdentifierInlines.h"
#include "LocalScope.h"
#include "BooleanObject.h"
#include "wson.h"

using namespace JSC;


namespace wson {
    wson_buffer* toWson(ExecState* state, JSValue val);
    JSValue toJSValue(ExecState* state, wson_buffer* buffer);
    JSValue toJSValue(ExecState* state, void* buffer, int length);

    /**
     * performance improve wson toJSValue. very big import improve
     */
    void init(VM* vm);
    void destory();
}



#endif /* wsonjsc_hpp */

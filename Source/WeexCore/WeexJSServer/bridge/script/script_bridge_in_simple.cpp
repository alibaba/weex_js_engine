//
// Created by yxp on 2018/6/29.
//

#include <WeexCore/WeexJSServer/object/WeexEnv.h>
#include "WeexCore/WeexJSServer/bridge/script/script_bridge_in_simple.h"
#include "WeexCore/WeexJSServer/bridge/script/core_side_in_simple.h"
#include "WeexCore/WeexJSServer/bridge/script/script_side_in_simple.h"
#include "WeexCore/WeexJSServer/bridge/script/script_side_in_queue.h"
#include "WeexCore/WeexJSServer/utils/WeexRuntime.h"
namespace weex {
namespace bridge {
namespace js {
ScriptBridgeInSimple* ScriptBridgeInSimple::g_instance = NULL;

ScriptBridgeInSimple::ScriptBridgeInSimple() {
  WeexEnv::env()->setScriptBridge(this);
  set_script_side(new ScriptSideInSimple(new WeexRuntime(this, false)));
  set_core_side(new CoreSideInSimple());
}

ScriptBridgeInSimple::~ScriptBridgeInSimple() {}
}  // namespace js
}  // namespace bridge
}  // namespace weex
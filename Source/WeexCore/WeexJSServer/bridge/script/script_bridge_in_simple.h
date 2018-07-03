//
// Created by yxp on 2018/6/29.
//

#ifndef WEEXV8_SCRIPT_BRIDGE_IN_SIMPLE_H
#define WEEXV8_SCRIPT_BRIDGE_IN_SIMPLE_H

#include "core/bridge/script_bridge.h"

namespace weex {
namespace bridge {
namespace js {
class ScriptBridgeInSimple : public WeexCore::ScriptBridge {
 public:
  static ScriptBridgeInSimple *Instance() {
    if (g_instance == NULL) {
      g_instance = new ScriptBridgeInSimple();
    }
    return g_instance;
  }

 private:
  static ScriptBridgeInSimple *g_instance;
  ScriptBridgeInSimple();
  virtual ~ScriptBridgeInSimple();
  DISALLOW_COPY_AND_ASSIGN(ScriptBridgeInSimple);
};
}  // namespace js
}  // namespace bridge
}  // namespace weex

#endif  // WEEXV8_SCRIPT_BRIDGE_IN_SIMPLE_H

#pragma once

#include <public/igameevents.h>

#include <map>
#include <string>

#include "core/globals.h"
#include "core/global_listener.h"
#include "core/script_engine.h"

namespace vspdotnet {
class ScriptCallback;
class PluginFunction;
}

struct EventHook {
  EventHook() {
    PreHook = nullptr;
    PostHook = nullptr;
  }
  vspdotnet::ScriptCallback* PreHook;
  vspdotnet::ScriptCallback* PostHook;
  std::string name;
};

namespace vspdotnet {

class EventManager : public GlobalClass,
                      public IGameEventListener2 {
 public:
  EventManager();
  ~EventManager();

 public:  // GlobalClass
  void OnShutdown() override;
  void OnAllInitialized() override;
  void OnStartup() override;

 public:  // IGameEventListener2
  void FireGameEvent(IGameEvent* event) override;

 public:
  bool UnhookEvent(const char* name, CallbackT callback,
                   bool post);
  bool HookEvent(const char* name, CallbackT callback,
                 bool post);
  int GetEventDebugID() override { return EVENT_DEBUG_ID_INIT; }

 private:
  bool OnFireEvent(IGameEvent* pEvent, bool bDontBroadcast);
  bool OnFireEvent_Post(IGameEvent* pEvent, bool bDontBroadcast);

 private:
  std::map<std::string, EventHook*> m_hooks;
};

}  // namespace vspdotnet

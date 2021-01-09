#pragma once

#include <igameevents.h>

#include <map>
#include <string>


#include "globals.h"
#include "script_engine.h"

namespace vspdotnet {
class CCallback;
class PluginFunction;
}

struct EventHook {
  EventHook() {
    PreHook = nullptr;
    PostHook = nullptr;
  }
  vspdotnet::CCallback* PreHook;
  vspdotnet::CCallback* PostHook;
  std::string name;
};

namespace vspdotnet {

class CEventManager : public GlobalClass,
                      public IGameEventListener2 {
 public:
  CEventManager();
  ~CEventManager();

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
  std::map<std::string, EventHook*> m_hooks_;
};

}  // namespace vspdotnet

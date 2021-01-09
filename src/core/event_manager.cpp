#include "event_manager.h"
#include "callback_manager.h"

#include "log.h"

SH_DECL_HOOK2(IGameEventManager2, FireEvent, SH_NOATTRIB, 0, bool, IGameEvent*,
              bool);

namespace vspdotnet
{

CEventManager::CEventManager() {}

CEventManager::~CEventManager() {}

void CEventManager::OnStartup() {}

void CEventManager::OnAllInitialized() {
  SH_ADD_HOOK(IGameEventManager2, FireEvent, globals::gameeventmanager,
              SH_MEMBER(this, &CEventManager::OnFireEvent), false);
  SH_ADD_HOOK(IGameEventManager2, FireEvent, globals::gameeventmanager,
              SH_MEMBER(this, &CEventManager::OnFireEvent_Post), true);  
}

void CEventManager::OnShutdown() {
  SH_REMOVE_HOOK(IGameEventManager2, FireEvent, globals::gameeventmanager,
                 SH_MEMBER(this, &CEventManager::OnFireEvent), false);
  SH_REMOVE_HOOK(IGameEventManager2, FireEvent, globals::gameeventmanager,
                 SH_MEMBER(this, &CEventManager::OnFireEvent_Post), true);

  globals::gameeventmanager->RemoveListener(this);
}

void CEventManager::FireGameEvent(IGameEvent* event) {}

bool CEventManager::HookEvent(const char* name, CallbackT callback,
                              bool post) {
  EventHook* p_hook;

  if (!globals::gameeventmanager->FindListener(this, name)) {
    if (!globals::gameeventmanager->AddListener(this, name, true)) {
      // Event doesn't exist.
      return false;
    }
  }

  VSPDN_CORE_INFO("Hooking event: {0} with callback pointer: {1}", name,
                  (void*)callback);

  auto search = m_hooks_.find(name);
  // If hook struct is not found
  if (search == m_hooks_.end()) {
    p_hook = new EventHook();

    if (post) {
      p_hook->PostHook = globals::callback_manager.CreateCallback(name);
      p_hook->PostHook->AddListener(callback);
    } else {
      p_hook->PreHook = globals::callback_manager.CreateCallback(name);
      p_hook->PreHook->AddListener(callback);
    }

    p_hook->name = std::string(name);

    m_hooks_[name] = p_hook;

    return true;
  } else {
    p_hook = search->second;
  }

  if (post) {
    if (!p_hook->PostHook) {
      p_hook->PostHook = globals::callback_manager.CreateCallback("");
    }

    p_hook->PostHook->AddListener(callback);
  } else {
    if (!p_hook->PreHook) {
      p_hook->PreHook = globals::callback_manager.CreateCallback("");;
    }

    p_hook->PreHook->AddListener(callback);
  }

  return true;
}

bool CEventManager::UnhookEvent(const char* name, CallbackT callback,
                                bool post) {
  EventHook* p_hook;
  CCallback* p_callback;

  auto search = m_hooks_.find(name);
  if (search == m_hooks_.end()) {
    return false;
  }

  p_hook = search->second;

  if (post) {
    p_callback = p_hook->PostHook;
  } else {
    p_callback = p_hook->PreHook;
  }

  // Remove from function list
  if (p_callback == nullptr) {
    return false;
  }

    p_callback = nullptr;
    if (post) {
      p_hook->PostHook = nullptr;
    } else {
      p_hook->PreHook = nullptr;
    }

  // TODO: Clean up callback if theres noone left attached.

  VSPDN_CORE_INFO("Unhooking event: {0} with callback pointer: {1}", name, (void*)callback);

  return true;
}

bool CEventManager::OnFireEvent(IGameEvent* pEvent, bool bDontBroadcast) {
  EventHook* p_hook;
  const char* name;

  if (!pEvent) {
    RETURN_META_VALUE(MRES_IGNORED, false);
  }

  name = pEvent->GetName();

  auto search = m_hooks_.find(name);
  if (search != m_hooks_.end()) {
    auto p_callback = search->second->PreHook;

    if (p_callback) {
      VSPDN_CORE_INFO("Pushing event `{0}` pointer: {1}", name, (void*)pEvent);
      p_callback->ScriptContext().Reset();
      p_callback->ScriptContext().SetArgument(0, pEvent);
      p_callback->Execute();
    }
  }

  RETURN_META_VALUE(MRES_IGNORED, true);

  return true;
}

bool CEventManager::OnFireEvent_Post(IGameEvent* pEvent, bool bDontBroadcast) {
  return true;
  /*RETURN_META_VALUE(MRES_IGNORED, true);*/
}
}  // namespace vspdotnet
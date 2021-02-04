#pragma once

namespace vspdotnet
{
class GlobalClass {
 public:
  virtual ~GlobalClass() = default;

  GlobalClass() {
    m_pGlobalClassNext = GlobalClass::head;
    GlobalClass::head = this;
  }

 public:
  virtual void OnStartup() {}
  virtual void OnShutdown() {}
  virtual void OnAllInitialized() {}
  virtual void OnAllInitialized_Post() {}
  virtual void OnLevelChange(const char *mapName) {}
  virtual void OnLevelEnd() {}

 public:
  GlobalClass *m_pGlobalClassNext;
  static GlobalClass *head;
};
}

#define CALL_GLOBAL_LISTENER(func) \
   GlobalClass *pBase = GlobalClass::head;\
  pBase = GlobalClass::head; \
  while (pBase) { \
    pBase->func; \
    pBase = pBase->m_pGlobalClassNext; \
}
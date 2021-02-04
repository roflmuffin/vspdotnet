#include <map>
#include <vector>

#include "hook.h"
#include "core/autonative.h"
#include "core/global_listener.h"

class CHook;

namespace vspdotnet {
  
class ValveFunction;

struct EntityHook
{
  int index;
  CallbackT callback;
};

class HookManager : public GlobalClass
{
public:
  void OnAllInitialized() override;
  void AddHook(ValveFunction* function, int index, CallbackT callback, bool is_post);
  void Unhook(ValveFunction* function, int index, CallbackT callback, bool is_post);
  bool HookHandler(HookType_t, CHook*);
 private:
  std::map<CHook*, std::map<HookType_t, std::vector<EntityHook>>> m_callback_map;

  // Hack to copy the this pointer on windows.
  std::unordered_map<void*, void*> m_ecx_addresses;
};}

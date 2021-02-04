#include "core/hook_manager.h"


#include "entity.h"
#include "function.h"

namespace vspdotnet
{
void HookManager::OnAllInitialized()
{
}

static bool InternalHookHandler(HookType_t type, CHook* hook)
{
  return globals::hook_manager.HookHandler(type, hook);
}

bool HookManager::HookHandler(HookType_t type, CHook* hook)
{
  auto callbacks = m_callback_map[hook][type];

  if (callbacks.empty()) return false;

  if (type == HOOKTYPE_POST)
  {
    switch (hook->m_pCallingConvention->m_returnType)
    {
    }
  }

  CBaseEntityWrapper* entity = nullptr;
  if (type == HOOKTYPE_PRE)
  {
    entity = hook->GetArgument<CBaseEntityWrapper*>(0);
    m_ecx_addresses[hook->GetArgumentAddress(0)] = entity;
  }
  else
  {
    entity = reinterpret_cast<CBaseEntityWrapper*>(m_ecx_addresses[hook->GetArgumentAddress(0)]);
    m_ecx_addresses.erase(hook->GetArgumentAddress(0));
  }

  bool override_result = false;

  for (auto entity_hook : callbacks)
  {
    if (entity_hook.index != entity->GetIndex()) continue;

    auto nativeContext = fxNativeContext{};
    auto context = ScriptContextRaw(nativeContext);

    for (int i = 0; i < hook->m_pCallingConvention->m_vecArgTypes.size(); i++)
    {
      auto arg = hook->m_pCallingConvention->m_vecArgTypes[i];

      switch (arg)
      {
        case DATA_TYPE_BOOL:
          context.Push(hook->GetArgument<bool>(i));
          break;
        case DATA_TYPE_CHAR:
          context.Push(hook->GetArgument<char>(i));
          break;
        case DATA_TYPE_UCHAR:
          context.Push(hook->GetArgument<unsigned>(i));
          break;
        case DATA_TYPE_SHORT:
          context.Push(hook->GetArgument<short>(i));
          break;
        case DATA_TYPE_USHORT:
          context.Push(hook->GetArgument<unsigned short>(i));
          break;
        case DATA_TYPE_INT:
          context.Push(hook->GetArgument<int>(i));
          break;
        case DATA_TYPE_UINT:
          context.Push(hook->GetArgument<unsigned int>(i));
          break;
        case DATA_TYPE_LONG:
          context.Push(hook->GetArgument<long>(i));
          break;
        case DATA_TYPE_ULONG:
          context.Push(hook->GetArgument<unsigned long>(i));
          break;
        case DATA_TYPE_LONG_LONG:
          context.Push(hook->GetArgument<long long>(i));
          break;
        case DATA_TYPE_ULONG_LONG:
          context.Push(hook->GetArgument<unsigned long long>(i));
          break;
        case DATA_TYPE_FLOAT:
          context.Push(hook->GetArgument<float>(i));
          break;
        case DATA_TYPE_DOUBLE:
          context.Push(hook->GetArgument<double>(i));
          break;
        case DATA_TYPE_POINTER:
          {
            if (i == 0)
              context.Push(entity);
            else
              context.Push(hook->GetArgument<unsigned long>(i));
            break;
          }
        case DATA_TYPE_STRING:
          context.Push(hook->GetArgument<const char*>(i));
          break;
        default:
          VSPDN_CORE_ERROR("Unknown parameter type found in hook");
          break;
      }
    }

    if (type == HOOKTYPE_POST)
    {
      auto return_type = hook->m_pCallingConvention->m_returnType;
      switch (return_type)
      {
        case DATA_TYPE_VOID:
          break;
        case DATA_TYPE_BOOL:
          context.Push(hook->GetReturnValue<bool>());
          break;
        case DATA_TYPE_CHAR:
          context.Push(hook->GetReturnValue<char>());
          break;
        case DATA_TYPE_UCHAR:
          context.Push(hook->GetReturnValue<unsigned>());
          break;
        case DATA_TYPE_SHORT:
          context.Push(hook->GetReturnValue<short>());
          break;
        case DATA_TYPE_USHORT:
          context.Push(hook->GetReturnValue<unsigned short>());
          break;
        case DATA_TYPE_INT:
          context.Push(hook->GetReturnValue<int>());
          break;
        case DATA_TYPE_UINT:
          context.Push(hook->GetReturnValue<unsigned int>());
          break;
        case DATA_TYPE_LONG:
          context.Push(hook->GetReturnValue<long>());
          break;
        case DATA_TYPE_ULONG:
          context.Push(hook->GetReturnValue<unsigned long>());
          break;
        case DATA_TYPE_LONG_LONG:
          context.Push(hook->GetReturnValue<long long>());
          break;
        case DATA_TYPE_ULONG_LONG:
          context.Push(hook->GetReturnValue<unsigned long long>());
          break;
        case DATA_TYPE_FLOAT:
          context.Push(hook->GetReturnValue<float>());
          break;
        case DATA_TYPE_DOUBLE:
          context.Push(hook->GetReturnValue<double>());
          break;
        case DATA_TYPE_POINTER:
          context.Push(hook->GetReturnValue<unsigned long>());
          break;
        case DATA_TYPE_STRING:
          context.Push(hook->GetReturnValue<const char*>());
          break;
        default:
          VSPDN_CORE_ERROR("Unknown return type found in hook");
          break;
      }
    }

    entity_hook.callback(&nativeContext);

    if (context.GetResult<int>() > 0)
    {
      override_result = true;
      switch (hook->m_pCallingConvention->m_returnType)
      {
        case DATA_TYPE_VOID:
          break;
        case DATA_TYPE_BOOL:
          hook->SetReturnValue(context.GetResult<bool>());
          break;
        case DATA_TYPE_CHAR:
          hook->SetReturnValue(context.GetResult<char>());
          break;
        case DATA_TYPE_UCHAR:
          hook->SetReturnValue(context.GetResult<unsigned>());
          break;
        case DATA_TYPE_SHORT:
          hook->SetReturnValue(context.GetResult<short>());
          break;
        case DATA_TYPE_USHORT:
          hook->SetReturnValue(context.GetResult<unsigned short>());
          break;
        case DATA_TYPE_INT:
          hook->SetReturnValue(context.GetResult<int>());
          break;
        case DATA_TYPE_UINT:
          hook->SetReturnValue(context.GetResult<unsigned int>());
          break;
        case DATA_TYPE_LONG:
          hook->SetReturnValue(context.GetResult<long>());
          break;
        case DATA_TYPE_ULONG:
          hook->SetReturnValue(context.GetResult<unsigned long>());
          break;
        case DATA_TYPE_LONG_LONG:
          hook->SetReturnValue(context.GetResult<long long>());
          break;
        case DATA_TYPE_ULONG_LONG:
          hook->SetReturnValue(context.GetResult<unsigned long long>());
          break;
        case DATA_TYPE_FLOAT:
          hook->SetReturnValue(context.GetResult<float>());
          break;
        case DATA_TYPE_DOUBLE:
          hook->SetReturnValue(context.GetResult<double>());
          break;
        case DATA_TYPE_POINTER:
          {
            hook->SetReturnValue(context.GetResult<void*>());
            break;
          case DATA_TYPE_STRING:
            hook->SetReturnValue(context.GetResult<const char*>());
            break;
          }
        default:
          VSPDN_CORE_ERROR("Unknown return type found in hook");
          break;
      }
    }
  }

  return override_result;
}

void HookManager::AddHook(ValveFunction* function, int index, CallbackT callback, bool is_post)
{
  auto type = is_post ? HOOKTYPE_POST : HOOKTYPE_PRE;
  auto hook = function->AddHook(type, &InternalHookHandler);

  m_callback_map[hook][type].push_back({index, callback});
}

void HookManager::Unhook(ValveFunction* function, int index, CallbackT callback, bool is_post)
{
  auto type = is_post ? HOOKTYPE_POST : HOOKTYPE_PRE;

  if (!function->IsHooked()) return;

  auto hook = function->GetHook();

  EntityHook entity_hook = {index, callback};
  auto it = std::remove_if(m_callback_map[hook][type].begin(), m_callback_map[hook][type].end(),
                           [entity_hook](EntityHook hook)
                           {
                             return hook.callback == entity_hook.callback &&
                                    hook.index == entity_hook.index;
                           });

  bool success;
  if ((success = it != m_callback_map[hook][type].end()))
    m_callback_map[hook][type].erase(it, m_callback_map[hook][type].end());
}
}

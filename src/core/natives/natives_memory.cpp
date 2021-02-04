#include <core/datamaps.h>

#include <ios>
#include <sstream>


#include "core/autonative.h"
#include "core/entity.h"
#include "core/function.h"
#include "core/hook_manager.h"
#include "core/script_engine.h"
#include "core/sig_scanner.h"

namespace vspdotnet
{
std::vector<ValveFunction*> m_managed_ptrs;

ValveFunction* CreateVirtualFunctionBySignature(ScriptContext& script_context)
{
  auto ptr = script_context.GetArgument<unsigned long>(0);
  auto binary_name = script_context.GetArgument<const char*>(1);
  auto signature_hex_string = script_context.GetArgument<const char*>(2);
  auto num_arguments = script_context.GetArgument<int>(3);
  auto return_type = script_context.GetArgument<DataType_t>(4);

  std::istringstream hex_chars_stream(signature_hex_string);
  std::vector<unsigned char> bytes;

  unsigned int c;
  while (hex_chars_stream >> std::hex >> c)
  {
    bytes.push_back(c);
  }

  CBinaryFile* serverBinary = vspdotnet::FindBinary(binary_name, false);
  auto function_addr = serverBinary->FindSignature(bytes.data(), bytes.size());

  if (function_addr == ULONG_MAX)
  {
    script_context.ThrowNativeError(
        "Error finding method with signature %s in binary %s",
        signature_hex_string, binary_name);
    return nullptr;
  }

  auto args = std::vector<DataType_t>();
  for (int i = 0; i < num_arguments; i++)
  {
    args.push_back(script_context.GetArgument<DataType_t>(5 + i));
  }

  auto function =
      new ValveFunction(function_addr, CONV_THISCALL, args, return_type);
  function->SetSignature(signature_hex_string);

  m_managed_ptrs.push_back(function);
  return function;
}

ValveFunction* CreateVirtualFunction(ScriptContext& script_context)
{
  auto ptr = script_context.GetArgument<unsigned long>(0);
  auto vtable_offset = script_context.GetArgument<int>(1);
  auto num_arguments = script_context.GetArgument<int>(2);
  auto return_type = script_context.GetArgument<DataType_t>(3);

  void** vtable = *(void***)ptr;
  if (!vtable)
  {
    script_context.ThrowNativeError(
        "Failed to get the virtual function table.");
    return nullptr;
  }

  auto function_addr = (unsigned long)vtable[vtable_offset];

  auto args = std::vector<DataType_t>();
  for (int i = 0; i < num_arguments; i++)
  {
    args.push_back(script_context.GetArgument<DataType_t>(4 + i));
  }

  auto function =
      new ValveFunction(function_addr, CONV_THISCALL, args, return_type);
  function->SetOffset(vtable_offset);

  m_managed_ptrs.push_back(function);
  return function;
}

void ExecuteVirtualFunction(ScriptContext& script_context)
{
  auto function = script_context.GetArgument<ValveFunction*>(0);

  if (!function)
  {
    script_context.ThrowNativeError("Invalid function pointer");
    return;
  }

  function->Call(script_context, 1);
}

void HookFunction(ScriptContext& script_context)
{
  auto function = script_context.GetArgument<ValveFunction*>(0);
  auto entity_index = script_context.GetArgument<int>(1);
  auto post = script_context.GetArgument<bool>(2);
  auto callback = script_context.GetArgument<CallbackT>(3);

  if (!function)
  {
    script_context.ThrowNativeError("Invalid function pointer");
    return;
  }

  globals::hook_manager.AddHook(function, entity_index, callback, post);
}

void UnhookFunction(ScriptContext& script_context) {
  auto function = script_context.GetArgument<ValveFunction*>(0);
  auto entity_index = script_context.GetArgument<int>(1);
  auto post = script_context.GetArgument<bool>(2);
  auto callback = script_context.GetArgument<CallbackT>(3);

  if (!function) {
    script_context.ThrowNativeError("Invalid function pointer");
    return;
  }

  globals::hook_manager.Unhook(function, entity_index, callback, post);
}

REGISTER_NATIVES(memory, {
                 ScriptEngine::RegisterNativeHandler("CREATE_VIRTUAL_FUNCTION",
                   CreateVirtualFunction);
                 ScriptEngine::RegisterNativeHandler("CREATE_VIRTUAL_FUNCTION_BY_SIGNATURE",
                   CreateVirtualFunctionBySignature);
                 ScriptEngine::RegisterNativeHandler("EXECUTE_VIRTUAL_FUNCTION",
                   ExecuteVirtualFunction);
  ScriptEngine::RegisterNativeHandler("HOOK_FUNCTION", HookFunction);
  ScriptEngine::RegisterNativeHandler("UNHOOK_FUNCTION", UnhookFunction);
                 })
}

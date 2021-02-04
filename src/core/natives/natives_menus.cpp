#include "core/autonative.h"
#include "core/menu_manager.h"
#include "core/script_engine.h"
#include "core/user_message_manager.h"

namespace vspdotnet
{

Menu *CreateMenu(ScriptContext& script_context)
{
  auto title = script_context.GetArgument<const char *>(0);

  return globals::menu_manager.CreateMenu(title);
}

MenuItem* AddMenuOption(ScriptContext &script_context)
{
  auto menu = script_context.GetArgument<Menu *>(0);
  auto display = script_context.GetArgument<const char *>(1);
  auto value = script_context.GetArgument<const char *>(2);
  auto flags = script_context.GetArgument<int>(3);

  auto menu_item = new MenuItem(display, value, flags);
  menu->AddMenuItem(menu_item);
  return menu_item;
}

void ShowMenu(ScriptContext &script_context) {
  auto index = script_context.GetArgument<int>(0);
  auto menu = script_context.GetArgument<Menu *>(1);

  globals::menu_manager.ShowMenu(index, menu);
}

void AddMenuHandler(ScriptContext &script_context) {
  auto menu = script_context.GetArgument<Menu *>(0);
  auto callback = script_context.GetArgument<CallbackT>(1);

  if (menu) {
    menu->GetCallback()->AddListener(callback);
  }
}

REGISTER_NATIVES(menus, {
  ScriptEngine::RegisterNativeHandler("SHOW_MENU", ShowMenu);
  ScriptEngine::RegisterNativeHandler("CREATE_MENU", CreateMenu);
  ScriptEngine::RegisterNativeHandler("ADD_MENU_OPTION", AddMenuOption);
  ScriptEngine::RegisterNativeHandler("ADD_MENU_HANDLER", AddMenuHandler);
})
}
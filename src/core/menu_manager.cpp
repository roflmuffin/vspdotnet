#include "core/menu_manager.h"

#include <public/tier1/convar.h>

#include <algorithm>
#include <cstring>

#include "core/callback_manager.h"
#include "core/globals.h"
#include "core/log.h"
#include "core/user_message_manager.h"

namespace vspdotnet {
Menu::Menu(std::string title) : m_title(title) {
  m_callback = globals::callback_manager.CreateCallback(title.c_str());
}

void Menu::AddMenuItem(MenuItem* item) { m_menuitems.push_back(item); }

std::string Menu::GetMenuString() {
  std::string buffer;

  buffer.append(m_title + "\n \n");

  int keyOffset = 1;

  if (RequiresPrevPageButton()) {
    buffer.append("->1. Prev\n");
    m_keybits |= (1 << 1 - 1);
    keyOffset++;
  }

  m_num_displayed = 0;

  for (int i = m_current_offset;
       i < std::min(m_current_offset + MenuItemsPerPage(), (int)m_menuitems.size()); i++) {
    auto option = m_menuitems[i];
    if (option->m_enabled) {
      buffer.append("->");
    }

    buffer.append(std::to_string(keyOffset++) + ". " + option->m_display + "\n");
    m_keybits |= (1 << (keyOffset - 2));
    m_num_displayed++;
  }

  if (RequiresNextPageButton()) {
    buffer.append("->8. Next\n");
    m_keybits |= (1 << 8 - 1);
  }

  buffer.append("->9. Close\n");
  m_keybits |= (1 << 9 - 1);

  return buffer;
}

bool Menu::RequiresPrevPageButton() const { return m_page > 0; }

bool Menu::RequiresNextPageButton() const {
  return (m_current_offset + m_num_per_page) < m_menuitems.size();
}

int Menu::MenuItemsPerPage() const {
  return m_num_per_page + 2 - (RequiresNextPageButton() ? 1 : 0) -
         (RequiresPrevPageButton() ? 1 : 0);
}

void Menu::PrevPage() {
  m_page--;
  m_current_offset = m_prev_page_offsets.top();
  m_prev_page_offsets.pop();
}

void Menu::NextPage() {
  m_prev_page_offsets.push(m_current_offset);
  m_current_offset = m_current_offset + MenuItemsPerPage();
  m_page++;
}

bool Menu::HandleKeyPress(int client, unsigned key) {
  if (key == 8 && RequiresNextPageButton()) {
    NextPage();
    return false;
  }

  if (key == 1 && RequiresPrevPageButton()) {
    PrevPage();
    return false;
  }

  if (key == 9) {
    return true;
  }

  auto offset = m_current_offset;
  auto desired_value = key;

  if (RequiresPrevPageButton()) desired_value = key - 1;

  auto menu_item_index = m_current_offset + desired_value - 1;

  auto menu_option = m_menuitems[m_current_offset + desired_value - 1];

  VSPDN_CORE_INFO("User selected menu option with display {0} and value {1}",
                  menu_option->m_display, menu_option->m_value);

  globals::menu_manager.SetActiveMenu(client, nullptr);
  Reset();

  m_callback->ScriptContext().Push(client);
  m_callback->ScriptContext().Push(menu_option->m_display.c_str());
  m_callback->ScriptContext().Push(menu_option->m_value.c_str());
  m_callback->Execute();

  return true;
}

void Menu::Reset() {
  m_current_offset = 0;
  m_page = 0;
  m_keybits = 0;
  m_prev_page_offsets = std::stack<int>();
}

MenuManager::MenuManager() {}

bool MenuManager::OnClientCommand(int client, const char* cmdname,
                                   const CCommand& cmd) {
  if (strcmp(cmdname, "menuselect") == 0) {
    if (m_active_menus_[client] == nullptr) return false;

    int arg = atoi(cmd.Arg(1));
    HandleKeyPress(client, arg);
    return true;
  }

  return false;
}

void MenuManager::HandleKeyPress(int client, unsigned key) {
  auto menu = m_active_menus_[client];

  if (menu->HandleKeyPress(client, key)) {
    // If their selection caused an action to take place, we clear the active menu.
    /*SetActiveMenu(client, nullptr);
    menu->Reset();*/
  } else {
    ShowMenu(client, menu);
  }
}

Menu* MenuManager::CreateMenu(std::string title) {
  auto* menu = new Menu(title);
  m_menus.push_back(menu);

  return menu;
}

void MenuManager::SetActiveMenu(int client, Menu* menu) {
  m_active_menus_[client] = menu;
}

void MenuManager::ShowMenu(int client, Menu* menu) {
  globals::user_message_manager.ShowMenu(client, menu->GetKeyBits(), 100,
                                         menu->GetMenuString().c_str());
  SetActiveMenu(client, menu);
}

}  // namespace vspdotnet
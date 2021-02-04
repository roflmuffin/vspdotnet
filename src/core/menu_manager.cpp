#include "core/menu_manager.h"

#include <public/tier1/convar.h>

#include <algorithm>
#include <cstring>

#include "core/callback_manager.h"
#include "core/globals.h"
#include "core/log.h"
#include "core/user_message_manager.h"

namespace vspdotnet {
Menu::Menu(std::string title) : m_title_(title) {
  m_callback_ = globals::callback_manager.CreateCallback(title.c_str());
}

void Menu::AddMenuItem(MenuItem* item) { m_menuitems_.push_back(item); }

std::string Menu::GetMenuString() {
  std::string buffer;

  buffer.append(m_title_ + "\n \n");

  int keyOffset = 1;

  if (RequiresPrevPageButton()) {
    buffer.append("->1. Prev\n");
    m_keybits_ |= (1 << 1 - 1);
    keyOffset++;
  }

  m_num_displayed_ = 0;

  for (int i = m_current_offset_;
       i < std::min(m_current_offset_ + MenuItemsPerPage(), (int)m_menuitems_.size()); i++) {
    auto option = m_menuitems_[i];
    if (option->m_enabled_) {
      buffer.append("->");
    }

    buffer.append(std::to_string(keyOffset++) + ". " + option->m_display_ + "\n");
    m_keybits_ |= (1 << (keyOffset - 2));
    m_num_displayed_++;
  }

  if (RequiresNextPageButton()) {
    buffer.append("->8. Next\n");
    m_keybits_ |= (1 << 8 - 1);
  }

  buffer.append("->9. Close\n");
  m_keybits_ |= (1 << 9 - 1);

  return buffer;
}

bool Menu::RequiresPrevPageButton() const { return m_page_ > 0; }

bool Menu::RequiresNextPageButton() const {
  return (m_current_offset_ + m_num_per_page_) < m_menuitems_.size();
}

int Menu::MenuItemsPerPage() const {
  return m_num_per_page_ + 2 - (RequiresNextPageButton() ? 1 : 0) -
         (RequiresPrevPageButton() ? 1 : 0);
}

void Menu::PrevPage() {
  m_page_--;
  m_current_offset_ = m_prev_page_offsets.top();
  m_prev_page_offsets.pop();
}

void Menu::NextPage() {
  m_prev_page_offsets.push(m_current_offset_);
  m_current_offset_ = m_current_offset_ + MenuItemsPerPage();
  m_page_++;
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

  auto offset = m_current_offset_;
  auto desired_value = key;

  if (RequiresPrevPageButton()) desired_value = key - 1;

  auto menu_item_index = m_current_offset_ + desired_value - 1;

  auto menu_option = m_menuitems_[m_current_offset_ + desired_value - 1];

  VSPDN_CORE_INFO("User selected menu option with display {0} and value {1}",
                  menu_option->m_display_, menu_option->m_value_);

  globals::menu_manager.SetActiveMenu(client, nullptr);
  Reset();

  m_callback_->ScriptContext().Push(client);
  m_callback_->ScriptContext().Push(menu_option->m_display_.c_str());
  m_callback_->ScriptContext().Push(menu_option->m_value_.c_str());
  m_callback_->Execute();

  return true;
}

void Menu::Reset() {
  m_current_offset_ = 0;
  m_page_ = 0;
  m_keybits_ = 0;
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
  m_menus_.push_back(menu);

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
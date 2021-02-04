#pragma once

#include <stack>
#include <string>
#include <vector>

#include "core/callback_manager.h"

class CCommand;

namespace vspdotnet
{
#define MENUOPTION_DISABLED (1 << 0)

class MenuItem 
{
  friend class Menu;

 public:
  MenuItem(std::string display, std::string value, int flags) : m_display_(display), m_value_(value), m_enabled_(true)
  {
    if (flags & MENUOPTION_DISABLED)
    {
      m_enabled_ = false;
    }
  }

 private:
  std::string m_display_;
  std::string m_value_;
  bool m_enabled_;
};

class Menu
{
public:
  Menu(std::string title);
 void AddMenuItem(MenuItem* item);
 void RemoveMenuItem(MenuItem* item);
  bool RequiresPrevPageButton() const;
  bool RequiresNextPageButton() const;
  int MenuItemsPerPage() const;
  void NextPage();
  void PrevPage();
  bool HandleKeyPress(int client, unsigned int key);
  void Reset();

  ScriptCallback* GetCallback() { return m_callback_; }

  int GetKeyBits() { return m_keybits_; }
  std::string GetMenuString();

private:
 std::vector<MenuItem*> m_menuitems_;
 std::string m_title_;
 ScriptCallback* m_callback_;

 int m_keybits_ = 0;

 int m_page_ = 0;
 int m_current_offset_ = 0;
 int m_num_displayed_ = 0;
 int m_num_per_page_ = 6;

 std::stack<int> m_prev_page_offsets;

};

class MenuManager
{
 public:
  MenuManager();
  bool OnClientCommand(int client, const char* cmdname, const CCommand& cmd);
  void HandleKeyPress(int client, unsigned int key);
  Menu* CreateMenu(std::string title);
  void SetActiveMenu(int client, Menu* menu);
  void ShowMenu(int client, Menu* menu);

 private:
  std::vector<Menu*> m_menus_;
  Menu* m_active_menus_[65];
};
}

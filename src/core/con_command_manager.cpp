#include "core/con_command_manager.h"

#undef clamp
#include <eiface.h>

#include <algorithm>

#include "core/callback_manager.h"
#include "sourcehook/sourcehook.h"
#include "utilities/conversions.h"
#include "entity.h"

namespace vspdotnet {

SH_DECL_HOOK1_void(ConCommand, Dispatch, SH_NOATTRIB, false, const CCommand&);
SH_DECL_HOOK1_void(IServerGameClients, SetCommandClient, SH_NOATTRIB, false,
                   int);

void ConCommandInfo::HookChange(CallbackT cb) {
  this->callback->AddListener(cb);
}

void ConCommandInfo::UnhookChange(CallbackT cb) {
  if (this->callback && this->callback->GetFunctionCount()) {
    callback->RemoveListener(cb);
  }
}

ConCommandManager::ConCommandManager() : last_command_client_(-1) {}

ConCommandManager::~ConCommandManager() {}

void ConCommandManager::OnAllInitialized() {
  SH_ADD_HOOK(IServerGameClients, SetCommandClient, globals::server_game_clients,
              SH_MEMBER(this, &ConCommandManager::SetCommandClient), false);
}

void ConCommandManager::OnShutdown() {
  SH_REMOVE_HOOK(IServerGameClients, SetCommandClient,
                 globals::server_game_clients,
                 SH_MEMBER(this, &ConCommandManager::SetCommandClient), false);
}

#ifndef SWIG
void CommandCallback(const CCommand& command) {
  globals::con_command_manager.InternalDispatch(
      globals::con_command_manager.GetCommandClient(), &command);
}
#endif

ConCommandInfo* ConCommandManager::AddOrFindCommand(const char* name,
                                                    const char* description,
                                                    bool server_only,
                                                    int flags) {
  ConCommandInfo* p_info = m_cmd_lookup_[std::string(name)];

  if (!p_info) {
    auto found = std::find_if(
        m_cmd_list_.begin(), m_cmd_list_.end(), [&](ConCommandInfo* info) {
          return V_strcasecmp(info->p_cmd->GetName(), name) == 0;
        });
    if (found != m_cmd_list_.end()) {
      return *found;
    }

    p_info = new ConCommandInfo();
    ConCommand* p_cmd = g_pCVar->FindCommand(name);

    if (!p_cmd) {
      if (!description) {
        description = "";
      }

      char* new_name = strdup(name);
      char* new_desc = strdup(description);

      p_cmd = new ConCommand(new_name, CommandCallback, new_desc, flags);
      p_info->sdn = true;
      p_info->callback = globals::callback_manager.CreateCallback(name);
      p_info->server_only = server_only;
    } else {
      p_info->callback = globals::callback_manager.CreateCallback(name);
      p_info->server_only = server_only;

      SH_ADD_HOOK(ConCommand, Dispatch, p_cmd, SH_STATIC(CommandCallback),
                  false);
    }

    if (p_cmd) {
      p_info->p_cmd = p_cmd;

      m_cmd_list_.push_back(p_info);
      m_cmd_lookup_[name] = p_info;
    }

    return p_info;
  }

  return p_info;
}

ConCommandInfo* ConCommandManager::AddCommand(const char* name,
                                              const char* description,
                                              bool server_only, int flags,
                                              CallbackT callback) {
  ConCommandInfo* p_info =
      AddOrFindCommand(name, description, server_only, flags);
  if (!p_info || !p_info->callback) {
    return nullptr;
  }

  p_info->callback->AddListener(callback);

  return p_info;
}

bool ConCommandManager::RemoveCommand(const char* name, CallbackT callback) {
  ConCommandInfo* p_info = m_cmd_lookup_[std::string(name)];
  if (!p_info) return false;

  if (p_info->callback && p_info->callback->GetFunctionCount()) {
    p_info->callback->RemoveListener(callback);
  }

  if (!p_info->callback || p_info->callback->GetFunctionCount() == 0) {
    g_pCVar->UnregisterConCommand(p_info->p_cmd);

    bool success;
    auto it =
        std::remove_if(m_cmd_list_.begin(), m_cmd_list_.end(),
                       [p_info](ConCommandInfo* i) { return p_info == i; });

    if ((success = it != m_cmd_list_.end()))
      m_cmd_list_.erase(it, m_cmd_list_.end());
    if (success) m_cmd_lookup_[std::string(name)] = nullptr;

    return success;
  }

  return true;
}

ConCommandInfo* ConCommandManager::FindCommand(const char* name) {
  ConCommandInfo* p_info = m_cmd_lookup_[std::string(name)];

  if (!p_info) {
    auto found = std::find_if(
        m_cmd_list_.begin(), m_cmd_list_.end(), [&](ConCommandInfo* info) {
          return V_strcasecmp(info->p_cmd->GetName(), name) == 0;
        });
    if (found != m_cmd_list_.end()) {
      return *found;
    }

    ConCommand* p_cmd = g_pCVar->FindCommand(name);
    if (!p_cmd) return nullptr;

    p_info = new ConCommandInfo();

    p_info->callback = globals::callback_manager.CreateCallback(name);
    p_info->server_only = false;

    SH_ADD_HOOK(ConCommand, Dispatch, p_cmd, SH_STATIC(CommandCallback), false);

    if (p_cmd) {
      p_info->p_cmd = p_cmd;

      m_cmd_list_.push_back(p_info);
      m_cmd_lookup_[name] = p_info;
    }

    return p_info;
  }

  return p_info;
}

int ConCommandManager::GetCommandClient() { return last_command_client_; }

void ConCommandManager::SetCommandClient(int client) {
  last_command_client_ = client + 1;
}

bool ConCommandManager::InternalDispatch(int client, const CCommand* args) {
  if (client) {
    auto entity = reinterpret_cast<CBaseEntityWrapper*>(ExcBaseEntityFromIndex(client));
    if (!entity || !entity->IsPlayer()) return false;
  }

  const char* cmd = args->Arg(0);

  ConCommandInfo* p_info = m_cmd_lookup_[cmd];
  if (p_info == nullptr) {
    if (client == 0 && !globals::engine->IsDedicatedServer()) return false;

    auto found = std::find_if(
        m_cmd_list_.begin(), m_cmd_list_.end(), [cmd](ConCommandInfo* info) {
          return V_strcasecmp(info->p_cmd->GetName(), cmd);
        });
    if (found == m_cmd_list_.end()) {
      return false;
    }

    p_info = *found;
  }

  int argc = args->ArgC() - 1;

  int realClient = client;

  if (p_info->callback) {
    p_info->callback->ScriptContext().Reset();
    p_info->callback->ScriptContext().SetArgument(0, realClient);
    p_info->callback->ScriptContext().SetArgument(1, args);
    p_info->callback->Execute();
  }

  return true;
}

bool ConCommandManager::DispatchClientCommand(int client, const char* cmd,
                                              const CCommand* args) {
  ConCommandInfo* p_info = m_cmd_lookup_[cmd];
  if (p_info == nullptr) {
    auto found =
        std::find_if(m_cmd_list_.begin(), m_cmd_list_.end(),
                     [&](const ConCommandInfo* info) {
                       return V_strcasecmp(info->p_cmd->GetName(), cmd) == 0;
                     });
    if (found == m_cmd_list_.end()) {
      return false;
    }

    p_info = *found;
  }

  if (p_info->server_only) return false;

  if (p_info->callback) {
    p_info->callback->ScriptContext().Reset();
    p_info->callback->ScriptContext().Push(client);
    p_info->callback->ScriptContext().Push(reinterpret_cast<int>(args));
    p_info->callback->Execute();

    return true;
  }

  return false;
}
}  // namespace vspdotnet
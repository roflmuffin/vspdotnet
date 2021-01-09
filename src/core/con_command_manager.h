#pragma once

#include <map>
#include <vector>

#include "core/globals.h"
#include "core/script_engine.h"

// Required to access convar methods :(
#define protected public
#define private public
#include <tier1/convar.h>
#undef protected
#undef private

namespace vspdotnet {
class CCallback;

class ConCommandInfo {
  friend class ConCommandManager;

 public:
  ConCommandInfo() {}

 public:
  void HookChange(CallbackT callback);
  void UnhookChange(CallbackT callback);

 private:
  bool sdn;
  ConCommand* p_cmd;
  CCallback* callback;
  bool server_only;
};

class ConCommandManager : public GlobalClass {
  friend class ConCommandInfo;
  friend void CommandCallback(const CCommand& command);

 public:
  ConCommandManager();
  ~ConCommandManager();
  void OnAllInitialized() override;
  void OnShutdown() override;
  ConCommandInfo* AddOrFindCommand(const char* name, const char* description,
                                   bool server_only, int flags);
  bool DispatchClientCommand(int client, const char* cmd, const CCommand* args);

 public:
  ConCommandInfo* AddCommand(const char* name, const char* description,
                             bool server_only, int flags, CallbackT callback);
  bool RemoveCommand(const char* name, CallbackT callback);
  ConCommandInfo* FindCommand(const char* name);

 private:
  int GetCommandClient();
  void SetCommandClient(int client);
  bool InternalDispatch(int client, const CCommand* args);

 private:
  int last_command_client_;
  std::vector<ConCommandInfo*> m_cmd_list_;
  std::map<std::string, ConCommandInfo*> m_cmd_lookup_;
};

}  // namespace vspdotnet
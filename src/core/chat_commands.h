#include "core/global_listener.h"

class CCommand;

namespace vspdotnet {
class ChatCommands : public GlobalClass
{
public:
  ChatCommands() {}

  void OnAllInitialized() override;
  bool OnSayCommandPre(int client, CCommand* args);
  bool OnSayCommandPost(int client, CCommand* args);
 private:
  bool m_execute_in_post = false;
  const char* m_command_to_execute;
  char* m_args_backup_;
};
}  // namespace vspdotnet
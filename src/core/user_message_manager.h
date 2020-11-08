/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

#pragma once

#include <public/irecipientfilter.h>
#include <public/tier1/utlvector.h>

#include "core/globals.h"
#include "core/global_listener.h"

namespace google {
namespace protobuf {
class Message;
}
}  // namespace google

namespace vspdotnet {

class CustomRecipientFilter : public IRecipientFilter {
 public:
  CustomRecipientFilter();
  ~CustomRecipientFilter();

 public:  // IRecipientFilter
  bool IsReliable() const override;
  bool IsInitMessage() const override;
  int GetRecipientCount() const override;
  int GetRecipientIndex(int slot) const override;
  void MakeInitMessage(void);
  void MakeReliable(void);
  void AddPlayer(int index);
  void RemovePlayer(int index);
  void AddAllPlayers();

 public:
  void Reset();

 private:
  bool m_is_reliable;
  bool m_is_init_message;
  CUtlVector<int> m_recipients;
};


class UserMessageManager : public GlobalClass {
 public:
  UserMessageManager();
  virtual ~UserMessageManager();
  void OnShutdown() override;
  void OnAllInitialized() override;
  void SendMessageToChat(int client, const char* message);
  void SendHintMessage(int client, const char* message);
  void SendCenterMessage(int client, const char* message);
  void OnSendUserMessage_Pre(IRecipientFilter& filter, int msg_type,
                             const google::protobuf::Message& msg);
  void OnSendUserMessage_Post(IRecipientFilter& filter, int msg_type,
                              const google::protobuf::Message& msg);
  void ShowMenu(int client, int keys, int time, const char* body);

 private:
  int m_text_msg_id;
  int m_hint_text_msg_id;
  int m_say_text_msg_id;
  int m_menu_msg_id;
};
}
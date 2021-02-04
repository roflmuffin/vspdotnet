#include "core/user_message_manager.h"

#include <game/shared/shareddefs.h>
#include <game/shared/csgo/protobuf/cstrike15_usermessages.pb.h>
#include <game/shared/csgo/protobuf/cstrike15_usermessage_helpers.h>
#include <google/protobuf/message.h>
#include <public/eiface.h>
#include <sourcehook/sourcehook.h>

#include "core/globals.h"


SH_DECL_HOOK3_void(IVEngineServer, SendUserMessage, SH_NOATTRIB, 0,
                   IRecipientFilter&, int, const google::protobuf::Message&);

namespace vspdotnet {
UserMessageManager::UserMessageManager() {}

UserMessageManager::~UserMessageManager() {}

void UserMessageManager::OnAllInitialized() {
  m_text_msg_id = g_Cstrike15UsermessageHelpers.GetIndex("TextMsg");
  m_hint_text_msg_id = g_Cstrike15UsermessageHelpers.GetIndex("HintText");
  m_say_text_msg_id = g_Cstrike15UsermessageHelpers.GetIndex("SayText");
  m_menu_msg_id = g_Cstrike15UsermessageHelpers.GetIndex("ShowMenu");

  /*SH_ADD_HOOK(IVEngineServer, SendUserMessage, vspdotnet::globals::engine,
              SH_MEMBER(this, &UserMessageManager::OnSendUserMessage_Pre),
              false);
  SH_ADD_HOOK(IVEngineServer, SendUserMessage, vspdotnet::globals::engine,
              SH_MEMBER(this, &UserMessageManager::OnSendUserMessage_Post),
              true);*/
}

void UserMessageManager::OnShutdown() {
  /*SH_REMOVE_HOOK(IVEngineServer, SendUserMessage, vspdotnet::globals::engine,
                 SH_MEMBER(this, &UserMessageManager::OnSendUserMessage_Pre),
                 false);
  SH_REMOVE_HOOK(IVEngineServer, SendUserMessage, vspdotnet::globals::engine,
                 SH_MEMBER(this, &UserMessageManager::OnSendUserMessage_Post),
                 true);*/
}

void UserMessageManager::SendMessageToChat(int client, const char* message) {
  CCSUsrMsg_SayText pMsg;
  pMsg.set_ent_idx(0);
  pMsg.set_text(message);
  pMsg.set_chat(false);

  auto recipients = new CustomRecipientFilter();
  recipients->AddPlayer(client);

  vspdotnet::globals::engine->SendUserMessage(
      static_cast<IRecipientFilter&>(*recipients), m_say_text_msg_id, pMsg);
}

void UserMessageManager::SendHintMessage(int client, const char* message) {
  CCSUsrMsg_HintText pMsg;
  pMsg.set_text(message);

  auto recipients = new CustomRecipientFilter();
  recipients->AddPlayer(client);
  recipients->MakeReliable();

  vspdotnet::globals::engine->SendUserMessage(
      static_cast<IRecipientFilter&>(*recipients), m_hint_text_msg_id, pMsg);
}

void padTo(std::string& str, const size_t num, const char paddingChar = ' ') {
  if (num > str.size())
    str.insert(str.size() - 1, num - str.size(), paddingChar);
}

void UserMessageManager::SendCenterMessage(int client, const char* message) {
  CCSUsrMsg_TextMsg pMsg;

  auto newMessage = new std::string(message);
  newMessage->resize(2048);
  newMessage->shrink_to_fit();

  // This would be a hack to allow font tags in CSGO
  // Note that it only works for english, other languages may fail
  /*auto newMessage = std::string(message);
  padTo(newMessage, 2048);

  pMsg.set_msg_dst(4);
  pMsg.add_params("#SFUI_ContractKillStart");
  pMsg.add_params(newMessage);
  pMsg.add_params("");
  pMsg.add_params("");
  pMsg.add_params("");*/

  pMsg.set_msg_dst(HUD_PRINTCENTER);
  pMsg.add_params(newMessage->c_str());
  pMsg.add_params("");
  pMsg.add_params("");
  pMsg.add_params("");
  pMsg.add_params("");

  auto recipients = new CustomRecipientFilter();
  recipients->AddPlayer(client);

  vspdotnet::globals::engine->SendUserMessage(
      static_cast<IRecipientFilter&>(*recipients), m_text_msg_id, pMsg);
}

void UserMessageManager::ShowMenu(int client, int keys, int time,
                                  const char* body) {
  CCSUsrMsg_ShowMenu msg;

  msg.set_bits_valid_slots(keys);
  msg.set_display_time(time);
  msg.set_menu_string(body);

  auto recipients = new CustomRecipientFilter();
  recipients->AddPlayer(client);

  vspdotnet::globals::engine->SendUserMessage(
      static_cast<IRecipientFilter&>(*recipients), m_menu_msg_id, msg);
}

void UserMessageManager::OnSendUserMessage_Pre(
    IRecipientFilter& filter, int msg_type,
    const google::protobuf::Message& msg) {
  const char* name = g_Cstrike15UsermessageHelpers.GetName(msg_type);

  char myString[1024];
  auto stdstring = new std::string(myString);

  msg.SerializeToString(stdstring);

  // Msg("Message type [%d] (%s) send to [%d] people\n", msg_type, name,
  // filter.GetRecipientCount());
  /*if (msg_type == 7)

  //{
          auto message = static_cast<CCSUsrMsg_TextMsg*>(test);
  }*/
}

void UserMessageManager::OnSendUserMessage_Post(
    IRecipientFilter& filter, int msg_type,
    const google::protobuf::Message& msg) {}

CustomRecipientFilter::CustomRecipientFilter() { Reset(); }

CustomRecipientFilter::~CustomRecipientFilter() {}

void CustomRecipientFilter::Reset() {
  m_is_reliable = false;
  m_is_init_message = false;
  m_recipients.RemoveAll();
}

bool CustomRecipientFilter::IsReliable() const { return m_is_reliable; }

bool CustomRecipientFilter::IsInitMessage() const { return m_is_init_message; }

void CustomRecipientFilter::MakeInitMessage() {
  m_is_init_message = true;
  ;
}

void CustomRecipientFilter::MakeReliable() {
  m_is_reliable = true;
  ;
}

int CustomRecipientFilter::GetRecipientCount() const {
  return m_recipients.Count();
}

int CustomRecipientFilter::GetRecipientIndex(int slot) const {
  if (slot < 0 || slot > GetRecipientCount()) {
    return -1;
  }

  return m_recipients[slot];
}

void CustomRecipientFilter::AddPlayer(int index) {
  m_recipients.AddToTail(index);
}

void CustomRecipientFilter::RemovePlayer(int index) {
  m_recipients.FindAndRemove(index);
}

void CustomRecipientFilter::AddAllPlayers() {
  m_recipients.RemoveAll();
  for (int i = 0; i < vspdotnet::globals::gpGlobals->maxClients; i++) {
    // auto player = g_PlayerManager.GetPlayerByIndex(i);
    // if (!player || !player->IsConnected()) continue;

    AddPlayer(i);
  }
}
}  // namespace vspdotnet
#include "core/player_manager.h"
#include "core/callback_manager.h"

#include <sourcehook/sourcehook.h>
#include <eiface.h>
#include <inetchannelinfo.h>
#include <iserver.h>



#include "entity.h"
#include "con_command_manager.h"
#include "user_message_manager.h"
#include "utilities/conversions.h"

SH_DECL_HOOK5(IServerGameClients, ClientConnect, SH_NOATTRIB, 0, bool, edict_t*,
              const char*, const char*, char*, int);
SH_DECL_HOOK2_void(IServerGameClients, ClientPutInServer, SH_NOATTRIB, 0,
                   edict_t*, const char*);
SH_DECL_HOOK1_void(IServerGameClients, ClientDisconnect, SH_NOATTRIB, 0,
                   edict_t*);
SH_DECL_HOOK3_void(IServerGameDLL, ServerActivate, SH_NOATTRIB, 0, edict_t*,
                   int, int);
SH_DECL_HOOK2_void(IServerGameClients, ClientCommand, SH_NOATTRIB, 0, edict_t*,
                   const CCommand&);
SH_DECL_HOOK1_void(IServerGameDLL, GameFrame, SH_NOATTRIB, false, bool);

namespace vspdotnet {
void PlayerManager::OnStartup() {}

void PlayerManager::OnAllInitialized() {
  SH_ADD_HOOK(IServerGameClients, ClientConnect, globals::server_game_clients,
              SH_MEMBER(this, &PlayerManager::OnClientConnect), false);
  SH_ADD_HOOK(IServerGameClients, ClientConnect, globals::server_game_clients,
              SH_MEMBER(this, &PlayerManager::OnClientConnect_Post), true);
  SH_ADD_HOOK(IServerGameClients, ClientPutInServer,
              globals::server_game_clients,
              SH_MEMBER(this, &PlayerManager::OnClientPutInServer), true);
  SH_ADD_HOOK(IServerGameClients, ClientDisconnect,
              globals::server_game_clients,
              SH_MEMBER(this, &PlayerManager::OnClientDisconnect), false);
  SH_ADD_HOOK(IServerGameClients, ClientDisconnect,
              globals::server_game_clients,
              SH_MEMBER(this, &PlayerManager::OnClientDisconnect_Post), true);
  SH_ADD_HOOK(IServerGameDLL, ServerActivate, globals::server_game_dll,
              SH_MEMBER(this, &PlayerManager::OnServerActivate), true);
  SH_ADD_HOOK(IServerGameDLL, GameFrame, globals::server_game_dll,
              SH_MEMBER(this, &PlayerManager::OnGameFrame), false);
  SH_ADD_HOOK(IServerGameClients, ClientCommand, globals::server_game_clients,
              SH_MEMBER(this, &PlayerManager::OnClientCommand), false);

  m_on_client_connect_callback_ =
      globals::callback_manager.CreateCallback("OnClientConnect");
  m_on_client_connected_callback_ =
      globals::callback_manager.CreateCallback("OnClientConnected");
  m_on_client_put_in_server_callback_ =
      globals::callback_manager.CreateCallback("OnClientPutInServer");
  m_on_client_disconnect_callback_ =
      globals::callback_manager.CreateCallback("OnClientDisconnect");
  m_on_client_disconnect_post_callback_ =
      globals::callback_manager.CreateCallback("OnClientDisconnectPost");
  m_on_activate_callback_ =
      globals::callback_manager.CreateCallback("OnMapStart");
  m_on_tick_callback_ = globals::callback_manager.CreateCallback("OnTick");
}


void PlayerManager::OnShutdown()
{
  SH_REMOVE_HOOK(IServerGameClients, ClientConnect, globals::server_game_clients,
                 SH_MEMBER(this, &PlayerManager::OnClientConnect), false);
  SH_REMOVE_HOOK(IServerGameClients, ClientConnect, globals::server_game_clients,
                 SH_MEMBER(this, &PlayerManager::OnClientConnect_Post), true);
  SH_REMOVE_HOOK(IServerGameClients, ClientPutInServer, globals::server_game_clients,
                 SH_MEMBER(this, &PlayerManager::OnClientPutInServer), true);
  SH_REMOVE_HOOK(IServerGameClients, ClientDisconnect, globals::server_game_clients,
                 SH_MEMBER(this, &PlayerManager::OnClientDisconnect), false);
  SH_REMOVE_HOOK(IServerGameClients, ClientDisconnect, globals::server_game_clients,
                 SH_MEMBER(this, &PlayerManager::OnClientDisconnect_Post),
                 true);
  SH_REMOVE_HOOK(IServerGameDLL, ServerActivate, globals::server_game_dll,
                 SH_MEMBER(this, &PlayerManager::OnServerActivate), true);
  SH_REMOVE_HOOK(IServerGameDLL, GameFrame, globals::server_game_dll,
                 SH_MEMBER(this, &PlayerManager::OnGameFrame), false);
  SH_REMOVE_HOOK(IServerGameClients, ClientCommand, globals::server_game_clients,
                 SH_MEMBER(this, &PlayerManager::OnClientCommand), false);

  globals::callback_manager.ReleaseCallback(m_on_client_connect_callback_);
  globals::callback_manager.ReleaseCallback(m_on_client_connected_callback_);
  globals::callback_manager.ReleaseCallback(m_on_client_put_in_server_callback_);
  globals::callback_manager.ReleaseCallback(m_on_client_disconnect_callback_);
  globals::callback_manager.ReleaseCallback(m_on_client_disconnect_post_callback_);
  globals::callback_manager.ReleaseCallback(m_on_activate_callback_);
  globals::callback_manager.ReleaseCallback(m_on_tick_callback_);
}

bool PlayerManager::OnClientConnect(edict_t* pEntity, const char* pszName,
                                    const char* pszAddress, char* reject,
                                    int maxrejectlen) {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players_[client];

  if (pPlayer->IsConnected()) {
    OnClientDisconnect(pPlayer->GetEdict());
    OnClientDisconnect_Post(pPlayer->GetEdict());
  }

  pPlayer->Initialize(pszName, pszAddress, pEntity);

  m_on_client_connect_callback_->ScriptContext().Reset();
  m_on_client_connect_callback_->ScriptContext().Push(client);
  m_on_client_connect_callback_->ScriptContext().Push(pszName);
  m_on_client_connect_callback_->ScriptContext().Push(pszAddress);
  m_on_client_connect_callback_->Execute(false);

  if (m_on_client_connect_callback_->GetFunctionCount() > 0) {
    auto cancel =
        m_on_client_connect_callback_->ScriptContext().GetArgument<bool>(0);
    auto cancelReason =
        m_on_client_connect_callback_->ScriptContext().GetArgument<const char*>(
            1);

    m_on_client_connect_callback_->ResetContext();

    if (cancel) {
      strcpy(reject, cancelReason);
      maxrejectlen = sizeof(cancelReason);

      if (!pPlayer->IsFakeClient()) {
        RETURN_META_VALUE(MRES_SUPERCEDE, false);
      }
    }
  }

  m_user_id_lookup_[globals::engine->GetPlayerUserId(pEntity)] = client;

  return true;
}

bool PlayerManager::OnClientConnect_Post(edict_t* pEntity, const char* pszName,
                                         const char* pszAddress, char* reject,
                                         int maxrejectlen) {
  int client = ExcIndexFromEdict(pEntity);
  bool orig_value = META_RESULT_ORIG_RET(bool);
  CPlayer* pPlayer = &m_players_[client];

  if (orig_value) {
    m_on_client_connected_callback_->ScriptContext().Reset();
    m_on_client_connected_callback_->ScriptContext().Push(
        pPlayer->GetEntity());
    m_on_client_connected_callback_->Execute();

    if (!pPlayer->IsFakeClient() && m_is_listen_server_ &&
        strncmp(pszAddress, "127.0.0.1", 9) == 0) {
      m_listen_client_ = client;
    }
  } else {
    InvalidatePlayer(pPlayer);
  }

  return true;
}

void PlayerManager::OnClientPutInServer(edict_t* pEntity,
                                        char const* playername) {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players_[client];

  if (!pPlayer->IsConnected()) {
    pPlayer->m_is_fake_client_ = true;

    char error[255];
    if (!OnClientConnect(pEntity, playername, "127.0.0.1", error,
                         sizeof(error))) {
      /* :TODO: kick the bot if it's rejected */
      return;
    }

    m_on_client_connected_callback_->ScriptContext().Reset();
    m_on_client_connected_callback_->ScriptContext().Push(
        pPlayer->GetEntity());
    m_on_client_connected_callback_->Execute();
  }

  if (globals::playerinfo_manager) {
    pPlayer->m_info_ = globals::playerinfo_manager->GetPlayerInfo(pEntity);
  }

  pPlayer->Connect();
  m_player_count_++;

  m_on_client_put_in_server_callback_->ScriptContext().Reset();
  m_on_client_put_in_server_callback_->ScriptContext().Push(
      pPlayer->GetEntity());
  m_on_client_put_in_server_callback_->Execute();
}

void PlayerManager::OnClientDisconnect(edict_t* pEntity) {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players_[client];

  if (pPlayer->IsConnected()) {
    m_on_client_disconnect_callback_->ScriptContext().Reset();
    m_on_client_disconnect_callback_->ScriptContext().Push(
        pPlayer->GetEntity());
    m_on_client_disconnect_callback_->Execute();
  }

  if (pPlayer->WasCountedAsInGame()) {
    m_player_count_--;
  }
}

void PlayerManager::OnClientDisconnect_Post(edict_t* pEntity) const {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players_[client];
  if (!pPlayer->IsConnected()) {
    /* We don't care, prevent a double call */
    return;
  }

  InvalidatePlayer(pPlayer);

  m_on_client_disconnect_post_callback_->ScriptContext().Reset();
  m_on_client_disconnect_post_callback_->ScriptContext().Push(
      pPlayer->GetEntity());
  m_on_client_disconnect_post_callback_->Execute();
}

void PlayerManager::OnServerActivate(edict_t* pEdictList, int edictCount,
                                     int clientMax) const {
  m_on_activate_callback_->ScriptContext().Reset();
  m_on_activate_callback_->ScriptContext().Push(globals::server->GetMapName());
  m_on_activate_callback_->Execute();
}

void PlayerManager::OnGameFrame(bool simulating) const {
  if (!simulating) return;

  if (m_on_tick_callback_->GetFunctionCount()) {
    m_on_tick_callback_->ScriptContext().Reset();
    m_on_tick_callback_->Execute();
  }
}

void PlayerManager::OnLevelEnd() {
  for (int i = 1; i <= m_max_clients_; i++) {
    if (m_players_[i].IsConnected()) {
      OnClientDisconnect(m_players_[i].GetEdict());
      OnClientDisconnect_Post(m_players_[i].GetEdict());
    }
  }
  m_player_count_ = 0;
}

void PlayerManager::OnClientCommand(edict_t* pEntity,
                                    const CCommand& args) const {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players_[client];

  if (!pPlayer->IsConnected()) {
    return;
  }

  const char* cmd = args.Arg(0);

  bool response =
      globals::con_command_manager.DispatchClientCommand(client, cmd, &args);
  if (response) {
    RETURN_META(MRES_SUPERCEDE);
  }
}

int PlayerManager::ListenClient() const { return m_listen_client_; }

int PlayerManager::NumPlayers() const { return m_player_count_; }

int PlayerManager::MaxClients() const { return m_max_clients_; }

CPlayer* PlayerManager::GetPlayerByIndex(int client) const {
  if (client > m_max_clients_ || client < 1) {
    return nullptr;
  }

  return &m_players_[client];
}

CPlayer* PlayerManager::GetClientOfUserId(int user_id) const {
  if (user_id < 0 || user_id > USHRT_MAX) {
    return nullptr;
  }

  int client = m_user_id_lookup_[user_id];

  /* Verify the userid.  The cache can get messed up with older
   * Valve engines.  :TODO: If this gets fixed, do an old engine
   * check before invoking this backwards compat code.
   */
  if (client) {
    CPlayer* player = GetPlayerByIndex(client);
    if (player && player->IsConnected()) {
      int realUserId = globals::engine->GetPlayerUserId(player->GetEdict());
      if (realUserId == user_id) {
        return player;
      }
    }
  }

  /* If we can't verify the userid, we have to do a manual loop */
  CPlayer* player;
  for (int i = 1; i <= m_max_clients_; i++) {
    player = GetPlayerByIndex(i);
    if (!player || !player->IsConnected()) {
      continue;
    }
    if (globals::engine->GetPlayerUserId(player->GetEdict()) == user_id) {
      m_user_id_lookup_[user_id] = i;
      return GetPlayerByIndex(i);
    }
  }

  return nullptr;
}

void PlayerManager::InvalidatePlayer(CPlayer* pPlayer) const {
  auto userid = globals::engine->GetPlayerUserId(pPlayer->m_p_edict_);
  if (userid != -1) m_user_id_lookup_[userid] = 0;

  pPlayer->Disconnect();
}

CPlayer::CPlayer() {}

void CPlayer::Initialize(const char* name, const char* ip, edict_t* pEntity) {
  m_is_connected_ = true;
  m_p_edict_ = pEntity;
  m_i_index_ = ExcIndexFromEdict(pEntity);
  m_name_ = std::string(name);
  m_ip_address_ = std::string(ip);
}

edict_t* CPlayer::GetEdict() const { return m_p_edict_; }

IPlayerInfo* CPlayer::GetPlayerInfo() const { return m_info_; }

const char* CPlayer::GetName() const { return strdup(m_name_.c_str()); }

const char* CPlayer::GetAuthString() { return ""; }

bool CPlayer::IsConnected() const { return m_is_connected_; }

bool CPlayer::IsFakeClient() const { return m_is_fake_client_; }

bool CPlayer::IsAuthorized() const { return m_is_authorized_; }

bool CPlayer::IsAuthStringValidated() const {
  if (!IsFakeClient()) {
    return globals::engine->IsClientFullyAuthenticated(m_p_edict_);
  }
}

void CPlayer::Authorize() { m_is_authorized_ = true; }

void CPlayer::PrintToConsole(const char* message) const {
  if (m_is_connected_ == false || m_is_fake_client_ == true) {
    return;
  }

  INetChannelInfo* pNetChan = globals::engine->GetPlayerNetInfo(m_i_index_);
  if (pNetChan == nullptr) {
    return;
  }

   globals::engine->ClientPrintf(m_p_edict_, message);
}

void CPlayer::PrintToChat(const char* message) {
  globals::user_message_manager.SendMessageToChat(m_i_index_, message);
}

void CPlayer::PrintToHint(const char* message) {
  globals::user_message_manager.SendHintMessage(m_i_index_, message);
}

void CPlayer::PrintToCenter(const char* message) {
  globals::user_message_manager.SendCenterMessage(m_i_index_, message);
}

void CPlayer::SetName(const char* name) { m_name_ = strdup(name); }

INetChannelInfo* CPlayer::GetNetInfo() const {
  return globals::engine->GetPlayerNetInfo(m_i_index_);
}

PlayerManager::PlayerManager()
{
  m_max_clients_ = 64;
  m_players_ = new CPlayer[66];
  m_player_count_ = 0;
  m_user_id_lookup_ = new int[USHRT_MAX + 1];
  memset(m_user_id_lookup_, 0, sizeof(int) * (USHRT_MAX + 1));
}

bool CPlayer::WasCountedAsInGame() const { return m_is_in_game_; }

int CPlayer::GetUserId() {
  if (m_user_id_ == -1) {
    m_user_id_ = globals::engine->GetPlayerUserId(GetEdict());
  }

  return m_user_id_;
}

CBaseEntity* CPlayer::GetBaseEntity() const {
  edict_t* pEdict = ExcEdictFromIndex(m_i_index_);
  if (!pEdict || pEdict->IsFree() || !IsConnected()) {
    return nullptr;
  }

  IServerUnknown* pUnk;
  if ((pUnk = pEdict->GetUnknown()) == nullptr) {
    return nullptr;
  }

  return pUnk->GetBaseEntity();
}

bool CPlayer::IsInGame() const {
  return m_is_in_game_ && (m_p_edict_->GetUnknown() != nullptr);
}

void CPlayer::Kick(const char* kickReason) {
  char buffer[255];
  sprintf(buffer, "kickid %d %s\n", GetUserId(), kickReason);
  globals::engine->ServerCommand(buffer);
}

const char* CPlayer::GetWeaponName() const { return m_info_->GetWeaponName(); }

void CPlayer::ChangeTeam(int team) const { m_info_->ChangeTeam(team); }

int CPlayer::GetTeam() const { return m_info_->GetTeamIndex(); }

CBaseEntityWrapper* CPlayer::GetEntity() const {
  CBaseEntity* entity;
  if (!BaseEntityFromEdict(m_p_edict_, entity)) return nullptr;

  CBaseEntityWrapper* pEntity = reinterpret_cast<CBaseEntityWrapper*>(entity);
  return pEntity;
}

int CPlayer::GetArmor() const { return m_info_->GetArmorValue(); }

int CPlayer::GetFrags() const { return m_info_->GetFragCount(); }

int CPlayer::GetDeaths() const { return m_info_->GetDeathCount(); }

const char* CPlayer::GetKeyValue(const char* key) const {
  return globals::engine->GetClientConVarValue(m_i_index_, key);
}

Vector CPlayer::GetMaxSize() const { return m_info_->GetPlayerMaxs(); }

Vector CPlayer::GetMinSize() const { return m_info_->GetPlayerMins(); }

int CPlayer::GetMaxHealth() const { return m_info_->GetMaxHealth(); }

const char* CPlayer::GetIpAddress() const { return m_ip_address_.c_str(); }

const char* CPlayer::GetModelName() const { return m_info_->GetModelName(); }

int CPlayer::GetUserId() const { return m_user_id_; }

float CPlayer::GetTimeConnected() const {
  if (!IsConnected() || IsFakeClient()) {
    return 0;
  }

  return GetNetInfo()->GetTimeConnected();
}

float CPlayer::GetLatency() const {
  return GetNetInfo()->GetLatency(FLOW_INCOMING) +
         GetNetInfo()->GetLatency(FLOW_OUTGOING);
}

void CPlayer::Connect() {
  if (m_is_in_game_) {
    return;
  }

  m_is_in_game_ = true;
}

void CPlayer::Disconnect() {
  m_is_connected_ = false;
  m_is_in_game_ = false;
  m_name_.clear();
  m_p_edict_ = nullptr;
  m_info_ = nullptr;
  m_is_fake_client_ = false;
  m_user_id_ = -1;
  m_is_authorized_ = false;
  m_ip_address_.clear();
}

QAngle CPlayer::GetAbsAngles() const { return m_info_->GetAbsAngles(); }

Vector CPlayer::GetAbsOrigin() const { return m_info_->GetAbsOrigin(); }

bool CPlayer::IsAlive() const {
  if (!IsInGame()) {
    return false;
  }

  return !m_info_->IsDead();
}

}  // namespace vspdotnet
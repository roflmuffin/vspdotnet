#include "core/player_manager.h"
#include "core/callback_manager.h"

#include <sourcehook/sourcehook.h>
#include <eiface.h>
#include <inetchannelinfo.h>
#include <iserver.h>



#include "entity.h"
#include "con_command_manager.h"
#include "entity_listener.h"
#include "main.h"
#include "menu_manager.h"
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

  m_on_client_connect_callback =
      globals::callback_manager.CreateCallback("OnClientConnect");
  m_on_client_connected_callback =
      globals::callback_manager.CreateCallback("OnClientConnected");
  m_on_client_put_in_server_callback =
      globals::callback_manager.CreateCallback("OnClientPutInServer");
  m_on_client_disconnect_callback =
      globals::callback_manager.CreateCallback("OnClientDisconnect");
  m_on_client_disconnect_post_callback =
      globals::callback_manager.CreateCallback("OnClientDisconnectPost");
  m_on_activate_callback =
      globals::callback_manager.CreateCallback("OnMapStart");
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

  globals::callback_manager.ReleaseCallback(m_on_client_connect_callback);
  globals::callback_manager.ReleaseCallback(m_on_client_connected_callback);
  globals::callback_manager.ReleaseCallback(m_on_client_put_in_server_callback);
  globals::callback_manager.ReleaseCallback(m_on_client_disconnect_callback);
  globals::callback_manager.ReleaseCallback(m_on_client_disconnect_post_callback);
  globals::callback_manager.ReleaseCallback(m_on_activate_callback);
}

bool PlayerManager::OnClientConnect(edict_t* pEntity, const char* pszName,
                                    const char* pszAddress, char* reject,
                                    int maxrejectlen) {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players[client];

  if (pPlayer->IsConnected()) {
    OnClientDisconnect(pPlayer->GetEdict());
    OnClientDisconnect_Post(pPlayer->GetEdict());
  }

  pPlayer->Initialize(pszName, pszAddress, pEntity);

  m_on_client_connect_callback->ScriptContext().Reset();
  m_on_client_connect_callback->ScriptContext().Push(client);
  m_on_client_connect_callback->ScriptContext().Push(pszName);
  m_on_client_connect_callback->ScriptContext().Push(pszAddress);
  m_on_client_connect_callback->Execute(false);

  if (m_on_client_connect_callback->GetFunctionCount() > 0) {
    auto cancel =
        m_on_client_connect_callback->ScriptContext().GetArgument<bool>(0);
    auto cancelReason =
        m_on_client_connect_callback->ScriptContext().GetArgument<const char*>(
            1);

    m_on_client_connect_callback->ResetContext();

    if (cancel) {
      strcpy(reject, cancelReason);
      maxrejectlen = sizeof(cancelReason);

      if (!pPlayer->IsFakeClient()) {
        RETURN_META_VALUE(MRES_SUPERCEDE, false);
      }
    }
  }

  m_user_id_lookup[globals::engine->GetPlayerUserId(pEntity)] = client;

  return true;
}

bool PlayerManager::OnClientConnect_Post(edict_t* pEntity, const char* pszName,
                                         const char* pszAddress, char* reject,
                                         int maxrejectlen) {
  int client = ExcIndexFromEdict(pEntity);
  bool orig_value = META_RESULT_ORIG_RET(bool);
  CPlayer* pPlayer = &m_players[client];

  if (orig_value) {
    m_on_client_connected_callback->ScriptContext().Reset();
    m_on_client_connected_callback->ScriptContext().Push(
        pPlayer->GetEntity());
    m_on_client_connected_callback->Execute();

    if (!pPlayer->IsFakeClient() && m_is_listen_server &&
        strncmp(pszAddress, "127.0.0.1", 9) == 0) {
      m_listen_client = client;
    }
  } else {
    InvalidatePlayer(pPlayer);
  }

  return true;
}

void PlayerManager::OnClientPutInServer(edict_t* pEntity,
                                        char const* playername) {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players[client];

  if (!pPlayer->IsConnected()) {
    pPlayer->m_is_fake_client = true;

    char error[255];
    if (!OnClientConnect(pEntity, playername, "127.0.0.1", error,
                         sizeof(error))) {
      /* :TODO: kick the bot if it's rejected */
      return;
    }

    m_on_client_connected_callback->ScriptContext().Reset();
    m_on_client_connected_callback->ScriptContext().Push(
        pPlayer->GetEntity());
    m_on_client_connected_callback->Execute();
  }

  if (globals::playerinfo_manager) {
    pPlayer->m_info = globals::playerinfo_manager->GetPlayerInfo(pEntity);
  }

  pPlayer->Connect();
  m_player_count++;

  globals::entity_listener.HandleEntityCreated(pPlayer->GetBaseEntity(), client);
  globals::entity_listener.HandleEntitySpawned(pPlayer->GetBaseEntity(), client);

  m_on_client_put_in_server_callback->ScriptContext().Reset();
  m_on_client_put_in_server_callback->ScriptContext().Push(
      pPlayer->GetEntity());
  m_on_client_put_in_server_callback->Execute();
}

void PlayerManager::OnClientDisconnect(edict_t* pEntity) {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players[client];

  if (pPlayer->IsConnected()) {
    m_on_client_disconnect_callback->ScriptContext().Reset();
    m_on_client_disconnect_callback->ScriptContext().Push(
        pPlayer->GetEntity());
    m_on_client_disconnect_callback->Execute();
  }

  if (pPlayer->WasCountedAsInGame()) {
    m_player_count--;
  }

  globals::entity_listener.HandleEntityDeleted(pPlayer->GetBaseEntity(), client);
}

void PlayerManager::OnClientDisconnect_Post(edict_t* pEntity) const {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players[client];
  if (!pPlayer->IsConnected()) {
    /* We don't care, prevent a double call */
    return;
  }

  InvalidatePlayer(pPlayer);

  m_on_client_disconnect_post_callback->ScriptContext().Reset();
  m_on_client_disconnect_post_callback->ScriptContext().Push(
      pPlayer->GetEntity());
  m_on_client_disconnect_post_callback->Execute();
}

void PlayerManager::OnServerActivate(edict_t* pEdictList, int edictCount,
                                     int clientMax) const {
  m_on_activate_callback->ScriptContext().Reset();
  m_on_activate_callback->ScriptContext().Push(globals::server->GetMapName());
  m_on_activate_callback->Execute();
}

void PlayerManager::OnGameFrame(bool simulating) const {
  globals::vsp_plugin.OnThink(simulating);

  if (!simulating) return;
}

void PlayerManager::OnLevelEnd() {
  for (int i = 1; i <= m_max_clients; i++) {
    if (m_players[i].IsConnected()) {
      OnClientDisconnect(m_players[i].GetEdict());
      OnClientDisconnect_Post(m_players[i].GetEdict());
    }
  }
  m_player_count = 0;
}

void PlayerManager::OnClientCommand(edict_t* pEntity,
                                    const CCommand& args) const {
  int client = ExcIndexFromEdict(pEntity);
  CPlayer* pPlayer = &m_players[client];

  if (!pPlayer->IsConnected()) {
    return;
  }

  const char* cmd = args.Arg(0);

  if (globals::menu_manager.OnClientCommand(client, cmd, args))
  {
    RETURN_META(MRES_SUPERCEDE);
  }

  bool response =
      globals::con_command_manager.DispatchClientCommand(client, cmd, &args);
  if (response) {
    RETURN_META(MRES_SUPERCEDE);
  }
}

int PlayerManager::ListenClient() const { return m_listen_client; }

int PlayerManager::NumPlayers() const { return m_player_count; }

int PlayerManager::MaxClients() const { return m_max_clients; }

CPlayer* PlayerManager::GetPlayerByIndex(int client) const {
  if (client > m_max_clients || client < 1) {
    return nullptr;
  }

  return &m_players[client];
}

CPlayer* PlayerManager::GetClientOfUserId(int user_id) const {
  if (user_id < 0 || user_id > USHRT_MAX) {
    return nullptr;
  }

  int client = m_user_id_lookup[user_id];

  /* Verify the userid.  The cache can get messed up with older
   * Valve engines.  :TODO: If this gets fixed, do an old engine
   * check before invoking this backwards compat code.
   */
  if (client) {
    CPlayer* player = GetPlayerByIndex(client);
    if (player && player->IsConnected()) {
      int realUserId = ExcUseridFromEdict(player->GetEdict());
      if (realUserId == user_id) {
        return player;
      }
    }
  }

  /* If we can't verify the userid, we have to do a manual loop */
  CPlayer* player;
  auto index = ExcIndexFromUserid(user_id);
  player = GetPlayerByIndex(index);
  if (player && player->IsConnected()) {
    m_user_id_lookup[user_id] = index;
    return player;
  }

  return nullptr;
}

void PlayerManager::InvalidatePlayer(CPlayer* pPlayer) const {
  auto userid = globals::engine->GetPlayerUserId(pPlayer->m_p_edict);
  if (userid != -1) m_user_id_lookup[userid] = 0;

  pPlayer->Disconnect();
}

CPlayer::CPlayer() {}

void CPlayer::Initialize(const char* name, const char* ip, edict_t* pEntity) {
  m_is_connected = true;
  m_p_edict = pEntity;
  m_i_index = ExcIndexFromEdict(pEntity);
  m_name = std::string(name);
  m_ip_address = std::string(ip);
}

edict_t* CPlayer::GetEdict() const { return m_p_edict; }

IPlayerInfo* CPlayer::GetPlayerInfo() const { return m_info; }

const char* CPlayer::GetName() const { return strdup(m_name.c_str()); }

const char* CPlayer::GetAuthString() { return ""; }

bool CPlayer::IsConnected() const { return m_is_connected; }

bool CPlayer::IsFakeClient() const { return m_is_fake_client; }

bool CPlayer::IsAuthorized() const { return m_is_authorized; }

bool CPlayer::IsAuthStringValidated() const {
  if (!IsFakeClient()) {
    return globals::engine->IsClientFullyAuthenticated(m_p_edict);
  }
}

void CPlayer::Authorize() { m_is_authorized = true; }

void CPlayer::PrintToConsole(const char* message) const {
  if (m_is_connected == false || m_is_fake_client == true) {
    return;
  }

  INetChannelInfo* pNetChan = globals::engine->GetPlayerNetInfo(m_i_index);
  if (pNetChan == nullptr) {
    return;
  }

   globals::engine->ClientPrintf(m_p_edict, message);
}

void CPlayer::PrintToChat(const char* message) {
  globals::user_message_manager.SendMessageToChat(m_i_index, message);
}

void CPlayer::PrintToHint(const char* message) {
  globals::user_message_manager.SendHintMessage(m_i_index, message);
}

void CPlayer::PrintToCenter(const char* message) {
  globals::user_message_manager.SendCenterMessage(m_i_index, message);
}

void CPlayer::SetName(const char* name) { m_name = strdup(name); }

INetChannelInfo* CPlayer::GetNetInfo() const {
  return globals::engine->GetPlayerNetInfo(m_i_index);
}

PlayerManager::PlayerManager()
{
  m_max_clients = 64;
  m_players = new CPlayer[66];
  m_player_count = 0;
  m_user_id_lookup = new int[USHRT_MAX + 1];
  memset(m_user_id_lookup, 0, sizeof(int) * (USHRT_MAX + 1));
}

bool CPlayer::WasCountedAsInGame() const { return m_is_in_game; }

int CPlayer::GetUserId() {
  if (m_user_id == -1) {
    m_user_id = globals::engine->GetPlayerUserId(GetEdict());
  }

  return m_user_id;
}

CBaseEntity* CPlayer::GetBaseEntity() const {
  edict_t* pEdict = ExcEdictFromIndex(m_i_index);
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
  return m_is_in_game && (m_p_edict->GetUnknown() != nullptr);
}

void CPlayer::Kick(const char* kickReason) {
  char buffer[255];
  sprintf(buffer, "kickid %d %s\n", GetUserId(), kickReason);
  globals::engine->ServerCommand(buffer);
}

const char* CPlayer::GetWeaponName() const { return m_info->GetWeaponName(); }

void CPlayer::ChangeTeam(int team) const { m_info->ChangeTeam(team); }

int CPlayer::GetTeam() const { return m_info->GetTeamIndex(); }

CBaseEntityWrapper* CPlayer::GetEntity() const {
  CBaseEntity* entity;
  if (!BaseEntityFromEdict(m_p_edict, entity)) return nullptr;

  CBaseEntityWrapper* pEntity = reinterpret_cast<CBaseEntityWrapper*>(entity);
  return pEntity;
}

int CPlayer::GetArmor() const { return m_info->GetArmorValue(); }

int CPlayer::GetFrags() const { return m_info->GetFragCount(); }

int CPlayer::GetDeaths() const { return m_info->GetDeathCount(); }

const char* CPlayer::GetKeyValue(const char* key) const {
  return globals::engine->GetClientConVarValue(m_i_index, key);
}

Vector CPlayer::GetMaxSize() const { return m_info->GetPlayerMaxs(); }

Vector CPlayer::GetMinSize() const { return m_info->GetPlayerMins(); }

int CPlayer::GetMaxHealth() const { return m_info->GetMaxHealth(); }

const char* CPlayer::GetIpAddress() const { return m_ip_address.c_str(); }

const char* CPlayer::GetModelName() const { return m_info->GetModelName(); }

int CPlayer::GetUserId() const { return m_user_id; }

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
  if (m_is_in_game) {
    return;
  }

  m_is_in_game = true;
}

void CPlayer::Disconnect() {
  m_is_connected = false;
  m_is_in_game = false;
  m_name.clear();
  m_p_edict = nullptr;
  m_info = nullptr;
  m_is_fake_client = false;
  m_user_id = -1;
  m_is_authorized = false;
  m_ip_address.clear();
}

QAngle CPlayer::GetAbsAngles() const { return m_info->GetAbsAngles(); }

Vector CPlayer::GetAbsOrigin() const { return m_info->GetAbsOrigin(); }

bool CPlayer::IsAlive() const {
  if (!IsInGame()) {
    return false;
  }

  return !m_info->IsDead();
}

}  // namespace vspdotnet
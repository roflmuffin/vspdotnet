#pragma once

#include <string>

#include "core/globals.h"

class CBaseEntity;
class INetChannelInfo;
class IPlayerInfo;
struct edict_t;

namespace vspdotnet {
class CCallback;
class CBaseEntityWrapper;

class CPlayer {
  friend class PlayerManager;
 public:
  CPlayer();

 public:
  void Initialize(const char* name, const char* ip, edict_t* pEntity);
  void Connect();
  void Disconnect();
  edict_t* GetEdict() const;
  IPlayerInfo* GetPlayerInfo() const;
  bool WasCountedAsInGame() const;
  int GetUserId();
  CBaseEntity* GetBaseEntity() const;
  bool IsAuthStringValidated() const;
  void Authorize();

 public:
  const char* GetName() const;
  const char* GetAuthString();
  bool IsConnected() const;
  bool IsFakeClient() const;
  bool IsAuthorized() const;
  void PrintToConsole(const char* message) const;
  void PrintToChat(const char* message);
  void PrintToHint(const char* message);
  void PrintToCenter(const char* message);
  QAngle GetAbsAngles() const;
  Vector GetAbsOrigin() const;
  bool IsAlive() const;
  bool IsInGame() const;
  void Kick(const char* kickReason);
  const char* GetWeaponName() const;
  void ChangeTeam(int team) const;
  int GetTeam() const;
  CBaseEntityWrapper* GetEntity() const;
  int GetArmor() const;
  int GetFrags() const;
  int GetDeaths() const;
  const char* GetKeyValue(const char* key) const;
  Vector GetMaxSize() const;
  Vector GetMinSize() const;
  int GetMaxHealth() const;
  const char* GetIpAddress() const;
  const char* GetModelName() const;
  int GetUserId() const;
  float GetTimeConnected() const;
  float GetLatency() const;

 private:
  std::string m_name_;
  edict_t* m_p_edict_ = nullptr;
  IPlayerInfo* m_info_ = nullptr;
  std::string m_auth_id_;
  bool m_is_connected_ = false;
  bool m_is_fake_client_ = false;
  bool m_is_in_game_ = false;
  bool m_is_authorized_ = false;
  int m_user_id_ = 1;
  int m_i_index_;
  std::string m_ip_address_;
  void SetName(const char* name);
  INetChannelInfo* GetNetInfo() const;
};


class PlayerManager : public GlobalClass {
  friend class CPlayer;

 public:
  PlayerManager();
  void OnStartup() override;
  void OnAllInitialized() override;
  bool OnClientConnect(edict_t* pEntity, const char* pszName,
                       const char* pszAddress, char* reject, int maxrejectlen);
  bool OnClientConnect_Post(edict_t* pEntity, const char* pszName,
                            const char* pszAddress, char* reject,
                            int maxrejectlen);
  void OnClientPutInServer(edict_t* pEntity, char const* playername);
  void OnClientDisconnect(edict_t* pEntity);
  void OnClientDisconnect_Post(edict_t* pEntity) const;
  void OnServerActivate(edict_t* pEdictList, int edictCount,
                        int clientMax) const;
  void OnGameFrame(bool simulating) const;
  void OnThink(bool last_tick) const;
  void OnShutdown() override;
  void OnLevelEnd() override;
  void OnClientCommand(edict_t* pEntity, const CCommand& args) const;
  int ListenClient() const;

  public:
   int NumPlayers() const;
   int MaxClients() const;
   CPlayer* GetPlayerByIndex(int client) const;
   CPlayer* GetClientOfUserId(int user_id) const;

  private:
   void InvalidatePlayer(CPlayer* pPlayer) const;

   CPlayer* m_players_;
   int m_max_clients_ = 0;
   int m_player_count_ = 0;
   int* m_user_id_lookup_;
   int m_listen_client_;
   bool m_is_listen_server_;

  CCallback* m_on_client_connect_callback_;
  CCallback* m_on_client_put_in_server_callback_;
  CCallback* m_on_client_connected_callback_;
  CCallback* m_on_client_disconnect_callback_;
  CCallback* m_on_client_disconnect_post_callback_;
  CCallback* m_on_activate_callback_;
  CCallback* m_on_tick_callback_;
};

}  // namespace vspdotnet
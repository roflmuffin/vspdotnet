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

#include <core/sig_scanner.h>

#include "core/autonative.h"
#include "core/entity.h"
#include "core/script_engine.h"

#include <game/server/iplayerinfo.h>

#include <sstream>



#include "core/function.h"
#include "core/player_manager.h"

namespace vspdotnet {

CREATE_GETTER_FUNCTION(Player, IPlayerInfo*, PlayerInfo, CBaseEntity*, ExcPlayerInfoFromBaseEntity(obj))
CREATE_GETTER_FUNCTION(PlayerInfo, const char*, Name, IPlayerInfo*, obj->GetName());
CREATE_GETTER_FUNCTION(PlayerInfo, int, UserId, IPlayerInfo*, obj->GetUserID());
CREATE_GETTER_FUNCTION(PlayerInfo, const char*, SteamId, IPlayerInfo*, obj->GetNetworkIDString());
CREATE_GETTER_FUNCTION(PlayerInfo, int, Team, IPlayerInfo*, obj->GetTeamIndex());
CREATE_SETTER_FUNCTION(PlayerInfo, int, Team, IPlayerInfo*, obj->ChangeTeam(value));

CREATE_GETTER_FUNCTION(PlayerInfo, int, Kills, IPlayerInfo*, obj->GetFragCount());
CREATE_GETTER_FUNCTION(PlayerInfo, int, Deaths, IPlayerInfo*, obj->GetDeathCount());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsConnected, IPlayerInfo*, obj->IsConnected());
CREATE_GETTER_FUNCTION(PlayerInfo, int, Armor, IPlayerInfo*, obj->GetArmorValue());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsHLTV, IPlayerInfo*, obj->IsHLTV());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsPlayer, IPlayerInfo*, obj->IsHLTV());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsFakeClient, IPlayerInfo*, obj->IsFakeClient());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsDead, IPlayerInfo*, obj->IsDead());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsInAVehicle, IPlayerInfo*, obj->IsInAVehicle());
CREATE_GETTER_FUNCTION(PlayerInfo, bool, IsObserver, IPlayerInfo*, obj->IsObserver());
CREATE_GETTER_FUNCTION(PlayerInfo, const char*, WeaponName, IPlayerInfo*, obj->GetWeaponName());
CREATE_GETTER_FUNCTION(PlayerInfo, const char*, ModelName, IPlayerInfo*, obj->GetModelName());
CREATE_GETTER_FUNCTION(PlayerInfo, int, Health, IPlayerInfo*, obj->GetHealth());
CREATE_GETTER_FUNCTION(PlayerInfo, int, MaxHealth, IPlayerInfo*, obj->GetMaxHealth());

bool PlayerIsInGame(ScriptContext& script_context)
{
  auto index = script_context.GetArgument<int>(0);


  if ((index < 1) || (index > globals::player_manager.MaxClients())) {
    script_context.ThrowNativeError("Client index %d is invalid", index);
    return false;
  }

  return globals::player_manager.GetPlayerByIndex(index)->IsInGame();
}

bool IsFakeClient(ScriptContext& script_context)
{
  auto index = script_context.GetArgument<int>(0);

  if ((index < 1) || (index > globals::player_manager.MaxClients())) {
    script_context.ThrowNativeError("Client index %d is invalid", index);
    return false;
  }

   return globals::player_manager.GetPlayerByIndex(index)->IsFakeClient();
}

REGISTER_NATIVES(player, {
  ScriptEngine::RegisterNativeHandler("PLAYER_GET_PLAYERINFO", PlayerGetPlayerInfo);

  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_NAME", PlayerInfoGetName);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_USERID", PlayerInfoGetUserId);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_STEAMID", PlayerInfoGetSteamId);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_TEAM", PlayerInfoGetTeam);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_SET_TEAM", PlayerInfoSetTeam);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_KILLS", PlayerInfoGetKills);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_DEATHS", PlayerInfoGetDeaths);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_CONNECTED", PlayerInfoGetIsConnected);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_ARMOR", PlayerInfoGetArmor);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_HLTV", PlayerInfoGetIsHLTV);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_PLAYER", PlayerInfoGetIsPlayer);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_FAKECLIENT", PlayerInfoGetIsFakeClient);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_DEAD", PlayerInfoGetIsDead);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_IN_VEHICLE", PlayerInfoGetIsInAVehicle);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_IS_OBSERVER", PlayerInfoGetIsObserver);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_WEAPON_NAME", PlayerInfoGetWeaponName);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_MODEL_NAME", PlayerInfoGetModelName);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_HEALTH", PlayerInfoGetHealth);
  ScriptEngine::RegisterNativeHandler("PLAYERINFO_GET_MAX_HEALTH", PlayerInfoGetMaxHealth);

  ScriptEngine::RegisterNativeHandler("CLIENT_IS_IN_GAME", PlayerIsInGame);
  ScriptEngine::RegisterNativeHandler("IS_FAKE_CLIENT", IsFakeClient);
})

}

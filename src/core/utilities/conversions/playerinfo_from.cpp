/**
* =============================================================================
* Source Python
* Copyright (C) 2012-2015 Source Python Development Team.  All rights reserved.
* =============================================================================
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License, version 3.0, as published by the
* Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License along with
* this program.  If not, see <http://www.gnu.org/licenses/>.
*
* As a special exception, the Source Python Team gives you permission
* to link the code of this program (as well as its derivative works) to
* "Half-Life 2," the "Source Engine," and any Game MODs that run on software
* by the Valve Corporation.  You must obey the GNU General Public License in
* all respects for all other code used.  Additionally, the Source.Python
* Development Team grants this exception to all derivative works.
*/

//-----------------------------------------------------------------------------
// Includes.
//-----------------------------------------------------------------------------
#include "../conversions.h"
#include "core/globals.h"

namespace vspdotnet {
//-----------------------------------------------------------------------------
// Returns a PlayerInfo instance from the given index.
//-----------------------------------------------------------------------------
bool PlayerInfoFromIndex(int iEntityIndex, IPlayerInfo*& output) {
  if (iEntityIndex == WORLD_ENTITY_INDEX ||
      iEntityIndex > (int)globals::gpGlobals->maxClients)
    return false;

  edict_t* pEdict;
  if (!EdictFromIndex(iEntityIndex, pEdict)) return false;

  return PlayerInfoFromEdict(pEdict, output);
}

//-----------------------------------------------------------------------------
// Returns a PlayerInfo instance from the given BaseEntity instance.
//-----------------------------------------------------------------------------
bool PlayerInfoFromBaseEntity(CBaseEntity* pBaseEntity, IPlayerInfo*& output) {
  int iEntityIndex;
  if (!IndexFromBaseEntity(pBaseEntity, iEntityIndex)) return false;

  return PlayerInfoFromIndex(iEntityIndex, output);
}

//-----------------------------------------------------------------------------
// Returns a PlayerInfo instance from the given Edict instance.
//-----------------------------------------------------------------------------
bool PlayerInfoFromEdict(edict_t* pEdict, IPlayerInfo*& output) {
  if (!pEdict || pEdict->IsFree()) return false;

  int iEntityIndex;
  if (!IndexFromEdict(pEdict, iEntityIndex)) return false;

  // We need to check this, because GetPlayerInfo() doesn't make sure the given
  // edict is a player edict. Instead it will return a non-NULL pointer.
  if (iEntityIndex <= WORLD_ENTITY_INDEX ||
      iEntityIndex > (int)globals::gpGlobals->maxClients)
    return false;

  IPlayerInfo* pPlayerInfo = globals::playerinfo_manager->GetPlayerInfo(pEdict);
  if (!pPlayerInfo) return false;

  output = pPlayerInfo;
  return true;
}

//-----------------------------------------------------------------------------
// Returns a PlayerInfo instance from the given BaseHandle instance.
//-----------------------------------------------------------------------------
bool PlayerInfoFromBaseHandle(CBaseHandle hBaseHandle, IPlayerInfo*& output) {
  int iEntityIndex;
  if (!IndexFromBaseHandle(hBaseHandle, iEntityIndex)) return false;

  return PlayerInfoFromIndex(iEntityIndex, output);
}

//-----------------------------------------------------------------------------
// Returns a PlayerInfo instance from the given IntHandle.
//-----------------------------------------------------------------------------
bool PlayerInfoFromIntHandle(int iEntityHandle, IPlayerInfo*& output) {
  int iEntityIndex;
  if (!IndexFromIntHandle(iEntityHandle, iEntityIndex)) return false;

  return PlayerInfoFromIndex(iEntityIndex, output);
}

//-----------------------------------------------------------------------------
// Returns a PlayerInfo instance from the given UserID.
//-----------------------------------------------------------------------------
bool PlayerInfoFromUserid(int iUserID, IPlayerInfo*& output) {
  edict_t* pEdict;
  if (!EdictFromUserid(iUserID, pEdict)) return false;

  return PlayerInfoFromEdict(pEdict, output);
}
}  // namespace vspdotnet
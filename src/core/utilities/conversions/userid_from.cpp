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
#include <eiface.h>

namespace vspdotnet {
//-----------------------------------------------------------------------------
// Returns a UserID from the given PlayerInfo instance.
//-----------------------------------------------------------------------------
bool UseridFromPlayerInfo(IPlayerInfo* pPlayerInfo, int& output) {
  if (!pPlayerInfo) return false;

  int iUserID = pPlayerInfo->GetUserID();
  if (iUserID == INVALID_PLAYER_USERID) return false;

  output = iUserID;
  return true;
}

//-----------------------------------------------------------------------------
// Returns a UserID from the given index.
//-----------------------------------------------------------------------------
bool UseridFromIndex(int iEntityIndex, int& output) {
  IPlayerInfo* pPlayerInfo;
  if (!PlayerInfoFromIndex(iEntityIndex, pPlayerInfo)) return false;

  return UseridFromPlayerInfo(pPlayerInfo, output);
}

//-----------------------------------------------------------------------------
// Returns a UserID from the given Edict instance.
//-----------------------------------------------------------------------------
bool UseridFromEdict(edict_t* pEdict, int& output) {
  int result = globals::engine->GetPlayerUserId(pEdict);
  if (result == -1) return false;

  output = (int)result;
  return true;
}

//-----------------------------------------------------------------------------
// Returns a UserID from the given BaseHandle instance.
//-----------------------------------------------------------------------------
bool UseridFromBaseHandle(CBaseHandle hBaseHandle, int& output) {
  int iEntityIndex;
  if (!IndexFromBaseHandle(hBaseHandle, iEntityIndex)) return false;

  return UseridFromIndex(iEntityIndex, output);
}

//-----------------------------------------------------------------------------
// Returns a UserID from the given IntHandle.
//-----------------------------------------------------------------------------
bool UseridFromIntHandle(int iEntityHandle, int& output) {
  CBaseHandle hBaseHandle;
  if (!BaseHandleFromIntHandle(iEntityHandle, hBaseHandle)) return false;

  return UseridFromBaseHandle(hBaseHandle, output);
}

//-----------------------------------------------------------------------------
// Returns a UserID from the given BaseEntity instance.
//-----------------------------------------------------------------------------
bool UseridFromBaseEntity(CBaseEntity* pBaseEntity, int& output) {
  IPlayerInfo* pPlayerInfo;
  if (!PlayerInfoFromBaseEntity(pBaseEntity, pPlayerInfo)) return false;

  return UseridFromPlayerInfo(pPlayerInfo, output);
}

}  // namespace vspdotnet
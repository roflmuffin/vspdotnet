#pragma once

#include "core/globals.h"

#include "basehandle.h"
#include <edict.h>
#include "public/game/server/iplayerinfo.h"
#include <toolframework/itoolentity.h>

class CBaseEntityWrapper;
class CBaseEntity;

namespace vspdotnet {

/*
inline void test(ScriptContext& script_context) {
  int from = script_context.GetArgument<from_type>(0);
}*/

#define WORLD_ENTITY_INDEX 0

#define INVALID_ENTITY_INDEX -1
#define INVALID_PLAYER_USERID -1

#define CREATE_EXC_CONVERSION_FUNCTION(to_type, to_name, from_type, from_name) \
  inline to_type Exc##to_name##From##from_name(from_type from) {               \
    to_type result;                                                            \
    if (!to_name##From##from_name(from, result)) {                             \
      return -1;                                                               \
    }                                                                          \
    return result;                                                             \
  }

#define CREATE_EXC_CONVERSION_FUNCTION_POINTER(to_type, to_name, from_type, \
                                               from_name)                   \
  inline to_type Exc##to_name##From##from_name(from_type from) {            \
    to_type result;                                                         \
    if (!to_name##From##from_name(from, result)) {                          \
      return nullptr;                                                       \
    }                                                                       \
    return result;                                                          \
  }

//-----------------------------------------------------------------------------
// EdictFrom* declarations
//-----------------------------------------------------------------------------
bool EdictFromIndex(int iEntityIndex, edict_t*& output);
bool EdictFromUserid(int iUserID, edict_t*& output);
bool EdictFromPlayerInfo(IPlayerInfo* pPlayerInfo, edict_t*& output);
bool EdictFromBaseEntity(CBaseEntity* pBaseEntity, edict_t*& output);
bool EdictFromBaseHandle(CBaseHandle hBaseHandle, edict_t*& output);
bool EdictFromIntHandle(int iEntityHandle, edict_t*& output);

CREATE_EXC_CONVERSION_FUNCTION_POINTER(edict_t*, Edict, int, Index);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(edict_t*, Edict, CBaseHandle,
                                       BaseHandle);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(edict_t*, Edict, int, IntHandle);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(edict_t*, Edict, CBaseEntity*,
                                       BaseEntity);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(edict_t*, Edict, int, Userid);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(edict_t*, Edict, IPlayerInfo*,
                                       PlayerInfo);

//-----------------------------------------------------------------------------
// IntHandleFrom* declarations
//-----------------------------------------------------------------------------
bool IntHandleFromBaseHandle(CBaseHandle hBaseHandle, int& output);
bool IntHandleFromIndex(int iEntityIndex, int& output);
bool IntHandleFromEdict(edict_t* pEdict, int& output);
bool IntHandleFromBaseEntity(CBaseEntity* pBaseEntity, int& output);
bool IntHandleFromUserid(int iUserID, int& output);
bool IntHandleFromPlayerInfo(IPlayerInfo* pPlayerInfo, int& output);

CREATE_EXC_CONVERSION_FUNCTION(int, IntHandle, int, Index);
CREATE_EXC_CONVERSION_FUNCTION(int, IntHandle, edict_t*, Edict);
CREATE_EXC_CONVERSION_FUNCTION(int, IntHandle, CBaseHandle, BaseHandle);
CREATE_EXC_CONVERSION_FUNCTION(int, IntHandle, CBaseEntity*, BaseEntity);
CREATE_EXC_CONVERSION_FUNCTION(int, IntHandle, int, Userid);
CREATE_EXC_CONVERSION_FUNCTION(int, IntHandle, IPlayerInfo*, PlayerInfo);

//-----------------------------------------------------------------------------
// BaseEntityFrom* declarations
//-----------------------------------------------------------------------------
bool BaseEntityFromEdict(edict_t* pEdict, CBaseEntity*& output);
bool BaseEntityFromIndex(int iEntityIndex, CBaseEntity*& output);
bool BaseEntityFromIntHandle(int iEntityHandle, CBaseEntity*& output);
bool BaseEntityFromBaseHandle(CBaseHandle hBaseHandle, CBaseEntity*& output);
bool BaseEntityFromUserid(int iUserID, CBaseEntity*& output);
bool BaseEntityFromPlayerInfo(IPlayerInfo* pPlayerInfo, CBaseEntity*& output);

CREATE_EXC_CONVERSION_FUNCTION_POINTER(CBaseEntity*, BaseEntity, int, Index);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(CBaseEntity*, BaseEntity, edict_t*,
                                       Edict);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(CBaseEntity*, BaseEntity, CBaseHandle,
                                       BaseHandle);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(CBaseEntity*, BaseEntity, int,
                                       IntHandle);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(CBaseEntity*, BaseEntity, int, Userid);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(CBaseEntity*, BaseEntity, IPlayerInfo*,
                                       PlayerInfo);

//-----------------------------------------------------------------------------
// UseridFrom* declarations
//-----------------------------------------------------------------------------
bool UseridFromPlayerInfo(IPlayerInfo* pPlayerInfo, int& output);
bool UseridFromIndex(int iEntityIndex, int& output);
bool UseridFromEdict(edict_t* pEdict, int& output);
bool UseridFromBaseHandle(CBaseHandle hBaseHandle, int& output);
bool UseridFromIntHandle(int iEntityHandle, int& output);
bool UseridFromBaseEntity(CBaseEntity* pBaseEntity, int& output);

CREATE_EXC_CONVERSION_FUNCTION(int, Userid, int, Index);
CREATE_EXC_CONVERSION_FUNCTION(int, Userid, edict_t*, Edict);
CREATE_EXC_CONVERSION_FUNCTION(int, Userid, CBaseHandle, BaseHandle);
CREATE_EXC_CONVERSION_FUNCTION(int, Userid, int, IntHandle);
CREATE_EXC_CONVERSION_FUNCTION(int, Userid, IPlayerInfo*, PlayerInfo);
CREATE_EXC_CONVERSION_FUNCTION(int, Userid, CBaseEntity*, BaseEntity);

//-----------------------------------------------------------------------------
// PlayerInfoFrom* declarations
//-----------------------------------------------------------------------------
bool PlayerInfoFromIndex(int iEntityIndex, IPlayerInfo*& output);
bool PlayerInfoFromBaseEntity(CBaseEntity* pBaseEntity, IPlayerInfo*& output);
bool PlayerInfoFromEdict(edict_t* pEdict, IPlayerInfo*& output);
bool PlayerInfoFromBaseHandle(CBaseHandle hBaseHandle, IPlayerInfo*& output);
bool PlayerInfoFromIntHandle(int iEntityHandle, IPlayerInfo*& output);
bool PlayerInfoFromUserid(int iUserID, IPlayerInfo*& output);

CREATE_EXC_CONVERSION_FUNCTION_POINTER(IPlayerInfo*, PlayerInfo, int, Index);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(IPlayerInfo*, PlayerInfo, edict_t*,
                                       Edict);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(IPlayerInfo*, PlayerInfo, CBaseHandle,
                                       BaseHandle);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(IPlayerInfo*, PlayerInfo, int,
                                       IntHandle);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(IPlayerInfo*, PlayerInfo, int, Userid);
CREATE_EXC_CONVERSION_FUNCTION_POINTER(IPlayerInfo*, PlayerInfo, CBaseEntity*,
                                       BaseEntity);

//-----------------------------------------------------------------------------
// IndexFrom* declarations
//-----------------------------------------------------------------------------
bool IndexFromEdict(edict_t* pEdict, int& output);
bool IndexFromBaseEntity(CBaseEntity* pBaseEntity, int& output);
bool IndexFromBaseHandle(CBaseHandle hBaseHandle, int& output);
bool IndexFromIntHandle(int iEntityHandle, int& output);
bool IndexFromUserid(int iUserID, int& output);
bool IndexFromPlayerInfo(IPlayerInfo* pPlayerInfo, int& output);
bool IndexFromName(const char* szName, int& output);
bool IndexFromSteamID(const char* szSteamID, int& output);
bool IndexFromUniqueID(const char* szUniqueID, int& output);

CREATE_EXC_CONVERSION_FUNCTION(int, Index, edict_t*, Edict);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, CBaseHandle, BaseHandle);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, int, IntHandle);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, CBaseEntity*, BaseEntity);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, int, Userid);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, IPlayerInfo*, PlayerInfo);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, const char*, Name);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, const char*, SteamID);
CREATE_EXC_CONVERSION_FUNCTION(int, Index, const char*, UniqueID);

//-----------------------------------------------------------------------------
// BaseHandleFrom* declarations
//-----------------------------------------------------------------------------
bool BaseHandleFromIndex(int iEntityIndex, CBaseHandle& output);
bool BaseHandleFromIntHandle(int iEntityHandle, CBaseHandle& output);
bool BaseHandleFromBaseEntity(CBaseEntity* pBaseEntity, CBaseHandle& output);
bool BaseHandleFromUserid(int iUserID, CBaseHandle& output);
bool BaseHandleFromPlayerInfo(IPlayerInfo* pPlayerInfo, CBaseHandle& output);
bool BaseHandleFromEdict(edict_t* pEdict, CBaseHandle& output);

CREATE_EXC_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, int, Index);
CREATE_EXC_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, edict_t*, Edict);
CREATE_EXC_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, int, IntHandle);
CREATE_EXC_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, CBaseEntity*,
                               BaseEntity);
CREATE_EXC_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, int, Userid);
CREATE_EXC_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, IPlayerInfo*,
                               PlayerInfo);

bool AddressFromPlayerInfo2(IPlayerInfo* pInfo, const char*& output);
bool AddressFromPlayerInfo(IPlayerInfo* pInfo, const char*& output);

#define UNIQUE_ID_SIZE 128
bool UniqueIDFromPlayerInfo2(IPlayerInfo* pInfo, char*& output);
bool UniqueIDFromPlayerInfo(IPlayerInfo* pInfo, const char*& output);
bool UniqueIDFromIndex(int iIndex, const char*& output);

inline bool IsValidBaseEntityPointer(void* ptr) {
  if (!ptr) return false;

  CBaseEntity* pEntity = (CBaseEntity*)globals::server_tools->FirstEntity();
  while (pEntity) {
    if (pEntity == ptr) return true;

    pEntity = (CBaseEntity*)globals::server_tools->NextEntity(pEntity);
  }
  return false;
}

inline bool IsValidNetworkedEntityPointer(void* ptr) {
  if (!ptr) return false;

  for (int i = 0; i < globals::gpGlobals->maxEntities; ++i) {
    edict_t* pEdict = NULL;
    if (!EdictFromIndex(i, pEdict)) continue;

    if (pEdict->GetUnknown()->GetBaseEntity() == ptr) return true;
  }
  return false;
}

inline bool IsValidPlayerPointer(void* ptr) {
  if (!ptr) return false;

  for (int i = 1; i <= globals::gpGlobals->maxClients; ++i) {
    edict_t* pEdict = NULL;
    if (!EdictFromIndex(i, pEdict)) continue;

    if (pEdict->GetUnknown()->GetBaseEntity() == ptr) return true;
  }
  return false;
}

}  // namespace vspdotnet
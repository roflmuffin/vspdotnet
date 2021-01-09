#include <core/utilities/conversions.h>

#include "core/autonative.h"
#include "core/script_engine.h"
#include "core/entity.h"

#define CREATE_NATIVE_CONVERSION_FUNCTION(to_type, to_name, from_type, \
                                          from_name)                   \
  static inline to_type Native##to_name##From##from_name(              \
      ScriptContext& script_context) {                                 \
    from_type from = script_context.GetArgument<from_type>(0);         \
    to_type result = Exc##to_name##From##from_name(from);              \
    return result;                                                     \
  }

namespace vspdotnet
{


CREATE_NATIVE_CONVERSION_FUNCTION(edict_t*, Edict, int, Index);
CREATE_NATIVE_CONVERSION_FUNCTION(edict_t*, Edict, CBaseHandle,
                                       BaseHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(edict_t*, Edict, int, IntHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(edict_t*, Edict, CBaseEntity*,
                                       BaseEntity);
CREATE_NATIVE_CONVERSION_FUNCTION(edict_t*, Edict, int, Userid);
CREATE_NATIVE_CONVERSION_FUNCTION(edict_t*, Edict, IPlayerInfo*,
                                       PlayerInfo);

CREATE_NATIVE_CONVERSION_FUNCTION(int, IntHandle, int, Index);
CREATE_NATIVE_CONVERSION_FUNCTION(int, IntHandle, edict_t*, Edict);
CREATE_NATIVE_CONVERSION_FUNCTION(int, IntHandle, CBaseHandle, BaseHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(int, IntHandle, CBaseEntity*, BaseEntity);
CREATE_NATIVE_CONVERSION_FUNCTION(int, IntHandle, int, Userid);
CREATE_NATIVE_CONVERSION_FUNCTION(int, IntHandle, IPlayerInfo*, PlayerInfo);

CREATE_NATIVE_CONVERSION_FUNCTION(CBaseEntity*, BaseEntity, int, Index);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseEntity*, BaseEntity, edict_t*,
                                       Edict);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseEntity*, BaseEntity, CBaseHandle,
                                       BaseHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseEntity*, BaseEntity, int,
                                       IntHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseEntity*, BaseEntity, int, Userid);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseEntity*, BaseEntity, IPlayerInfo*,
                                       PlayerInfo);

CREATE_NATIVE_CONVERSION_FUNCTION(int, Userid, int, Index);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Userid, edict_t*, Edict);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Userid, CBaseHandle, BaseHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Userid, int, IntHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Userid, IPlayerInfo*, PlayerInfo);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Userid, CBaseEntity*, BaseEntity);


CREATE_NATIVE_CONVERSION_FUNCTION(IPlayerInfo*, PlayerInfo, int, Index);
CREATE_NATIVE_CONVERSION_FUNCTION(IPlayerInfo*, PlayerInfo, edict_t*,
                                       Edict);
CREATE_NATIVE_CONVERSION_FUNCTION(IPlayerInfo*, PlayerInfo, CBaseHandle,
                                       BaseHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(IPlayerInfo*, PlayerInfo, int,
                                       IntHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(IPlayerInfo*, PlayerInfo, int, Userid);
CREATE_NATIVE_CONVERSION_FUNCTION(IPlayerInfo*, PlayerInfo, CBaseEntity*,
                                       BaseEntity);

CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, edict_t*, Edict);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, CBaseHandle, BaseHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, int, IntHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, CBaseEntity*, BaseEntity);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, int, Userid);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, IPlayerInfo*, PlayerInfo);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, const char*, Name);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, const char*, SteamID);
CREATE_NATIVE_CONVERSION_FUNCTION(int, Index, const char*, UniqueID);

CREATE_NATIVE_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, int, Index);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, edict_t*, Edict);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, int, IntHandle);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, CBaseEntity*,
                               BaseEntity);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, int, Userid);
CREATE_NATIVE_CONVERSION_FUNCTION(CBaseHandle, BaseHandle, IPlayerInfo*,
                               PlayerInfo);
	
REGISTER_NATIVES(conversions, {
  REGISTER_NATIVE(EDICT_FROM_INDEX, NativeEdictFromIndex);
  REGISTER_NATIVE(EDICT_FROM_BASEHANDLE, NativeEdictFromBaseHandle);
  REGISTER_NATIVE(EDICT_FROM_INTHANDLE, NativeEdictFromIntHandle);
  REGISTER_NATIVE(EDICT_FROM_BASEENTITY, NativeEdictFromBaseEntity);
  REGISTER_NATIVE(EDICT_FROM_USERID, NativeEdictFromUserid);
  REGISTER_NATIVE(EDICT_FROM_PLAYERINFO, NativeEdictFromPlayerInfo);

  REGISTER_NATIVE(INTHANDLE_FROM_INDEX, NativeIntHandleFromIndex);
  REGISTER_NATIVE(INTHANDLE_FROM_EDICT, NativeIntHandleFromEdict);
  REGISTER_NATIVE(INTHANDLE_FROM_BASEHANDLE, NativeIntHandleFromBaseHandle);
  REGISTER_NATIVE(INTHANDLE_FROM_BASEENTITY, NativeIntHandleFromBaseEntity);
  REGISTER_NATIVE(INTHANDLE_FROM_USERID, NativeIntHandleFromUserid);
  REGISTER_NATIVE(INTHANDLE_FROM_PLAYERINFO, NativeIntHandleFromPlayerInfo);

  REGISTER_NATIVE(BASEENTITY_FROM_INDEX, NativeBaseEntityFromIndex);
  REGISTER_NATIVE(BASEENTITY_FROM_EDICT, NativeBaseEntityFromEdict);
  REGISTER_NATIVE(BASEENTITY_FROM_BASEHANDLE, NativeBaseEntityFromBaseHandle);
  REGISTER_NATIVE(BASEENTITY_FROM_INTHANDLE, NativeBaseEntityFromIntHandle);
  REGISTER_NATIVE(BASEENTITY_FROM_USERID, NativeBaseEntityFromUserid);
  REGISTER_NATIVE(BASEENTITY_FROM_PLAYERINFO, NativeBaseEntityFromPlayerInfo);

  REGISTER_NATIVE(USERID_FROM_INDEX, NativeUseridFromIndex);
  REGISTER_NATIVE(USERID_FROM_EDICT, NativeUseridFromEdict);
  REGISTER_NATIVE(USERID_FROM_BASEHANDLE, NativeUseridFromBaseHandle);
  REGISTER_NATIVE(USERID_FROM_INTHANDLE, NativeUseridFromIntHandle);
  REGISTER_NATIVE(USERID_FROM_PLAYERINFO, NativeUseridFromPlayerInfo);
  REGISTER_NATIVE(USERID_FROM_BASEENTITY, NativeUseridFromBaseEntity);

  REGISTER_NATIVE(PLAYERINFO_FROM_INDEX, NativePlayerInfoFromIndex);
  REGISTER_NATIVE(PLAYERINFO_FROM_EDICT, NativePlayerInfoFromEdict);
  REGISTER_NATIVE(PLAYERINFO_FROM_BASEHANDLE, NativePlayerInfoFromBaseHandle);
  REGISTER_NATIVE(PLAYERINFO_FROM_INTHANDLE, NativePlayerInfoFromIntHandle);
  REGISTER_NATIVE(PLAYERINFO_FROM_USERID, NativePlayerInfoFromUserid);
  REGISTER_NATIVE(PLAYERINFO_FROM_BASEENTITY, NativePlayerInfoFromBaseEntity);

  REGISTER_NATIVE(INDEX_FROM_EDICT, NativeIndexFromEdict);
  REGISTER_NATIVE(INDEX_FROM_BASEHANDLE, NativeIndexFromBaseHandle);
  REGISTER_NATIVE(INDEX_FROM_INTHANDLE, NativeIndexFromIntHandle);
  REGISTER_NATIVE(INDEX_FROM_BASEENTITY, NativeIndexFromBaseEntity);
  REGISTER_NATIVE(INDEX_FROM_USERID, NativeIndexFromUserid);
  REGISTER_NATIVE(INDEX_FROM_PLAYERINFO, NativeIndexFromPlayerInfo);
  REGISTER_NATIVE(INDEX_FROM_NAME, NativeIndexFromName);
  REGISTER_NATIVE(INDEX_FROM_STEAMID, NativeIndexFromSteamID);
  REGISTER_NATIVE(INDEX_FROM_UNIQUEID, NativeIndexFromUniqueID);

  REGISTER_NATIVE(BASEHANDLE_FROM_INDEX, NativeBaseHandleFromIndex);
  REGISTER_NATIVE(BASEHANDLE_FROM_EDICT, NativeBaseHandleFromEdict);
  REGISTER_NATIVE(BASEHANDLE_FROM_INTHANDLE, NativeBaseHandleFromIntHandle);
  REGISTER_NATIVE(BASEHANDLE_FROM_BASEENTITY, NativeBaseHandleFromBaseEntity);
  REGISTER_NATIVE(BASEHANDLE_FROM_USERID, NativeBaseHandleFromUserid);
  REGISTER_NATIVE(BASEHANDLE_FROM_PLAYERINFO, NativeBaseHandleFromPlayerInfo);

  ScriptEngine::RegisterNativeHandler<const char*>("GET_ENTITY_CLASSNAME", [](ScriptContext& script_context) {
        auto base_entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
        return base_entity->GetClassname();
  });

  ScriptEngine::RegisterNativeHandler<const char*>(
      "GET_PLAYERINFO_NAME", [](ScriptContext& script_context) {
        auto player_info = script_context.GetArgument<IPlayerInfo*>(0);
        return player_info->GetName();
      });
})
}
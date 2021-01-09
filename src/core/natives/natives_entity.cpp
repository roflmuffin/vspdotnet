#include "core/autonative.h"
#include "core/entity.h"
#include "core/script_engine.h"

#include <server_class.h>

namespace vspdotnet
{

int EntityGetPropInt(ScriptContext& script_context)
{
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  if (!entity)
  {
    script_context.ThrowNativeError("Invalid entity");
    return -1;
  }

  return entity->GetProp(type, name);
}

void EntitySetPropInt(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<int>(3);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return;
  }

  entity->SetProp(type, name, value);
}

float EntityGetPropFloat(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return -1;
  }

  return entity->GetPropFloat(type, name);
}

void EntitySetPropFloat(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<float>(3);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return;
  }

  entity->SetPropFloat(type, name, value);
}

Vector* EntityGetPropVector(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return nullptr;
  }

  return entity->GetPropVector(type, name);
}

void EntitySetPropVector(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<Vector*>(3);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return;
  }

  entity->SetPropVector(type, name, *value);
}

const char* EntityGetPropString(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return nullptr;
  }

  return entity->GetPropString(type, name);
}

void EntitySetPropString(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<const char*>(3);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return;
  }

  entity->SetPropString(type, name, value);
}

int EntityGetPropEnt(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return -1;
  }

  return entity->GetPropEnt(type, name);
}

void EntitySetPropEnt(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<int>(3);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return;
  }

  entity->SetPropEnt(type, name, value);
}

std::string last_kv;

const char* EntityGetKeyValue(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto name = script_context.GetArgument<const char*>(1);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return nullptr;
  }

  last_kv = entity->GetKeyValueStringRaw(name);
  auto ptr = last_kv.c_str();
  return ptr;
}

void EntitySetKeyValue(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  const auto name = script_context.GetArgument<const char*>(1);
  const auto value = script_context.GetArgument<const char*>(2);

  auto strValue = std::string(value);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
    return;
  }

  globals::server_tools->SetKeyValue(entity, name, value);
}

CBaseEntityWrapper* CreateEntityByClassname(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);

  CBaseEntity* pEntity = (CBaseEntity*)globals::server_tools->CreateEntityByName(name);
  return (CBaseEntityWrapper*)pEntity;
}

CBaseEntityWrapper* FindEntityByClassname(ScriptContext& script_context)
{
  auto startIndex = script_context.GetArgument<int>(0);
  auto searchname = script_context.GetArgument<const char*>(1);

  CBaseEntity* pEntity;

  if (startIndex == -1) {
    pEntity = static_cast<CBaseEntity*>(globals::server_tools->FirstEntity());
  } else {
    if (!BaseEntityFromIndex(startIndex, pEntity)) return nullptr;

    pEntity = reinterpret_cast<CBaseEntity*>(globals::server_tools->NextEntity(pEntity));
  }

  const char* classname;
  int lastletterpos;

  int offset =
      ((CBaseEntityWrapper*)pEntity)->FindDatamapPropertyOffset("m_iClassname");
  if (offset == -1) return nullptr;

  string_t s;

  while (pEntity) {
    if ((s = *(string_t*)((uint8_t*)pEntity + offset)) == NULL_STRING) {
      pEntity = (CBaseEntity*)globals::server_tools->NextEntity(pEntity);
      continue;
    }

    classname = STRING(s);

    lastletterpos = strlen(searchname) - 1;
    if (searchname[lastletterpos] == '*') {
      if (strnicmp(searchname, classname, lastletterpos) == 0) {
        return (CBaseEntityWrapper*)pEntity;
      }
    } else if (strcmpi(searchname, classname) == 0) {
      return (CBaseEntityWrapper*)pEntity;
    }

    pEntity = static_cast<CBaseEntity*>(globals::server_tools->NextEntity(pEntity));
  }

  return nullptr;
}

CBaseEntityWrapper* FindEntityByNetclass(ScriptContext& script_context) {
  auto startIndex = script_context.GetArgument<int>(0);
  auto classname = script_context.GetArgument<const char*>(1);

  int maxEntities = globals::gpGlobals->maxEntities;
  for (int i = startIndex; i < maxEntities; i++) {
    edict_t* current = ExcEdictFromIndex(i);
    if (current == nullptr || current->IsFree()) continue;

    IServerNetworkable* network = current->GetNetworkable();
    if (network == nullptr) continue;

    IHandleEntity* pHandleEnt = network->GetEntityHandle();
    if (pHandleEnt == nullptr) continue;

    ServerClass* sClass = network->GetServerClass();
    const char* name = sClass->GetName();

    CBaseEntity* pEntity;
    if (!strcmp(name, classname)) {
      if (!BaseEntityFromIndex(ExcIndexFromEdict(current), pEntity)) return nullptr;

      return (CBaseEntityWrapper*)pEntity;
    }
  }

  return nullptr;
}

const char* EntityGetClassname(ScriptContext& script_context)
{
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);

   if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
     return nullptr;
  }

  return entity->GetClassname();
}

void EntitySpawn(ScriptContext& script_context)
{
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
  }

  entity->Spawn();
}

bool EntityIsPlayer(ScriptContext& script_context)
{
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
  }

  return entity->IsPlayer();
}

bool EntityIsWeapon(ScriptContext& script_context) {
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);

  if (!entity) {
    script_context.ThrowNativeError("Invalid entity");
  }

  return entity->IsWeapon();
}

bool EntityIsNetworked(ScriptContext& script_context)
{
  auto entity = script_context.GetArgument<CBaseEntityWrapper*>(0);
  return entity->GetNetworkable();
}

REGISTER_NATIVES(entity, {
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_INT", EntityGetPropInt);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_FLOAT", EntityGetPropFloat);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_VECTOR", EntityGetPropVector);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_STRING", EntityGetPropString);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_ENT", EntityGetPropEnt);

  ScriptEngine::RegisterNativeHandler("ENTITY_SET_PROP_INT", EntitySetPropInt);
  ScriptEngine::RegisterNativeHandler("ENTITY_SET_PROP_FLOAT", EntitySetPropFloat);
  ScriptEngine::RegisterNativeHandler("ENTITY_SET_PROP_VECTOR", EntitySetPropVector);
  ScriptEngine::RegisterNativeHandler("ENTITY_SET_PROP_STRING", EntitySetPropString);
  ScriptEngine::RegisterNativeHandler("ENTITY_SET_PROP_ENT", EntitySetPropEnt);

  ScriptEngine::RegisterNativeHandler("ENTITY_GET_KEYVALUE", EntityGetKeyValue);
  ScriptEngine::RegisterNativeHandler("ENTITY_SET_KEYVALUE", EntitySetKeyValue);

  ScriptEngine::RegisterNativeHandler("ENTITY_CREATE_BY_CLASSNAME", CreateEntityByClassname);
  ScriptEngine::RegisterNativeHandler("ENTITY_FIND_BY_CLASSNAME", FindEntityByClassname);
  ScriptEngine::RegisterNativeHandler("ENTITY_FIND_BY_NETCLASS", FindEntityByNetclass);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_CLASSNAME", EntityGetClassname);
  ScriptEngine::RegisterNativeHandler("ENTITY_SPAWN", EntitySpawn);

  ScriptEngine::RegisterNativeHandler("ENTITY_IS_PLAYER", EntityIsPlayer);
  ScriptEngine::RegisterNativeHandler("ENTITY_IS_WEAPON", EntityIsWeapon);
  ScriptEngine::RegisterNativeHandler("ENTITY_IS_NETWORKED", EntityIsNetworked);
})
}

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

#include "core/autonative.h"
#include "core/entity.h"
#include "core/script_engine.h"

#include <server_class.h>
#include <core/main.h>


#include "datamap.h"
#include "core/player_manager.h"
#include "game/server/variant_t.h"

//
struct inputdata_t {
  CBaseEntity* pActivator;  // The entity that initially caused this chain of
                            // output events.
  CBaseEntity* pCaller;     // The entity that fired this particular output.
  variant_t value;          // The data parameter for this output.
  int nOutputID;            // The unique ID of the output that was fired.
};


namespace vspdotnet
{

inline bool IsValidEntity(ScriptContext& script_context, CBaseEntityWrapper*& entity) {
  const auto entity_index = script_context.GetArgument<int>(0);

  CBaseEntity* base_entity;
  if (!BaseEntityFromIndex(entity_index, base_entity)) {
    script_context.ThrowNativeError("Entity %d is invalid", entity_index);
    return false;
  }

  if (entity_index > 0 && entity_index <= globals::gpGlobals->maxClients)
  {
    auto player = globals::player_manager.GetPlayerByIndex(entity_index);
    if (!player || !player->IsConnected()) return false;
  }

  entity = reinterpret_cast<CBaseEntityWrapper*>(base_entity);

  return true;
}

int EntityGetProp(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return -1;

  const auto offset = script_context.GetArgument<int>(1);
  const auto size = script_context.GetArgument<int>(2);

  return entity->OffsetToInt(offset, size, true);
}

int EntityGetPropInt(ScriptContext& script_context)
{
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return -1;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  return entity->GetProp(type, name);
}

void EntitySetPropInt(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<int>(3);

  entity->SetProp(type, name, value);
}

void EntitySetProp(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  const auto offset = script_context.GetArgument<int>(1);
  const auto size = script_context.GetArgument<int>(2);
  const auto value = script_context.GetArgument<int>(3);

  entity->SetIntByOffset(offset, size, true, value);
}

float EntityGetPropFloat(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return -1;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  return entity->GetPropFloat(type, name);
}

void EntitySetPropFloat(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<float>(3);

  entity->SetPropFloat(type, name, value);
}

Vector* EntityGetPropVector(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return nullptr;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  return entity->GetPropVector(type, name);
}

void EntitySetPropVector(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<Vector*>(3);

  entity->SetPropVector(type, name, *value);
}

const char* EntityGetPropString(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return nullptr;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  return entity->GetPropString(type, name);
}

void EntitySetPropString(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<const char*>(3);

  entity->SetPropString(type, name, value);
}

int EntityGetPropEnt(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return -1;
  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);

  return entity->GetPropEnt(type, name);
}

int EntityGetPropEntByOffset(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return -1;

  auto offset = script_context.GetArgument<int>(1);

  auto handle = entity->GetPropertyPointerByOffset<CBaseHandle>(offset);
  return ExcIndexFromBaseHandle(*handle);
}


void EntitySetPropEnt(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  const auto type = script_context.GetArgument<PropType>(1);
  const auto name = script_context.GetArgument<const char*>(2);
  const auto value = script_context.GetArgument<int>(3);

  entity->SetPropEnt(type, name, value);
}

std::string last_kv;

const char* EntityGetKeyValue(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return nullptr;
  const auto name = script_context.GetArgument<const char*>(1);

  last_kv = entity->GetKeyValueStringRaw(name);
  auto ptr = last_kv.c_str();
  return ptr;
}

void EntitySetKeyValue(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;
  const auto name = script_context.GetArgument<const char*>(1);
  const auto value = script_context.GetArgument<const char*>(2);

  auto strValue = std::string(value);

  globals::server_tools->SetKeyValue(entity, name, value);
}

int CreateEntityByClassname(ScriptContext& script_context)
{
  auto name = script_context.GetArgument<const char*>(0);

  CBaseEntity* pEntity = (CBaseEntity*)globals::server_tools->CreateEntityByName(name);

  return ExcIndexFromBaseEntity(pEntity);
}

int FindEntityByClassname(ScriptContext& script_context)
{
  auto startIndex = script_context.GetArgument<int>(0);
  auto searchname = script_context.GetArgument<const char*>(1);

  CBaseEntity* pEntity;

  if (startIndex == -1) {
    pEntity = static_cast<CBaseEntity*>(globals::server_tools->FirstEntity());
  } else {
    if (!BaseEntityFromIndex(startIndex, pEntity)) return -1;
   

    pEntity = reinterpret_cast<CBaseEntity*>(globals::server_tools->NextEntity(pEntity));
  }

  if (!pEntity)
  {
    script_context.ThrowNativeError("Entity not found %s", searchname);   
    return -1;
  }

  const char* classname;
  int lastletterpos;

  int offset =
      ((CBaseEntityWrapper*)pEntity)->FindDatamapPropertyOffset("m_iClassname");
  if (offset == -1) return -1;

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
        return ExcIndexFromBaseEntity(pEntity);
      }
    } else if (strcmpi(searchname, classname) == 0) {
      return ExcIndexFromBaseEntity(pEntity);
    }

    pEntity = static_cast<CBaseEntity*>(globals::server_tools->NextEntity(pEntity));
  }

  return -1;
}

int FindEntityByNetclass(ScriptContext& script_context) {
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
      if (!BaseEntityFromIndex(ExcIndexFromEdict(current), pEntity)) return -1;      

      return ExcIndexFromBaseEntity(pEntity);
    }
  }

  return -1;
}

const char* EntityGetClassname(ScriptContext& script_context)
{
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return nullptr;

  return entity->GetClassname();
}

void EntitySpawn(ScriptContext& script_context)
{
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  entity->Spawn();
}

bool EntityIsPlayer(ScriptContext& script_context)
{
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return false;

  return entity->IsPlayer();
}

bool EntityIsWeapon(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return false;

  return entity->IsWeapon();
}


bool EntityIsNetworked(ScriptContext& script_context)
{
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return false;
  return entity->GetNetworkable();
}

bool EntityIsValid(ScriptContext& script_context)
{
  auto entity_index = script_context.GetArgument<int>(0);

  CBaseEntity* entity;
  return BaseEntityFromIndex(entity_index, entity) > 0;
}

int FindDataMapInfo(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return -1;

  auto prop_name = script_context.GetArgument<const char*>(1);

  return entity->FindDatamapPropertyOffset(prop_name);
}

void AcceptInput(ScriptContext& script_context) {
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  auto inputName = script_context.GetArgument<const char*>(1);

  const auto type_description =
      entity->FindDatamapPropertyDescription(inputName);

  if (!type_description) {
    script_context.ThrowNativeError("Input not found with name {0}", inputName);
    return;
  }

  inputdata_t pInputData;


  variant_t emptyVariant;
  pInputData.pActivator = nullptr;
  pInputData.pCaller = nullptr;
  pInputData.nOutputID = 0;
  pInputData.value = emptyVariant;

  (reinterpret_cast<CBaseEntity*>(entity)->*type_description->inputFunc)(
      pInputData);
}

void HookEntityOutput(ScriptContext& script_context)
{
  CBaseEntityWrapper* entity;
  if (!IsValidEntity(script_context, entity)) return;

  auto inputName = script_context.GetArgument<const char*>(1);

  const auto type_description = entity->FindDatamapPropertyDescription(inputName);

  if (!type_description) {
    script_context.ThrowNativeError("Input not found with name {0}", inputName);
    return;
  }

  
}

class CEntInfo {
 public:
  IHandleEntity* m_pEntity;
  int m_SerialNumber;
  CEntInfo* m_pPrev;
  CEntInfo* m_pNext;
  string_t m_iName;
  string_t m_iClassName;

  void ClearLinks();
};

CREATE_STATIC_GETTER_FUNCTION(MaxEntities, int, globals::gpGlobals->maxEntities);

void QueueTaskForNextFrame(ScriptContext& script_context)
{
  auto func = script_context.GetArgument<void*>(0);

  typedef void (voidfunc)(void);
  globals::vsp_plugin.AddTaskForNextFrame(
      [func]() {
        reinterpret_cast<voidfunc*>(func)();
  });
}

REGISTER_NATIVES(entity, {
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP", EntityGetProp);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_INT", EntityGetPropInt);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_FLOAT", EntityGetPropFloat);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_VECTOR", EntityGetPropVector);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_STRING", EntityGetPropString);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_ENT", EntityGetPropEnt);
  ScriptEngine::RegisterNativeHandler("ENTITY_GET_PROP_ENT_BY_OFFSET", EntityGetPropEntByOffset);

  ScriptEngine::RegisterNativeHandler("ENTITY_SET_PROP", EntitySetProp);
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
  ScriptEngine::RegisterNativeHandler("ENTITY_IS_VALID", EntityIsValid);

  ScriptEngine::RegisterNativeHandler("GET_MAX_ENTITIES", GetMaxEntities);

  ScriptEngine::RegisterNativeHandler("FIND_DATAMAP_INFO", FindDataMapInfo);
  ScriptEngine::RegisterNativeHandler("ACCEPT_INPUT", AcceptInput);
  ScriptEngine::RegisterNativeHandler("QUEUE_TASK_FOR_NEXT_FRAME", QueueTaskForNextFrame);
})
}

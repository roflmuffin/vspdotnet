#include "entity_listener.h"

#include "core/callback_manager.h"
#include "core/entity.h"
#include "core/globals.h"
#include "core/hooks.h"
#include "core/log.h"

typedef CBaseEntity CBaseCombatWeapon;
SH_DECL_MANUALHOOK3_void(Weapon_Drop, 287, 0, 0, CBaseCombatWeapon*, const Vector*, const Vector*);

namespace vspdotnet
{
bool UTIL_ContainsDataTable(SendTable* pTable, const char* name)
{
  const char* pname = pTable->GetName();
  int props = pTable->GetNumProps();
  SendProp* prop;
  SendTable* table;

  if (pname && strcmp(name, pname) == 0) return true;

  for (int i = 0; i < props; i++)
  {
    prop = pTable->GetProp(i);

    if ((table = prop->GetDataTable()) != NULL)
    {
      pname = table->GetName();
      if (pname && strcmp(name, pname) == 0)
      {
        return true;
      }

      if (UTIL_ContainsDataTable(table, name))
      {
        return true;
      }
    }
  }

  return false;
}

CBaseEntity* m_EntityCache[NUM_ENT_ENTRIES];

void EntityListener::OnEntityCreated(CBaseEntity* pEntity)
{
  auto index = ExcIndexFromBaseEntity(pEntity);

  // Index can be -1 for players before any players have connected.
  if (static_cast<unsigned>(index) == INVALID_EHANDLE_INDEX ||
      (index > 0 && index <= globals::gpGlobals->maxClients))
  {
    return;
  }

  if (!(index >= 0 && index < NUM_ENT_ENTRIES))
  {
    VSPDN_CORE_INFO("OnEntityCreated - Got entity index out of range (%d)",
                    index);
    return;
  }

  if (m_EntityCache[index] != pEntity)
  {
    HandleEntityCreated(pEntity, index);
  }
}


void EntityListener::OnEntitySpawned(CBaseEntity* pEntity)
{
  auto index = ExcIndexFromBaseEntity(pEntity);

  // Index can be -1 for players before any players have connected.
  if (static_cast<unsigned>(index) == INVALID_EHANDLE_INDEX ||
      (index > 0 && index <= globals::gpGlobals->maxClients))
  {
    return;
  }

  if (!(index >= 0 && index < NUM_ENT_ENTRIES))
  {
    VSPDN_CORE_INFO("OnEntitySpawned - Got entity index out of range (%d)",
                    index);
    return;
  }

  HandleEntitySpawned(pEntity, index);
}

void EntityListener::OnEntityDeleted(CBaseEntity* pEntity)
{
  auto index = ExcIndexFromBaseEntity(pEntity);

  // Index can be -1 for players before any players have connected.
  if (static_cast<unsigned>(index) == INVALID_EHANDLE_INDEX ||
      (index > 0 && index <= globals::gpGlobals->maxClients))
  {
    return;
  }

  HandleEntityDeleted(pEntity, index);
}

CHook* hook;
std::unordered_map<void*, void*> ecx_addresses2;

static bool TestHandler(HookType_t hook_type, CHook* hook)
{
  auto entity = hook->GetArgument<CBaseEntityWrapper*>(0);
  ecx_addresses2[hook->GetArgumentAddress(0)] = entity;
  auto weapon = hook->GetArgument<CBaseEntityWrapper*>(1);
  VSPDN_CORE_INFO("WeaponSwitch: {0} | {1} | {2}", entity->GetClassname(), weapon ? weapon->GetClassname() : "", entity->GetIndex());
  return false;
}

static bool TestHandlerPost(HookType_t hook_type, CHook* hook) {
  auto entity = reinterpret_cast<CBaseEntityWrapper*>(ecx_addresses2[hook->GetArgumentAddress(0)]);
  auto weapon = hook->GetArgument<CBaseEntityWrapper*>(1);
  VSPDN_CORE_INFO("WeaponSwitchPost: {0} | {1} | {2}", entity->GetClassname(),
                  weapon ? weapon->GetClassname() : "", entity->GetIndex());

  ecx_addresses2.erase(hook->GetArgumentAddress(0));
  return false;
}

static void Hook_WeaponDrop(CBaseCombatWeapon* pWeapon, const Vector* pvecTarget,
                            const Vector* pVelocity)
{
  auto entity = (META_IFACEPTR(CBaseEntityWrapper));
  auto weapon = reinterpret_cast<CBaseEntityWrapper*>(pWeapon);

  VSPDN_CORE_INFO("WeaponSwitch: {0} | {1} | {2}", entity->GetClassname(),
                  weapon ? weapon->GetClassname() : "", entity->GetIndex());
}

static void Hook_WeaponDropPost(CBaseCombatWeapon* pWeapon, const Vector* pvecTarget,
                            const Vector* pVelocity) {
  auto entity = (META_IFACEPTR(CBaseEntityWrapper));
  auto weapon = reinterpret_cast<CBaseEntityWrapper*>(pWeapon);

  VSPDN_CORE_INFO("WeaponSwitchPost: {0} | {1} | {2}", entity->GetClassname(),
                  weapon ? weapon->GetClassname() : "", entity->GetIndex());
}

void EntityListener::HandleEntityCreated(CBaseEntity* pEntity, int index)
{
  const char* pName = reinterpret_cast<CBaseEntityWrapper*>(pEntity)->GetClassname();
  if (!pName) pName = "";

  if (index == 1) {
    std::vector<DataType_t> vecArgTypes{DATA_TYPE_POINTER, DATA_TYPE_POINTER, DATA_TYPE_INT };
   ;
    auto prethinkfunc = hooks::GetVirtualFuncHelper((unsigned long)pEntity, 288);

    if (!hook) {
      /*hook = GetHookManager()->HookFunction((void*)prethinkfunc,
                                                 new x86MsThiscall(vecArgTypes, DATA_TYPE_VOID));

      hook->AddCallback(HOOKTYPE_PRE, (HookHandlerFn*)(void*)&TestHandler);
      hook->AddCallback(HOOKTYPE_POST, (HookHandlerFn*)(void*)&TestHandlerPost);*/

      /*SH_ADD_MANUALVPHOOK(Weapon_Drop, pEntity, SH_STATIC(Hook_WeaponDrop), false);
      SH_ADD_MANUALVPHOOK(Weapon_Drop, pEntity, SH_STATIC(Hook_WeaponDropPost), true);*/
    }
  }

  m_on_entity_created_callback_->ScriptContext().Reset();
  m_on_entity_created_callback_->ScriptContext().Push(index);
  m_on_entity_created_callback_->ScriptContext().Push(pName);
  m_on_entity_created_callback_->Execute();

  m_EntityCache[index] = pEntity;
}

void EntityListener::HandleEntitySpawned(CBaseEntity* pEntity, int index)
{
  const char* pName =
      reinterpret_cast<CBaseEntityWrapper*>(pEntity)->GetClassname();
  if (!pName) pName = "";

  m_on_entity_spawned_callback_->ScriptContext().Reset();
  m_on_entity_spawned_callback_->ScriptContext().Push(index);
  m_on_entity_spawned_callback_->ScriptContext().Push(pName);
  m_on_entity_spawned_callback_->Execute();
}

void EntityListener::HandleEntityDeleted(CBaseEntity* pEntity, int index)
{
  m_on_entity_deleted_callback_->ScriptContext().Reset();
  m_on_entity_deleted_callback_->ScriptContext().Push(index);
  m_on_entity_deleted_callback_->Execute();
}

void EntityListener::OnAllInitialized()
{
  m_on_entity_created_callback_ = globals::callback_manager.CreateCallback("OnEntityCreated");
  m_on_entity_spawned_callback_ = globals::callback_manager.CreateCallback("OnEntitySpawned");
  m_on_entity_deleted_callback_ = globals::callback_manager.CreateCallback("OnEntityDeleted");
}
}

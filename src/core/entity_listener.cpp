#include "entity_listener.h"

#include <server_class.h>

#include "globals.h"

#include <sourcehook/sourcehook.h>

#include "entity.h"
#include "hook.h"
#include "hooks.h"
#include "log.h"
#include "manager.h"
#include "datamaps.h"

namespace vspdotnet {

  bool UTIL_ContainsDataTable(SendTable *pTable, const char *name) {
    const char *pname = pTable->GetName();
    int props = pTable->GetNumProps();
    SendProp *prop;
    SendTable *table;

    if (pname && strcmp(name, pname) == 0) return true;

    for (int i = 0; i < props; i++) {
      prop = pTable->GetProp(i);

      if ((table = prop->GetDataTable()) != NULL) {
        pname = table->GetName();
        if (pname && strcmp(name, pname) == 0) {
          return true;
        }

        if (UTIL_ContainsDataTable(table, name)) {
          return true;
        }
      }
    }

    return false;
  }

    SH_DECL_MANUALHOOK0_void(PreThink, 371, 0, 0);
    SH_DECL_MANUALHOOK0_void(Think, 52, 0, 0);
    SH_DECL_MANUALHOOK0_void(PostThink, 372, 0, 0);

    static void Hook_PreThink()
    {
      auto ptr = META_IFACEPTR(CBaseEntityWrapper);
      auto classname = ptr->GetClassname();
      if (strcmp(classname, "player") == 0)
      {
        bool inBombZone = ptr->GetProp(Send, "m_bInBombZone");
        VSPDN_CORE_INFO("PreThink called by entity: {0} | inBombZone: {1}", (void *)ptr, inBombZone);        
      }
    }

    static void Hook_PostThink() {
      auto ptr = META_IFACEPTR(CBaseEntityWrapper);
      auto classname = ptr->GetClassname();
      if (strcmp(classname, "player") == 0) {
        bool inBombZone = ptr->GetProp(Send, "m_bInBombZone");
        VSPDN_CORE_INFO("PostThink called by entity: {0} | inBombZone: {1}",
                        (void *)ptr, inBombZone);
      }
    }

    static void Hook_Think() {
      auto ptr = META_IFACEPTR(CBaseEntityWrapper);
      auto classname = ptr->GetClassname();
      if (strcmp(classname, "player") == 0) {
        bool inBombZone = ptr->GetProp(Send, "m_bInBombZone");
        VSPDN_CORE_INFO("Think called by entity: {0} | inBombZone: {1}",
                        (void *)ptr, inBombZone);
      }
    }

    static void Hook_Reload()
    {
    }


    static bool NewHandler(HookType_t hook_type, CHook *hook) {
      auto ptr = hook->GetArgument<CBaseEntityWrapper *>(0);
      auto networked = ptr->GetNetworkable();
      edict_t *pEdict = networked->GetEdict();

      bool inBombZone = ptr->GetProp(Send, "m_bInBombZone");
      VSPDN_CORE_INFO("Think called by entity: {0} | inBombZone: {1}",
                      (void *)ptr, inBombZone);
      return false;
    }

    void EntityListener::Setup()
    {
      //SH_MANUALHOOK_RECONFIGURE(PreThink, 371, 0, 0);
    }

    void EntityListener::OnEntityCreated(CBaseEntity *pEntity) {
      /*SH_ADD_MANUALVPHOOK(PreThink, pEntity, SH_STATIC(Hook_PreThink), false);
      SH_ADD_MANUALVPHOOK(PostThink, pEntity, SH_STATIC(Hook_PostThink), false);
      SH_ADD_MANUALVPHOOK(Think, pEntity, SH_STATIC(Hook_Think), false);
      SH_ADD_MANUALVPHOOK(Reload, pEntity, SH_STATIC(Hook_Reload), false);*/
      auto wrapped = reinterpret_cast<CBaseEntityWrapper*>(pEntity);

      auto descMap = wrapped->GetDataDescMap();

      bool isPlayer = UTIL_ContainsDataTable(wrapped->GetServerClass()->m_pTable,
                             "DT_BasePlayer");

      if (!isPlayer) return;
      if (!wrapped->GetBaseEntity()) return;

      CHookManager *hook_manager = GetHookManager();


      hooks::MemFuncInfo info = {true, 0, 52,
                                 0};  // hooks::GetFunctionInfo(func);
      unsigned long myaddr = hooks::GetVirtualFuncHelper(
          (unsigned long)pEntity + info.vtbloffs, info.vtblindex);

      std::vector<DataType_t> vecArgTypes;
      vecArgTypes.push_back(DATA_TYPE_POINTER);

      /*va_list vl;
      va_start(vl, numArgs);
      for (int i = 0; i < numArgs; i++) {
        int type = va_arg(vl, int);
        vecArgTypes.push_back((DataType_t)type);
      }
      va_end(vl);*/

      bool post = false;

      CHook *pHook = hook_manager->HookFunction(
          (void *)myaddr, new x86MsThiscall(vecArgTypes, DATA_TYPE_VOID));

      pHook->AddCallback(post ? HOOKTYPE_POST : HOOKTYPE_PRE,
                         (HookHandlerFn *)(void *)NewHandler);

      VSPDN_CORE_TRACE("Entity Created: {0}", (void*)pEntity);
    }

    void EntityListener::OnEntitySpawned(CBaseEntity *pEntity) {
        VSPDN_CORE_TRACE("Entity Spawned: {0}", (void*)pEntity);
    }

    void EntityListener::OnEntityDeleted(CBaseEntity *pEntity) {
        VSPDN_CORE_TRACE("Entity Deleted: {0}", (void*)pEntity);
    }
}

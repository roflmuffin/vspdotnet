#include "core/interfaces.h"

#include <public/eiface.h>
#include <public/engine/IEngineSound.h>
#include <public/engine/IEngineTrace.h>
#include <public/game/server/iplayerinfo.h>
#include <public/igameevents.h>
#include <public/ivoiceserver.h>
#include <public/networkstringtabledefs.h>
#include <public/toolframework/itoolentity.h>
#include <public/vphysics_interface.h>
#include <public/vstdlib/random.h>

#include "core/globals.h"

namespace vspdotnet {
  namespace interfaces {

    InterfaceHelper_t engine_interfaces[] = {
      {INTERFACEVERSION_VENGINESERVER, (void**)&globals::engine},
      {INTERFACEVERSION_GAMEEVENTSMANAGER2, (void**)&globals::gameeventmanager},
      {INTERFACEVERSION_ISERVERPLUGINHELPERS, (void**)&globals::helpers},
      {INTERFACEVERSION_ENGINETRACE_SERVER, (void**)&globals::engine_trace},
      {IENGINESOUND_SERVER_INTERFACE_VERSION, (void**)&globals::engine_sound},
      {INTERFACENAME_NETWORKSTRINGTABLESERVER, (void**)&globals::net_string_tables},
      {VENGINE_SERVER_RANDOM_INTERFACE_VERSION, (void**)&globals::random_stream},
      {FILESYSTEM_INTERFACE_VERSION, (void**)&globals::filesystem},
      {VPHYSICS_INTERFACE_VERSION, (void**)&globals::physics},
      {VPHYSICS_COLLISION_INTERFACE_VERSION, (void**)&globals::physics_collision},
      {VPHYSICS_SURFACEPROPS_INTERFACE_VERSION, (void**)&globals::physics_surface_props},
      {MDLCACHE_INTERFACE_VERSION, (void**)&globals::model_cache},
      {INTERFACEVERSION_VOICESERVER, (void**)&globals::voice_server},
      {NULL, NULL}
    };

    InterfaceHelper_t game_interfaces[] = {
      {INTERFACEVERSION_PLAYERINFOMANAGER, (void**)&globals::playerinfo_manager},
      {INTERFACEVERSION_PLAYERBOTMANAGER, (void**)&globals::bot_manager},
      {INTERFACEVERSION_SERVERGAMEDLL, (void**)&globals::server_game_dll},
      {INTERFACEVERSION_SERVERGAMECLIENTS, (void**)&globals::server_game_clients},
      {VSERVERTOOLS_INTERFACE_VERSION, (void**)&globals::server_tools},
      {NULL, NULL}
    };

    bool InitializeInterfaces(InterfaceHelper_t* interface_list, CreateInterfaceFn factory) {
      InterfaceHelper_t* interface_helper = interface_list;
      while (interface_helper->interface)
      {
        void** pGlobal = interface_helper->global;

        // Get the interface from the given factory.
        *pGlobal = factory(interface_helper->interface, NULL);

        // If it's not valid, bail out.
        if (!pGlobal) {
          Warning("[Source .NET] Could not retrieve interface %s\n", interface_helper->interface);
          return false;
        }

        interface_helper++;
      }

      return true;

    }

    bool InitializeEngineInterfaces(CreateInterfaceFn factory) {
      return InitializeInterfaces(engine_interfaces, factory);
    }

    bool InitializeGameInterfaces(CreateInterfaceFn factory) {
      return InitializeInterfaces(game_interfaces, factory);
    }
  }
}

#include "core/globals.h"

#include <sourcehook/sourcehook.h>
#include <sourcehook/sourcehook_impl.h>

#include "core/callback_manager.h"
#include "core/chat_commands.h"
#include "core/convar_manager.h"
#include "core/con_command_manager.h"
#include "core/dotnet_host.h"
#include "core/entity_listener.h"
#include "core/event_manager.h"
#include "core/hook_manager.h"
#include "core/log.h"
#include "core/main.h"
#include "core/menu_manager.h"
#include "core/player_manager.h"
#include "core/sig_scanner.h"
#include "core/timer_system.h"
#include "core/user_message_manager.h"

#include <public/game/server/iplayerinfo.h>

namespace vspdotnet {

  template <typename T>
  inline T* GetObjectBySignature(CBinaryFile* binary, unsigned char* signature, int size, int address_offset) {
     unsigned long addr = binary->FindSignature(signature, size);
     auto obj = *(T **)(addr + address_offset);
     return obj;
  }

  namespace globals {
    IVEngineServer *engine = nullptr;
    IGameEventManager2 *gameeventmanager = nullptr;
    IPlayerInfoManager *playerinfo_manager = nullptr;
    IBotManager *bot_manager = nullptr;
    IServerPluginHelpers *helpers = nullptr;
    IUniformRandomStream *random_stream = nullptr;
    IEngineTrace *engine_trace = nullptr;
    IEngineSound *engine_sound = nullptr;
    INetworkStringTableContainer *net_string_tables = nullptr;
    CGlobalVars *gpGlobals = nullptr;
    IFileSystem *filesystem = nullptr;
    IServerGameDLL *server_game_dll = nullptr;
    IServerGameClients *server_game_clients = nullptr;
    IServerTools *server_tools = nullptr;
    IPhysics *physics = nullptr;
    IPhysicsCollision *physics_collision = nullptr;
    IPhysicsSurfaceProps *physics_surface_props = nullptr;
    IMDLCache *model_cache = nullptr;
    IVoiceServer *voice_server = nullptr;
    MyServerPlugin vsp_plugin;
    CDotNetManager dotnet_manager;
    EntityListener entity_listener;
    ICvar *cvars = nullptr;
    IServer *server = nullptr;
    CGlobalEntityList* global_entity_list = nullptr;
    EventManager event_manager;
    UserMessageManager user_message_manager;
    ConCommandManager con_command_manager;
    CallbackManager callback_manager;
    PlayerManager player_manager;
    MenuManager menu_manager;
    ConVarManager convar_manager;
    TimerSystem timer_system;
    ChatCommands chat_commands;
    HookManager hook_manager;
    SourceHook::Impl::CSourceHookImpl source_hook_impl;
    SourceHook::ISourceHook *source_hook = &source_hook_impl;
    int source_hook_pluginid = 0;
    CBinaryFile *server_binary = nullptr;
    CBinaryFile *engine_binary = nullptr;

    bool InitializeGlobals() {
      gpGlobals = playerinfo_manager->GetGlobalVars();
      cvars = g_pCVar;
      if (!gpGlobals)
        return false;

      engine_binary = FindBinary("bin/engine", false);
      server_binary = FindBinary("server", false);

      unsigned char iserversig[18] = {0x55, 0x8B, 0xEC, 0x56, 0xFF, 0x2A,
                                      0x2A, 0xB9, 0x2A, 0x2A, 0x2A, 0x2A,
                                      0xE8, 0x2A, 0x2A, 0x2A, 0x2A, 0x8B};

      unsigned char globalentitylistsig[] = {0x55, 0x8B, 0xEC, 0x8B, 0x0D,
                                             0x2A, 0x2A, 0x2A, 0x2A, 0x53};

      server = GetObjectBySignature<IServer>(engine_binary, iserversig, 18, 8);
      global_entity_list = GetObjectBySignature<CGlobalEntityList>(server_binary, globalentitylistsig, sizeof(globalentitylistsig), 101);

      if (!server)
      {
        VSPDN_CORE_ERROR("Could not find reference to IServer with signature {0}", iserversig);
        return false;
      }

      if (!global_entity_list) {
        VSPDN_CORE_ERROR( "Could not find reference to CGlobalEntityList with signature {0}", globalentitylistsig);
        return false;
      }

      VSPDN_CORE_INFO("IServer Address: {0}, CGlobalEntityList Address: {1}",
                      (void *)globals::server, (void *)globals::global_entity_list);

      global_entity_list->AddListenerEntity(&entity_listener);

      return true;
    }
  }
}

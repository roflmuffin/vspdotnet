#include "globals.h"
#include "main.h"
#include "dotnet_host.h"
#include "entity_listener.h"
#include "event_manager.h"

#include <public/game/server/iplayerinfo.h>
#include <sourcehook/sourcehook.h>
#include <sourcehook/sourcehook_impl.h>

#include "user_message_manager.h"
#include "con_command_manager.h"
#include "callback_manager.h"
#include "convar_manager.h"
#include "player_manager.h"

namespace vspdotnet {
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
    CEventManager event_manager;
    UserMessageManager user_message_manager;
    ConCommandManager con_command_manager;
    CCallbackManager callback_manager;
    PlayerManager player_manager;
    ConVarManager convar_manager;
    SourceHook::Impl::CSourceHookImpl source_hook_impl;
    SourceHook::ISourceHook *source_hook = &source_hook_impl;
    int source_hook_pluginid = 0;

    bool InitializeGlobals() {
      gpGlobals = playerinfo_manager->GetGlobalVars();
      cvars = g_pCVar;
      if (!gpGlobals)
        return false;

      return true;
    }
  }
}

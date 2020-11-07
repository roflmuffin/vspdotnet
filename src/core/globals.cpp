#include "globals.h"

#include <public/game/server/iplayerinfo.h>

namespace vspdotnet {
  namespace globals {
    IVEngineServer *engine = nullptr;
    IGameEventManager2 *gameevent_manager = nullptr;
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
    ICvar *cvars = nullptr;
    IServer *server = nullptr;
    CGlobalEntityList* global_entity_list = nullptr;

    bool InitializeGlobals() {
      gpGlobals = playerinfo_manager->GetGlobalVars();
      cvars = g_pCVar;
      if (!gpGlobals)
        return false;

      return true;
    }
  }
}

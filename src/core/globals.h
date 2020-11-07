#ifndef __GLOBALS_H_
#define __GLOBALS_H_

#include "main.h"

class IVEngineServer;
class IGameEventManager2;
class IPlayerInfoManager;
class IBotManager;
class IServerPluginHelpers;
class IUniformRandomStream;
class IEngineTrace;
class IEngineSound;
class INetworkStringTableContainer;
class CGlobalVars;
class IFileSystem;
class IServerGameDLL;
class IServerGameClients;
class IServerTools;
class IPhysics;
class IPhysicsCollision;
class IPhysicsSurfaceProps;
class IMDLCache;
class IVoiceServer;
class MyServerPlugin;
class IServer;
class CGlobalEntityList;

namespace vspdotnet {
  namespace globals {
    extern IVEngineServer *engine;
    extern IGameEventManager2 *gameevent_manager;
    extern IPlayerInfoManager *playerinfo_manager;
    extern IBotManager *bot_manager;
    extern IServerPluginHelpers *helpers;
    extern IUniformRandomStream *random_stream;
    extern IEngineTrace *engine_trace;
    extern IEngineSound *engine_sound;
    extern INetworkStringTableContainer *net_string_tables;
    extern CGlobalVars *gpGlobals;
    extern IFileSystem *filesystem;
    extern IServerGameDLL *server_game_dll;
    extern IServerGameClients *server_game_clients;
    extern IServerTools *server_tools;
    extern IPhysics *physics;
    extern IPhysicsCollision *physics_collision;
    extern IPhysicsSurfaceProps *physics_surface_props;
    extern IMDLCache *model_cache;
    extern IVoiceServer *voice_server;
    extern MyServerPlugin vsp_plugin;
    extern ICvar *cvars;
    extern IServer *server;
    extern CGlobalEntityList *global_entity_list;

    bool InitializeGlobals();
  }
}


#endif // __GLOBALS_H_

#ifndef __GLOBALS_H_
#define __GLOBALS_H_

//#include "main.h"
#include "convar_hack.h"
#include <sourcehook/sourcehook.h>

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
class CDotNetManager;
class ICvar;

namespace vspdotnet {
class EntityListener;
class CEventManager;
class UserMessageManager;
class ConCommandManager;
class CCallbackManager;
class ConVarManager;
class MyServerPlugin;
class PlayerManager;

namespace globals {
extern IVEngineServer *engine;
extern IGameEventManager2 *gameeventmanager;
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
extern CDotNetManager dotnet_manager;
extern ICvar *cvars;
extern IServer *server;
extern CGlobalEntityList *global_entity_list;
extern EntityListener entity_listener;
extern CEventManager event_manager;
extern UserMessageManager user_message_manager;
extern ConCommandManager con_command_manager;
extern CCallbackManager callback_manager;
extern ConVarManager convar_manager;
extern PlayerManager player_manager;
extern SourceHook::ISourceHook *source_hook;
extern int source_hook_pluginid;

bool InitializeGlobals();
}  // namespace globals

  class GlobalClass {
   public:
    virtual ~GlobalClass() = default;

    GlobalClass() {
      m_pGlobalClassNext = GlobalClass::head;
      GlobalClass::head = this;
    }

   public:
    virtual void OnStartup() {}
    virtual void OnShutdown() {}
    virtual void OnAllInitialized() {}
    virtual void OnAllInitialized_Post() {}
    virtual void OnLevelChange(const char *mapName) {}
    virtual void OnLevelEnd() {}

   public:
    GlobalClass *m_pGlobalClassNext;
    static GlobalClass *head;
  };
}

#undef SH_GLOB_SHPTR
#define SH_GLOB_SHPTR vspdotnet::globals::source_hook
#undef SH_GLOB_PLUGPTR
#define SH_GLOB_PLUGPTR vspdotnet::globals::source_hook_pluginid

#endif // __GLOBALS_H_

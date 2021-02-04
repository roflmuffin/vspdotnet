#pragma once

//#include "main.h"
#include "convar_hack.h"
#include "core/global_listener.h"
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
class EventManager;
class UserMessageManager;
class ConCommandManager;
class CallbackManager;
class ConVarManager;
class MyServerPlugin;
class PlayerManager;
class CBinaryFile;
class MenuManager;
class TimerSystem;
class ChatCommands;
class HookManager;

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
extern EventManager event_manager;
extern UserMessageManager user_message_manager;
extern ConCommandManager con_command_manager;
extern CallbackManager callback_manager;
extern ConVarManager convar_manager;
extern PlayerManager player_manager;
extern MenuManager menu_manager;
extern TimerSystem timer_system;
extern ChatCommands chat_commands;
extern HookManager hook_manager;
extern SourceHook::ISourceHook *source_hook;
extern int source_hook_pluginid;
extern CBinaryFile *server_binary;
extern CBinaryFile *engine_binary;

bool InitializeGlobals();
}  // namespace globals

}

#undef SH_GLOB_SHPTR
#define SH_GLOB_SHPTR vspdotnet::globals::source_hook
#undef SH_GLOB_PLUGPTR
#define SH_GLOB_PLUGPTR vspdotnet::globals::source_hook_pluginid
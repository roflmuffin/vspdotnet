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

// convar_hack must be first header file
#include "core/convar_hack.h"

#include "core/main.h"

#include <public/igameevents.h>
#include <public/iserver.h>

#include <iomanip>
#include <iostream>

#include "core/callback_manager.h"
#include "core/convar_manager.h"
#include "core/con_command_manager.h"
#include "core/dotnet_host.h"
#include "core/entity.h"
#include "core/globals.h"
#include "core/interfaces.h"
#include "core/log.h"
#include "core/script_engine.h"
#include "core/timer_system.h"
#include "core/user_message_manager.h"
#include "core/utils.h"
#include "core/utilities/conversions.h"


vspdotnet::GlobalClass * vspdotnet::GlobalClass::head = nullptr;

extern "C" __declspec(dllexport) void InvokeNative(vspdotnet::fxNativeContext &context) {
  if (context.nativeIdentifier == 0) return;
  vspdotnet::ScriptEngine::InvokeNative(context);
}

namespace vspdotnet {

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(MyServerPlugin, IServerPluginCallbacks,
                                  INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
                                  globals::vsp_plugin);

bool MyServerPlugin::Load(CreateInterfaceFn interfaceFactory,
                          CreateInterfaceFn gameServerFactory) {
  vspdotnet::Log::Init();

  ConnectInterfaces(&interfaceFactory, 1);

  if (!vspdotnet::interfaces::InitializeEngineInterfaces(interfaceFactory))
    return false;

  VSPDN_CORE_INFO("Successfully loaded engine interfaces.");

  if (!vspdotnet::interfaces::InitializeGameInterfaces(gameServerFactory))
    return false;

  VSPDN_CORE_INFO("Successfully loaded game interfaces.");
  if (!vspdotnet::globals::InitializeGlobals()) return false;

  VSPDN_CORE_INFO("Initializing MathLib...");
  MathLib_Init(2.2f, 2.2f, 0.f, 2.0f);

  VSPDN_CORE_INFO("Initializing commands...");
  convar::InitServerCommands();

  CALL_GLOBAL_LISTENER(OnAllInitialized())

  if (!globals::dotnet_manager.Initialize())
  {
    VSPDN_CORE_ERROR("Failed to load vspdotnet");
    return false;
  }

  VSPDN_CORE_INFO("Loaded successfully");

  CallbackT test = [](fxNativeContext *context) {
    VSPDN_CORE_INFO("Timer called!, current time: {0}", globals::gpGlobals->curtime);
  };
  globals::timer_system.CreateTimer(20.0f, test, 0);

  return true;
}

void MyServerPlugin::Unload() {
  // This will crash because CLR does not allow unloading and the srcds server unloads our dll, which in turns unloads CLR.
  globals::dotnet_manager.Shutdown();
  vspdotnet::Log::Close();
}

void MyServerPlugin::Pause() {}

void MyServerPlugin::UnPause() {}

const char *MyServerPlugin::GetPluginDescription() { return "Source.NET"; }

void MyServerPlugin::ServerActivate(edict_t *pEdictList, int edictCount,
                                    int clientMax) {
  VSPDN_CORE_TRACE("name={0},arg1={1},arg2={2}\n", "ServerActivate", edictCount,
                   clientMax);
}

static ScriptCallback *on_map_end_callback;
void MyServerPlugin::GameFrame(bool simulating)
{ }

static bool NewLevelStarted = false;
void MyServerPlugin::LevelShutdown() {
  if (NewLevelStarted)
  {
    CALL_GLOBAL_LISTENER(OnLevelEnd())

    if (on_map_end_callback && on_map_end_callback->GetFunctionCount()) {
      on_map_end_callback->ScriptContext().Reset();
      on_map_end_callback->Execute();
    }

    globals::timer_system.RemoveMapChangeTimers();

    VSPDN_CORE_TRACE("name={0}", "LevelShutdown");
    NewLevelStarted = false;
  }
}

void MyServerPlugin::LevelInit(const char *pMapName) {
  VSPDN_CORE_TRACE("name={0},mapname={1}", "LevelInit", pMapName);
  NewLevelStarted = true;

  if (!on_map_end_callback) {
    on_map_end_callback = globals::callback_manager.CreateCallback("OnMapEnd");
  }
}


void MyServerPlugin::ClientActive(edict_t *pEntity) {
  VSPDN_CORE_TRACE("name={0},edict={1}", "ClientActive", (void *)pEntity);
}

void MyServerPlugin::ClientFullyConnect(edict_t *pEntity) {
  VSPDN_CORE_TRACE("name={0},edict={1}", "ClientFullyConnect", (void *)pEntity);
}

void MyServerPlugin::ClientDisconnect(edict_t *pEntity) {
  VSPDN_CORE_TRACE("name={0},edict={1}", "ClientDisconnect", (void *)pEntity);
}

void MyServerPlugin::ClientPutInServer(edict_t *pEntity,
                                       const char *playername) {
  VSPDN_CORE_TRACE("name={0},edict={1},playername={2}", "ClientPutInServer",
                   (void *)pEntity, playername);
}

void MyServerPlugin::SetCommandClient(int index) {
  VSPDN_CORE_TRACE("name={0},index={1}", "SetCommandClient", index);
}

void MyServerPlugin::ClientSettingsChanged(edict_t *pEdict) {
  VSPDN_CORE_TRACE("name={0},edict={1}", "ClientSettingsChanged",
                   (void *)pEdict);
}

PLUGIN_RESULT MyServerPlugin::ClientConnect(bool *bAllowConnect,
                                            edict_t *pEntity,
                                            const char *pszName,
                                            const char *pszAddress,
                                            char *reject, int maxrejectlen) {
  VSPDN_CORE_TRACE("name={0},edict={1},name={2},address={3}", "ClientConnect",
                   (void *)pEntity, pszName, pszAddress);
  return PLUGIN_CONTINUE;
}

PLUGIN_RESULT MyServerPlugin::ClientCommand(edict_t *pEntity,
                                            const CCommand &args) {
  VSPDN_CORE_TRACE("name={0},edict={1},command{2}", "ClientCommand",
                   (void *)pEntity, args.Arg(0));

  return PLUGIN_CONTINUE;
}

PLUGIN_RESULT MyServerPlugin::NetworkIDValidated(const char *pszUserName,
                                                 const char *pszNetworkID) {
  VSPDN_CORE_TRACE("name={0},name={1},networkid={2}", "NetworkIDValidated",
                   pszUserName, pszNetworkID);
  return PLUGIN_CONTINUE;
}

void MyServerPlugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie,
                                              edict_t *pPlayerEntity,
                                              EQueryCvarValueStatus eStatus,
                                              const char *pCvarName,
                                              const char *pCvarValue) {
  VSPDN_CORE_TRACE("name={0},edict={1},status={2},cvar_name={3},cvar_value={4}",
                   "OnQueryCvarValueFinished", (void *)pPlayerEntity, eStatus,
                   pCvarName, pCvarValue);
}

void MyServerPlugin::OnEdictAllocated(edict_t *edict) {}

void MyServerPlugin::OnEdictFreed(const edict_t *edict) {}

bool MyServerPlugin::BNetworkCryptKeyCheckRequired(
    uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient,
    bool bClientWantsToUseCryptKey) {
  return false;
}

bool MyServerPlugin::BNetworkCryptKeyValidate(
    uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient,
    int nEncryptionKeyIndexFromClient, int numEncryptedBytesFromClient,
    byte *pbEncryptedBufferFromClient, byte *pbPlainTextKeyForNetchan) {
  return false;
}

  void MyServerPlugin::OnThink(bool last_tick) {
    globals::timer_system.OnGameFrame(last_tick);

    if (m_nextTasks.empty())
      return;

    VSPDN_CORE_TRACE("Executing queued tasks of size: {0} on tick number {1}", m_nextTasks.size(), globals::gpGlobals->tickcount);

    for (int i = 0; i < m_nextTasks.size(); i++) {
      m_nextTasks[i]();
    }

    m_nextTasks.clear();
  }


  void MyServerPlugin::AddTaskForNextFrame(std::function<void()> &&task) {
    m_nextTasks.push_back(std::forward<decltype(task)>(task));
  }
  }  // namespace vspdotnet

IChangeInfoAccessor *CBaseEdict::GetChangeAccessor() {
  return vspdotnet::globals::engine->GetChangeAccessor((const edict_t *)this);
}
#include "main.h"

#include <conventions/x86GccCdecl.h>
#include <conventions/x86GccThiscall.h>
#include <hook.h>
#include <IEngineTrace.h>
#include <manager.h>
#include <public/igameevents.h>
#include <public/iserver.h>

#include <iostream>
//#include <optional>

#include <iomanip>

#include "entity_listener.h"
#include "globals.h"
#include "hooks.h"
#include "interfaces.h"
#include "log.h"
#include "sig_scanner.h"
#include "script_engine.h"
#include "utils.h"
#include "dotnet_host.h"
#include "autonative.h"
#include "user_message_manager.h"

vspdotnet::GlobalClass * vspdotnet::GlobalClass::head = nullptr;

extern "C" __declspec(dllexport) void InvokeNative(vspdotnet::fxNativeContext &context) {
  if (context.nativeIdentifier == 0) return;
  vspdotnet::ScriptEngine::InvokeNative(context);
}

namespace vspdotnet {

class CPluginConVarAccessor : public IConCommandBaseAccessor {
 public:
  virtual bool RegisterConCommandBase(ConCommandBase *pCommand) {
    globals::cvars->RegisterConCommand(pCommand);
    return true;
  }
};

CPluginConVarAccessor g_ConVarAccessor;

void InitServerCommands() { ConVar_Register(0, &g_ConVarAccessor); }

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(MyServerPlugin, IServerPluginCallbacks,
                                  INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
                                  globals::vsp_plugin);

MyServerPlugin::MyServerPlugin() {}

MyServerPlugin::~MyServerPlugin() {}

static float GetCurrentTime(ScriptContext& context) {
  return globals::server->GetTime();
}

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
  InitServerCommands();

   CBinaryFile *engineBinary = vspdotnet::FindBinary("bin/engine", false);
   CBinaryFile *serverBinary = vspdotnet::FindBinary("server", false);

   VSPDN_CORE_INFO("Engine Binary: {0}", (void *)engineBinary);
   VSPDN_CORE_INFO("Server Binary: {0}", (void *)serverBinary);

  unsigned char iserversig[18] = {0x55, 0x8B, 0xEC, 0x56, 0xFF, 0x2A, 0x2A, 0xB9, 0x2A, 0x2A, 0x2A, 0x2A, 0xE8, 0x2A, 0x2A, 0x2A, 0x2A, 0x8B};
  unsigned long addr = engineBinary->FindSignature(iserversig, sizeof(iserversig));

  unsigned char globalentitylistsig[] = {
      0x55, 0x8B, 0xEC, 0x8B, 0x0D, 0x2A, 0x2A, 0x2A, 0x2A, 0x53};
  unsigned long entlistaddr = serverBinary->FindSignature(
      globalentitylistsig,
      sizeof(globalentitylistsig) / sizeof(globalentitylistsig[0]));

  globals::server = *(IServer **)(addr + 8); // 13 on windows
  globals::global_entity_list = *(CGlobalEntityList **)(entlistaddr + 101);

  globals::global_entity_list->AddListenerEntity(&vspdotnet::globals::entity_listener);

  VSPDN_CORE_INFO("Server Address: {0}, Entity List Address: {1}",
                  (void *)globals::server, (void *)globals::global_entity_list);

  GlobalClass *pBase = GlobalClass::head;
  pBase = GlobalClass::head;
  while (pBase) {
    pBase->OnAllInitialized();
    pBase = pBase->m_pGlobalClassNext;
  }

  globals::entity_listener.Setup();

  ScriptEngine::RegisterNativeHandler<void>(
      "PRINT_TO_CHAT", [](ScriptContext &script_context) {
        int index = script_context.GetArgument<int>(0);
        const char *message = script_context.GetArgument<const char *>(1);

        globals::user_message_manager.SendMessageToChat(index, message);
      });

  ScriptEngine::RegisterNativeHandler<void>(
      "PRINT_TO_CONSOLE", [](ScriptContext &script_context) {
        const char *message = script_context.GetArgument<const char *>(0);

        Msg("%s", message);
      });

    ScriptEngine::RegisterNativeHandler<void>(
      "PRINT_TO_HINT", [](ScriptContext &script_context) {
        int index = script_context.GetArgument<int>(0);
        const char *message = script_context.GetArgument<const char *>(1);

        globals::user_message_manager.SendHintMessage(index, message);
      });

  if (!globals::dotnet_manager.Initialize())
  {
    VSPDN_CORE_ERROR("Failed to load vspdotnet");
    return false;
  }

  VSPDN_CORE_INFO("Loaded successfully");

  return true;
}

bool NewHandler(HookType_t hook_type, CHook *hook) {
  VSPDN_CORE_INFO("Inside our new handler.");
  hook->SetReturnValue(5000);
  return false;
}

CON_COMMAND(testing, "testing") {
      HookHandlerFn test = [](HookType_t hook_type, CHook *hook) {
          VSPDN_CORE_INFO("Inside our new handler.");
          //hook->SetReturnValue(rand() % 10000);
          return false;
      };

      // hooks::CreateHook(globals::server, &IServer::GetUDPPort, test, true,
      //                   DATA_TYPE_INT, 0);
      // //hooks::CreateHook(globals::server, &IServer::)

      /*
      auto del = fastdelegate::MakeDelegate(vspdotnet::globals::vsp_plugin, &MyServerPlugin::GameFrame);

       hooks::CreateHook(globals::gameeventmanager, &IGameEventManager2::FireEvent,
                         &EventHandler, false, DATA_TYPE_BOOL, 2, DATA_TYPE_POINTER,
                         DATA_TYPE_BOOL);*/
    }

CON_COMMAND(udp_port, "Gets udp port") {
  VSPDN_CORE_INFO("The UDP Port is: {0}", globals::server->GetUDPPort());
}

CON_COMMAND(cmd_mapname, "Get Map Name") {
  VSPDN_CORE_INFO("The current mapname is: {0}", globals::server->GetMapName());
}

CON_COMMAND(cmd_tick, "Get Current Tick") {
  VSPDN_CORE_INFO("The current tick is: {0}", globals::server->GetTick());
  globals::vsp_plugin.AddTaskForNextFrame([]
  { VSPDN_CORE_INFO("This happened next tick... {0}", globals::server->GetTick());
  });
}

void MyServerPlugin::Unload() {
  // This will crash because CLR does not allow unloading and the srcds server unloads our dll, which in turns unloads CLR.
  globals::dotnet_manager.Shutdown();
  vspdotnet::Log::Close();
}

void MyServerPlugin::Pause() {}

void MyServerPlugin::UnPause() {}

const char *MyServerPlugin::GetPluginDescription() { return "Source.NET"; }

void MyServerPlugin::LevelInit(const char *pMapName) {
  VSPDN_CORE_TRACE("name={0},mapname={1}", "LevelInit", pMapName);
}

void MyServerPlugin::ServerActivate(edict_t *pEdictList, int edictCount,
                                    int clientMax) {
  VSPDN_CORE_TRACE("name={0},arg1={1},arg2={2}\n", "ServerActivate", edictCount,
                   clientMax);
}

void MyServerPlugin::GameFrame(bool simulating)
{ }

void MyServerPlugin::LevelShutdown() {
  VSPDN_CORE_TRACE("name={0}", "LevelShutdown");
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
    if (m_nextTasks.empty())
      return;

    VSPDN_CORE_INFO("Executing queued tasks of size: {0}", m_nextTasks.size());

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
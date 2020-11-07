#include "main.h"
#include "globals.h"
#include "interfaces.h"
#include "log.h"

#include "sig_scanner.h"
#include "entity_listener.h"

#include <chrono>
#include <thread>
#include <iostream>

#include <public/iserver.h>
#include <public/igameevents.h>

#include <hook.h>
#include <manager.h>
#include <conventions/x86GccThiscall.h>
#include <conventions/x86GccCdecl.h>

namespace vspdotnet {

  class CPluginConVarAccessor : public IConCommandBaseAccessor
  {
  public:
    virtual bool RegisterConCommandBase(ConCommandBase* pCommand)
      {
        globals::cvars->RegisterConCommand(pCommand);
        return true;
      }
  };

  CPluginConVarAccessor g_ConVarAccessor;

  void InitServerCommands()
  {
    ConVar_Register(0, &g_ConVarAccessor);
  }

  EXPOSE_SINGLE_INTERFACE_GLOBALVAR(MyServerPlugin, IServerPluginCallbacks,
                                    INTERFACEVERSION_ISERVERPLUGINCALLBACKS,
                                    globals::vsp_plugin);

  MyServerPlugin::MyServerPlugin() {}

  MyServerPlugin::~MyServerPlugin() {}

  bool MyServerPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) {
    vspdotnet::Log::Init();

    ConnectInterfaces(&interfaceFactory, 1);

    if (!vspdotnet::interfaces::InitializeEngineInterfaces(interfaceFactory))
      return false;

    VSPDN_CORE_INFO("Successfully loaded engine interfaces.");

    if (!vspdotnet::interfaces::InitializeGameInterfaces(gameServerFactory))
      return false;

    VSPDN_CORE_INFO("Successfully loaded game interfaces.");
    if (!vspdotnet::globals::InitializeGlobals())
      return false;

    VSPDN_CORE_INFO("Initializing MathLib...");
    MathLib_Init(2.2f, 2.2f, 0.f, 2.0f);

    VSPDN_CORE_INFO("Initializing commands...");
    InitServerCommands();

    CBinaryFile *engineBinary = vspdotnet::FindBinary("bin/engine", false);
    CBinaryFile *serverBinary = vspdotnet::FindBinary("server", false);

    VSPDN_CORE_INFO("Engine Binary: {0}", (void*) engineBinary);
    VSPDN_CORE_INFO("Server Binary: {0}", (void*) serverBinary);

    unsigned char iserversig[17] = {0x55, 0x89, 0xE5, 0x53, 0x83, 0xEC, 0x14, 0x8B, 0x45, 0x0C, 0xC7, 0x04, 0x24, 0x2A, 0x2A, 0x2A, 0x2A};
    unsigned long addr = engineBinary->FindSignature(iserversig, 17);

    unsigned char globalentitylistsig[] = {0xE8,0x2A,0x2A,0x2A,0x2A,0xE8,0x2A,0x2A,0x2A,0x2A,0xC7,0x2A,0x2A,0x2A,0x2A,0x2A,0x2A,0xE8,0x2A,0x2A,0x2A,0x2A,0xE8};
    unsigned long entlistaddr = serverBinary->FindSignature(globalentitylistsig, sizeof(globalentitylistsig)/sizeof(globalentitylistsig[0]));

    globals::server = *(IServer**)(addr + 13);
    globals::global_entity_list = *(CGlobalEntityList**)(entlistaddr + 13);

    globals::global_entity_list->AddListenerEntity(&vspdotnet::Listener);

    VSPDN_CORE_INFO("Server Address: {0}, Entity List Address: {1}", (void*)globals::server, (void*)globals::global_entity_list);



    VSPDN_CORE_INFO("Loaded successfully");
    return true;
  }

  bool PreMyFunc(HookType_t eHookType, CHook* pHook)
  {
    auto event = pHook->GetArgument<IGameEvent*>(1);
    if (event) {
      VSPDN_CORE_INFO("Prefunc This: {0}", event->GetName());

      if (strcmp(event->GetName(), "player_death") == 0) {
        VSPDN_CORE_INFO("Ignoring headshot");
        event->SetBool("headshot", false);
        event->SetBool("thrusmoke", true);
      }
    }

    return false;
  }

  bool PostMyFunc(HookType_t eHookType, CHook* pHook)
  {
    auto event = pHook->GetArgument<IGameEvent*>(1);
    if (event) {
      VSPDN_CORE_INFO("Postfunc This: {0}", event->GetName());
    }

    return false;
  }

  CON_COMMAND(testing, "testing") {
    CHookManager* pHookMngr = GetHookManager();

    IGameEventManager2* ptr = globals::gameevent_manager;

    auto test = &IGameEventManager2::FireEvent;
    void* vfuncaddr = (void*)(ptr->*test);

    auto intdifference = (unsigned long)ptr - (unsigned long)vfuncaddr;

    VSPDN_CORE_INFO("Virtual function pointer {0}, index maybe? {1}", vfuncaddr, intdifference);

    // Prepare calling convention
    std::vector<DataType_t> vecArgTypes;
    vecArgTypes.push_back(DATA_TYPE_POINTER); //this
    vecArgTypes.push_back(DATA_TYPE_POINTER); //igameevent
    vecArgTypes.push_back(DATA_TYPE_BOOL);

    // Hook the function
    CHook* pHook = pHookMngr->HookFunction(
      (void *&) vfuncaddr,
      new x86GccThiscall(vecArgTypes, DATA_TYPE_BOOL)
      );

    CHook* foundHook = pHookMngr->FindHook((void*&) vfuncaddr);

    if (foundHook) {
      VSPDN_CORE_INFO("Found existing hook {0}", (void*) foundHook);
    }

    pHook->AddCallback(HOOKTYPE_PRE, (HookHandlerFn *) (void *) &PreMyFunc);
    //pHook->AddCallback(HOOKTYPE_POST, (HookHandlerFn *) (void *) &PostMyFunc);

  }

  void MyServerPlugin::Unload() {}

  void MyServerPlugin::Pause() {}

  void MyServerPlugin::UnPause() {}

  const char *MyServerPlugin::GetPluginDescription() { return "Source.NET"; }

  void MyServerPlugin::LevelInit(const char *pMapName) {
    VSPDN_CORE_TRACE("name={0},mapname={1}","LevelInit", pMapName);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  void MyServerPlugin::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) {
    VSPDN_CORE_TRACE("name={0},arg1={1},arg2={2}\n", "ServerActivate", edictCount, clientMax);
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  void MyServerPlugin::GameFrame(bool simulating) {}

  void MyServerPlugin::LevelShutdown() {
    VSPDN_CORE_TRACE("name={0}", "LevelShutdown");
    std::this_thread::sleep_for(std::chrono::seconds(2));
  }

  void MyServerPlugin::ClientActive(edict_t *pEntity) {
    VSPDN_CORE_TRACE("name={0},edict={1}","ClientActive", (void*)pEntity);
  }

  void MyServerPlugin::ClientFullyConnect(edict_t *pEntity) {
    VSPDN_CORE_TRACE("name={0},edict={1}","ClientFullyConnect", (void*)pEntity);
  }

  void MyServerPlugin::ClientDisconnect(edict_t *pEntity) {
    VSPDN_CORE_TRACE("name={0},edict={1}","ClientDisconnect", (void*)pEntity);
  }

  void MyServerPlugin::ClientPutInServer(edict_t *pEntity, const char *playername) {
    VSPDN_CORE_TRACE("name={0},edict={1},playername={2}", "ClientPutInServer", (void*)pEntity, playername);
  }

  void MyServerPlugin::SetCommandClient(int index) {
    VSPDN_CORE_TRACE("name={0},index={1}", "SetCommandClient", index);
  }

  void MyServerPlugin::ClientSettingsChanged(edict_t *pEdict) {
    VSPDN_CORE_TRACE("name={0},edict={1}","ClientSettingsChanged", (void*)pEdict);
  }

  PLUGIN_RESULT MyServerPlugin::ClientConnect(bool *bAllowConnect,
                                              edict_t *pEntity,
                                              const char *pszName,
                                              const char *pszAddress,
                                              char *reject, int maxrejectlen) {
    VSPDN_CORE_TRACE("name={0},edict={1},name={2},address={3}","ClientConnect", (void*)pEntity, pszName, pszAddress);
    return PLUGIN_CONTINUE;
  }

  PLUGIN_RESULT MyServerPlugin::ClientCommand(edict_t *pEntity,
                                              const CCommand &args) {
    VSPDN_CORE_TRACE("name={0},edict={1},command{2}","ClientCommand",(void*)pEntity, args.Arg(0));

    if(strcmp(args.Arg(0), "hello_world") == 0) {
      return PLUGIN_STOP;
    }

    return PLUGIN_CONTINUE;
  }

  PLUGIN_RESULT MyServerPlugin::NetworkIDValidated(const char *pszUserName,
                                                   const char *pszNetworkID) {
    VSPDN_CORE_TRACE("name={0},name={1},networkid={2}","NetworkIDValidated", pszUserName, pszNetworkID);
    return PLUGIN_CONTINUE;
  }

  void MyServerPlugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie,
                                                edict_t *pPlayerEntity,
                                                EQueryCvarValueStatus eStatus,
                                                const char *pCvarName,
                                                const char *pCvarValue) {
    VSPDN_CORE_TRACE("name={0},edict={1},status={2},cvar_name={3},cvar_value={4}","OnQueryCvarValueFinished",(void*)pPlayerEntity, eStatus, pCvarName, pCvarValue);

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
}

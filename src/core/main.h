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

#pragma once

#include <functional>
#include <vector>

#include <public/engine/iserverplugin.h>

namespace vspdotnet {
  class MyServerPlugin : public IServerPluginCallbacks {
  public:
    MyServerPlugin() {}
    ~MyServerPlugin() {}

    // IServerPluginCallbacks interface
  public:
    bool Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) override;
    void Unload() override;
    void Pause() override;
    void UnPause() override;
    const char *GetPluginDescription() override;
    void LevelInit(const char *pMapName) override;
    void ServerActivate(edict_t *pEdictList, int edictCount, int clientMax) override;
    void GameFrame(bool simulating) override;
    void LevelShutdown() override;
    void ClientActive(edict_t *pEntity) override;
    void ClientFullyConnect(edict_t *pEntity) override;
    void ClientDisconnect(edict_t *pEntity) override;
    void ClientPutInServer(edict_t *pEntity, const char *playername) override;
    void SetCommandClient(int index) override;
    void ClientSettingsChanged(edict_t *pEdict) override;
    PLUGIN_RESULT ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen) override;
    PLUGIN_RESULT ClientCommand(edict_t *pEntity, const CCommand &args) override;
    PLUGIN_RESULT NetworkIDValidated(const char *pszUserName, const char *pszNetworkID) override;
    void OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue) override;
    void OnEdictAllocated(edict_t *edict) override;
    void OnEdictFreed(const edict_t *edict) override;
    bool BNetworkCryptKeyCheckRequired(uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient, bool bClientWantsToUseCryptKey) override;
    bool BNetworkCryptKeyValidate(uint32 unFromIP, uint16 usFromPort, uint32 unAccountIdProvidedByClient, int nEncryptionKeyIndexFromClient, int numEncryptedBytesFromClient, byte *pbEncryptedBufferFromClient, byte *pbPlainTextKeyForNetchan) override;

    void OnThink(bool last_tick);
    void AddTaskForNextFrame(std::function<void()>&& task);
  private:
      std::vector<std::function<void()>> m_nextTasks;
  };
}

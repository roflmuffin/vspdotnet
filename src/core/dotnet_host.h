#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <netcore/coreclr_delegates.h>
#include <netcore/hostfxr.h>

class PluginContext
{
    friend class CDotNetManager;
public:
    PluginContext(std::string dll_path)
            : m_dll_path_(dll_path)
    {
    }
public:
    //std::vector<std::tuple<CExecutableMarshalObject*, CallbackT>> listeners;
    //MonoDomain* GetDomain() const;
private:
    /*MonoDomain* m_domain_;
    MonoObject* m_mono_obj_;
    MonoMethod* m_load_method_;
    MonoMethod* m_unload_method_;*/
    std::string m_dll_path_;
};

class CDotNetManager {
    friend class PluginContext;
public:
    CDotNetManager();
    ~CDotNetManager();
public:
    bool Initialize();
    //void UnloadPlugin(MonoDomain* domain);
    void UnloadPlugin(PluginContext* context);
    void Shutdown();
    //void ReloadDomain(MonoDomain* mono_domain);
    //PluginContext* FindContext(MonoDomain* mono_domain);
    PluginContext* FindContext(std::string path);
private:
   // MonoDomain* CreateDomain(const char* path, bool hotReload);
private:
    std::vector<std::shared_ptr<PluginContext>> m_app_domains_;
    //std::unordered_map<MonoDomain*, PluginContext*> m_plugin_contexts_lookup_;
};
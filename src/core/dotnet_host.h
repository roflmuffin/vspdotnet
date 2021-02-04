#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class PluginContext
{
    friend class CDotNetManager;
public:
    PluginContext(std::string dll_path)
            : m_dll_path_(dll_path)
    {
    }
private:
    std::string m_dll_path_;
};

class CDotNetManager {
    friend class PluginContext;
public:
    CDotNetManager();
    ~CDotNetManager();
public:
    bool Initialize();
    void UnloadPlugin(PluginContext* context);
    void Shutdown();
    PluginContext* FindContext(std::string path);
private:
private:
    std::vector<std::shared_ptr<PluginContext>> m_app_domains_;
};
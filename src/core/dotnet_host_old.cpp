#include "dotnet_host.h"

#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#else
#include <dlfcn.h>
#include <unistd.h>
#include <locale>
#endif // WIN32
#include <chrono>
#include <string>
#include <codecvt>

#include <fstream>
#include <vector>
#include <functional>
#include <thread>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/mono-gc.h>
#include <mono/utils/mono-logger.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/mono-config.h>

#include <eiface.h>

#include "utils.h"
#include "globals.h"
#include "log.h"
#include "script_engine.h"

#ifdef __gnu_linux__
#include <signal.h>
#endif


MonoDomain* root_domain;

MonoDomain* PluginContext::GetDomain() const
{
  return m_domain_;
}

CDotNetManager::CDotNetManager() {
}

CDotNetManager::~CDotNetManager()
{
}

MonoAssembly* LoadAssemblyFromMemory(MonoDomain* domain, const char* path)
{
  std::ifstream infile(path, std::ios::binary);
  infile.seekg(0, std::ios::end);
  size_t file_size_in_byte = infile.tellg();
  std::vector<char> data; // used to store text data
  data.resize(file_size_in_byte);
  infile.seekg(0, std::ios::beg);
  infile.read(&data[0], file_size_in_byte);
  infile.close();

  MonoImageOpenStatus status;
  MonoImage* loaded_image = mono_image_open_from_data_with_name(data.data(), data.size(), true, &status, false, path);

  MonoAssembly* assembly = mono_image_get_assembly(loaded_image);
  if (assembly == NULL)
  {
    assembly = mono_assembly_load_from_full(loaded_image, path, &status, false);
  } else
  {
    assembly = mono_assembly_open(path, nullptr);
  }

  mono_image_close(loaded_image);

  const char* assemblyName = mono_stringify_assembly_name(mono_assembly_get_name(assembly));
  VSPDN_CORE_INFO("Loaded assembly [{0}] from memory at path {1} into app domain {2}", assemblyName, path, mono_domain_get_friendly_name(mono_domain_get()));

  return assembly;
}

MonoClass* helpers_class;

void AddFileWatcher(std::string path)
{
  auto oldDomain = mono_domain_get();
  mono_domain_set(mono_get_root_domain(), 0);
  /*
  MonoMethodDesc* description = mono_method_desc_new("void:AddFileWatcher(string)", false);
  //MonoMethod* method = mono_class_get_method_from_name(helpers_class, "AddFileWatcher", 1);
  MonoMethod* method = mono_method_desc_search_in_class(description, helpers_class);
  MonoObject* exception;*/

  MonoMethodDesc* description = mono_method_desc_new("void:AddFileWatcher(string)", false);
  MonoMethod* method = mono_method_desc_search_in_class(description, helpers_class);

  MonoObject* exception;
  MonoObject* plugin_obj = mono_runtime_invoke(method, NULL, NULL, &exception);

  auto str = mono_string_new(root_domain, path.c_str());

  void** params = new void* [1]{ str };

  mono_runtime_invoke(method, NULL, params, &exception);

  if (exception)
  {
    VSPDN_CORE_INFO("Found exception: {0}", (void*) exception);

    MonoString* pMsg = mono_object_to_string(exception, nullptr);

    auto result = mono_string_to_utf8(pMsg);
    VSPDN_CORE_WARN("Exception occurred in .NET from domain [{0}]: {1}", mono_domain_get_friendly_name(mono_domain_get()), result);
  }

  mono_domain_set(oldDomain, 0);
}


MonoDomain* CDotNetManager::CreateDomain(const char* path, bool hotReload)
{
  char* gamePath = new char[255];
  vspdotnet::globals::engine->GetGameDir(gamePath, 255);

  std::string baseDir = vspdotnet::utils::CSGODotNetDirectory();
  auto executablePath = std::string(baseDir + "/modules/" + path + "/" + path + ".dll");

  if (!hotReload)
  {
    AddFileWatcher(executablePath);
  }

  MonoDomain* domain = mono_domain_create_appdomain((char*)path, NULL);
  if (!mono_domain_set(domain, false))
  {
    return NULL;
  }

  // Load the shared library into loaded assembly list
  MonoAssembly* api_assembly = mono_assembly_open(std::string(baseDir + "/api/CSGONET.API.dll").c_str(), nullptr);

  // TODO: Try and load this from memory?
  //MonoAssembly* api_assembly = LoadAssemblyFromMemory(domain, std::string(baseDir + "/api/CSGONET.API.dll").c_str());

  MonoImage* api_image = mono_assembly_get_image(api_assembly);
  MonoClass* plugin_class = mono_class_from_name(api_image, "CSGONET.API.Core", "BasePlugin");

  // Load provided plugin/dll.
  auto* local_assembly = LoadAssemblyFromMemory(domain, executablePath.c_str());

  VSPDN_CORE_INFO("Attempting to Instantiate plugin {0}", path);

  // Use managed helper to instantiate instance of plugin
  MonoClass* local_helpers_class = mono_class_from_name(api_image, "CSGONET.API.Core", "Helpers");
  MonoMethodDesc * description = mono_method_desc_new("void:InstantitatePlugin()", false);
  MonoMethod* method = mono_method_desc_search_in_class(description, local_helpers_class);

  MonoObject* exception;
  MonoObject* plugin_obj = mono_runtime_invoke(method, NULL, NULL, &exception);

  if (exception)
  {
    VSPDN_CORE_INFO("Found exception: {0}", (void*) exception);

    MonoString* pMsg = mono_object_to_string(exception, nullptr);

    auto result = mono_string_to_utf8(pMsg);
    VSPDN_CORE_WARN("Exception occurred in .NET from domain [{0}]: {1}", mono_domain_get_friendly_name(mono_domain_get()), result);
    return nullptr;
  }

  if (!plugin_obj) {
    VSPDN_CORE_ERROR("Plugin could not be instantiated {0}", path);
    return nullptr;
  }

  // Get & call our load function
  MonoMethod* load_method = mono_class_get_method_from_name(plugin_class, "Load", 0);
  MonoMethod* load_method_impl = mono_object_get_virtual_method(plugin_obj, load_method);

  // Get our unload function
  MonoMethod* unload_method = mono_class_get_method_from_name(plugin_class, "Unload", 0);
  MonoMethod* unload_method_impl = mono_object_get_virtual_method(plugin_obj, unload_method);

  // Save all details of the plugin to a reference list
  auto plugin_context = new PluginContext(domain, plugin_obj, load_method_impl, unload_method_impl, executablePath);

  if (exception)
  {
    MonoString* pMsg = mono_object_to_string(exception, NULL);
    auto result = mono_string_to_utf8(pMsg);
    VSPDN_CORE_WARN("Exception occurred in .NET from domain [{0}]: {1}", mono_domain_get_friendly_name(mono_domain_get()), result);
    return nullptr;
  }

  m_app_domains_.push_back(std::shared_ptr<PluginContext>(plugin_context));
  m_plugin_contexts_lookup_[domain] = plugin_context;

  // Add our attribute hooks
  // Not working atm
/*  description = mono_method_desc_new("void:AddListeners(BasePlugin)", false);
  method = mono_method_desc_search_in_class(description, local_helpers_class);
  void* params[] = {plugin_obj};
  mono_runtime_invoke(method, NULL, params, &exception);

  if (exception)
  {
    MonoString* pMsg = mono_object_to_string(exception, NULL);
    auto result = mono_string_to_utf8(pMsg);
    VSPDN_CORE_WARN("Exception occurred in .NET from domain [{0}]: {1}", mono_domain_get_friendly_name(mono_domain_get()), result);
    return nullptr;
  }*/

  // Execute load after adding to lookup manager so callbacks & events know of our existence.
  mono_runtime_invoke(load_method_impl, plugin_obj, nullptr, &exception);

  return domain;
}

void test() {
  // Load Assembly...
  MonoAssembly* api_assembly = LoadAssemblyFromMemory(mono_get_root_domain(), "/home/michael/projects/CSGODotNet/csgo_dotnet_managed/ClassLibrary4/bin/Debug/ClassLibrary4.dll");
  MonoImage* api_image = mono_assembly_get_image(api_assembly);

  // Use managed helper to instantiate instance of plugin
  MonoImage* local_image = mono_assembly_get_image(api_assembly);
  MonoClass* local_helpers_class = mono_class_from_name(local_image, "ClassLibrary4", "Class1");
  MonoMethodDesc * description = mono_method_desc_new("void:Test()", false);
  MonoMethod* method = mono_method_desc_search_in_class(description, local_helpers_class);

  MonoObject* exception;
  MonoObject* plugin_obj = mono_runtime_invoke(method, NULL, NULL, &exception);

  if (exception)
  {
    printf("Found exception: {%p}", (void*) exception);

    MonoString* pMsg = mono_object_to_string(exception, nullptr);

    auto result = mono_string_to_utf8(pMsg);
    printf("Exception occurred in .NET from domain [%s]: {%s}\n", mono_domain_get_friendly_name(mono_domain_get()), result);
  }
}

void UnloadDomain(MonoDomain* domain)
{
  mono_domain_set(mono_get_root_domain(), 0);

  /*mono_thread_attach(mono_get_root_domain());
  mono_jit_thread_attach(mono_get_root_domain());

  mono_thread_attach(domain);
  mono_jit_thread_attach(domain);*/

  const char* name = mono_domain_get_friendly_name(domain);
  VSPDN_CORE_INFO("Attempting to unload domain {0}, current domain {1} thread {2}", name, mono_domain_get_friendly_name(mono_domain_get()), (void*)mono_thread_current());
  mono_domain_unload(domain);
  mono_gc_collect(mono_gc_max_generation());
}

static void OnLogCallback(const char* domain, const char* level, const char* message, mono_bool, void*)
{
  //printf("{0} | {1} | {2}\n", domain, level, message);
  printf("{%s} | {%s} | {%s}\n", domain, level, message);
}

static void OnErrCallback(const char* string, mono_bool is_stdout)
{
  printf("{%s}\n", string);
}

static void ReloadDomainStr(std::string path)
{
  VSPDN_CORE_INFO("RELOADING");
  PluginContext* context = vspdotnet::globals::dotnet_manager.FindContext(path);
  if (context && context->GetDomain() != mono_get_root_domain())
  {
    vspdotnet::globals::dotnet_manager.ReloadDomain(context->GetDomain());
  }
}

bool CDotNetManager::Initialize() {
  //dlopen("libmono-2.0.so", RTLD_LAZY | RTLD_GLOBAL);
  //dlopen("libstdc++.so", RTLD_GLOBAL);
  mono_set_dirs("/home/michael/projects/cmake_test/libraries/mono/lib","/home/michael/projects/cmake_test/libraries/mono/etc");

  mono_config_parse(NULL);

  // Setup Mono

  mono_debug_init(MONO_DEBUG_FORMAT_MONO);
  //mono_trace_set_level_string("debug");
  //mono_trace_set_log_handler(OnLogCallback, nullptr);
  //mono_trace_set_printerr_handler(OnErrCallback);
  //mono_trace_set_print_handler(OnErrCallback);

  root_domain = mono_jit_init("MainDomain");
  mono_add_internal_call("ClassLibrary4.Helpers::InvokeNative", reinterpret_cast<void*>(vspdotnet::ScriptEngine::InvokeNative));

  MonoDomain* domain = mono_domain_create_appdomain((char*)"ClassLibrary4", NULL);
  if (!mono_domain_set(domain, false))
  {
    return NULL;
  }


  // Load Assembly...
  MonoAssembly* api_assembly = LoadAssemblyFromMemory(mono_get_root_domain(), "/home/michael/projects/CSGODotNet/csgo_dotnet_managed/ClassLibrary4/bin/Debug/ClassLibrary4.dll");
  MonoImage* api_image = mono_assembly_get_image(api_assembly);

  // Use managed helper to instantiate instance of plugin
  MonoImage* local_image = mono_assembly_get_image(api_assembly);
  MonoClass* local_helpers_class = mono_class_from_name(local_image, "ClassLibrary4", "Class1");
  MonoMethodDesc * description = mono_method_desc_new("void:Test()", false);
  MonoMethod* method = mono_method_desc_search_in_class(description, local_helpers_class);

  // Helpers
  MonoAssembly* helpers_api_assembly = LoadAssemblyFromMemory(mono_get_root_domain(), "/home/michael/projects/CSGODotNet/csgo_dotnet_managed/CSGONET.API/bin/Debug/CSGONET.API.dll");
  MonoImage* helpers_api_image = mono_assembly_get_image(helpers_api_assembly);

  MonoClass* helpers_class = mono_class_from_name(helpers_api_image, "CSGONET.API.Core", "Helpers");
  MonoMethodDesc* helper_description = mono_method_desc_new("void:AddFileWatcher(string)", false);
  MonoMethod* helper_method = mono_method_desc_search_in_class(helper_description, helpers_class);

  const char* path = "/home/michael/projects/CSGODotNet/csgo_dotnet_managed/ClassLibrary4/bin/Debug/ClassLibrary4.dll";

  MonoObject* exception;
/*  MonoObject* plugin_obj = mono_runtime_invoke(helper_method, NULL, NULL, &exception);

  if (exception)
  {
    printf("Found exception: {%p}", (void*) exception);

    //mono_unhandled_exception(exception);

    MonoString* pMsg = mono_object_to_string(exception, nullptr);

    auto result = mono_string_to_utf8(pMsg);
    printf("Exception occurred in .NET from domain [%s]: {%s}\n", mono_domain_get_friendly_name(mono_domain_get()), result);
  }*/

  auto str = mono_string_new(root_domain, path);

  void** params = new void* [1]{ str };

  mono_runtime_invoke(helper_method, NULL, params, &exception);

  if (exception)
  {
    printf("Found exception: {%p}", (void*) exception);

    //mono_unhandled_exception(exception);

    MonoString* pMsg = mono_object_to_string(exception, nullptr);

    auto result = mono_string_to_utf8(pMsg);
    printf("Exception occurred in .NET from domain [%s]: {%s}\n", mono_domain_get_friendly_name(mono_domain_get()), result);
  }

  return true;
}

/*bool CDotNetManager::Initialize2() {
#ifdef WIN32
  mono_set_dirs("C:\\Program Files (x86)\\Mono\\lib", "C:\Program Files (x86)\Mono\etc");
#else
  mono_set_dirs("/home/michael/projects/cmake_test/libraries/mono/lib","/home/michael/projects/cmake_test/libraries/mono/etc");
  //mono_set_dirs((vspdotnet::utils::CSGODotNetDirectory() + "/mono/lib").c_str(), (vspdotnet::utils::CSGODotNetDirectory() + "/mono/etc").c_str());
#endif


  if (!root_domain)
  {
    dlopen("/home/michael/projects/cmake_test/libraries/mono/lib/libmono-2.0.so", RTLD_GLOBAL);

    mono_config_parse(NULL);

    mono_debug_init(MONO_DEBUG_FORMAT_MONO);
    //mono_trace_set_level_string("debug");
    mono_trace_set_log_handler(OnLogCallback, nullptr);
    mono_trace_set_printerr_handler(OnErrCallback);
    mono_trace_set_print_handler(OnErrCallback);
    root_domain = mono_jit_init("MainDomain");

    mono_add_internal_call("CSGONET.API.Core.Helpers::ReloadDomain", reinterpret_cast<void*>(ReloadDomainStr));
    mono_add_internal_call("ClassLibrary4.Helpers::InvokeNative", reinterpret_cast<void*>(vspdotnet::ScriptEngine::InvokeNative));
    //mono_thread_set_main(mono_thread_current());
  }

  std::string baseDir = vspdotnet::utils::CSGODotNetDirectory();

  //MonoAssembly* api_assembly = mono_assembly_open(std::string(baseDir + "/api/CSGONET.API.dll").c_str(), nullptr);
  //MonoAssembly* api_assembly = LoadAssemblyFromMemory(mono_get_root_domain(), std::string(baseDir + "/api/CSGONET.API.dll").c_str());
  MonoAssembly* api_assembly = LoadAssemblyFromMemory(mono_get_root_domain(), "/home/michael/projects/CSGODotNet/csgo_dotnet_managed/CSGONET.API/bin/Debug/CSGONET.API.dll");
  MonoImage* api_image = mono_assembly_get_image(api_assembly);

  // Add our file watcher in the root domain, so when we cause a reload no errors occur.
  helpers_class = mono_class_from_name(api_image, "CSGONET.API.Core", "Helpers");

  //MonoDomain* firstDomain = CreateDomain("ClassLibrary4", false);
  //test();
  MonoDomain* firstDomain = CreateDomain("ClassLibrary4", false);

  //MonoDomain* second_domain = CreateDomain("TestModule", false);

  return true;
}*/
void CDotNetManager::UnloadPlugin(MonoDomain* domain)
{
  PluginContext* context = FindContext(domain);
  UnloadPlugin(context);
}

void CDotNetManager::UnloadPlugin(PluginContext* context)
{
/*  for (std::tuple<CExecutableMarshalObject*, CallbackT> listener : context->listeners)
  {
    auto handle = std::get<0>(listener);
    auto callback = std::get<1>(listener);
    Msg("Deregistering listener %s %s\n", handle->GetName().c_str(), mono_domain_get_friendly_name(context->GetDomain()));
    handle->RemoveListener(callback);
  }*/

  mono_runtime_invoke(context->m_unload_method_, context->m_mono_obj_, nullptr, nullptr);
  UnloadDomain(context->m_domain_);
  m_plugin_contexts_lookup_[context->m_domain_] = nullptr;

  int foundIndex = -1;
  for(int i = 0; i < m_app_domains_.size(); i++)
  {
    if (m_app_domains_[i].get() == context) foundIndex = i;
  }

  m_app_domains_.erase(m_app_domains_.begin() + foundIndex);
}

void CDotNetManager::Shutdown()
{
  for(int i = m_app_domains_.size() - 1; i >= 0; i--)
  {
    UnloadPlugin(m_app_domains_[i].get());
  }
}

void CDotNetManager::ReloadDomain(MonoDomain* mono_domain)
{
  PluginContext* context = FindContext(mono_domain);
  if (context == nullptr) return;

  // We should only change mono domains from the main thread
  vspdotnet::globals::vsp_plugin.AddTaskForNextFrame([this, context]() -> void
                                  {
                                      std::string name = mono_domain_get_friendly_name(context->m_domain_);
                                      UnloadPlugin(context);
                                      CreateDomain(name.c_str(), true);
                                  });
}

PluginContext* CDotNetManager::FindContext(MonoDomain* mono_domain)
{
  PluginContext* context = m_plugin_contexts_lookup_[mono_domain];
  return context;
}

PluginContext* CDotNetManager::FindContext(std::string path)
{
  for (auto app_domain : m_app_domains_)
  {
    if (app_domain->m_dll_path_ == path)
    {
      return app_domain.get();
    }
  }

  return nullptr;
}

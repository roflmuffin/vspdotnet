using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Threading.Tasks;
using CSGONET.API.Core;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Listeners;
using McMaster.NETCore.Plugins;

namespace CSGONET.API.Core
{
    public sealed class GlobalContext
    {

        private static GlobalContext _instance = null;
        public static GlobalContext Instance => _instance;

        bool AreModulesWereLoaded;
        private DirectoryInfo rootDir;

        private List<PluginContext> _loadedPlugins = new List<PluginContext>();

        public GlobalContext()
        {
            AreModulesWereLoaded = false;
            rootDir = new FileInfo(Assembly.GetExecutingAssembly().Location).Directory.Parent;
            _instance = this;
        }

        ~GlobalContext()
        {
            foreach (var plugin in _loadedPlugins)
            {
                plugin.Unload();
            }
        }

        public void OnNativeUnload()
        {
            foreach (var plugin in _loadedPlugins)
            {
                plugin.Unload();
            }
        }

        public void LoadAll()
        {
            Console.WriteLine("Loading .NET modules...");

            DirectoryInfo modules_directory_info;
            try
            {
                modules_directory_info = new DirectoryInfo(Path.Combine(rootDir.FullName, "modules"));
            }
            catch (Exception e)
            {
                Console.WriteLine("Unable to access .NET modules directory: " + e.GetType().ToString() + " " + e.Message);
                return;
            }

            DirectoryInfo[] proper_modules_directories;
            try
            {
                proper_modules_directories = modules_directory_info.GetDirectories();
            }
            catch
            {
                proper_modules_directories = new DirectoryInfo[0];
            }

            var filePaths = proper_modules_directories
                .Where(d => d.GetFiles().Any((f) => f.Name == d.Name + ".dll"))
                .Select(d => d.GetFiles().First((f) => f.Name == d.Name + ".dll").FullName)
                .ToArray();

            foreach (var path in filePaths)
            {
                var plugin = new PluginContext(path);

                try
                {
                    _loadedPlugins.Add(plugin);
                    plugin.Load();
                }
                catch (Exception e)
                {
                    _loadedPlugins.Remove(plugin);
                    Console.WriteLine($"Failed to load plugin {path} with error {e}");
                }
            }

            Console.WriteLine("All managed modules were loaded.");
        }

        public PluginContext FindPluginByType(Type moduleClass)
        {
            var plugin = _loadedPlugins.FirstOrDefault(x => x.PluginType == moduleClass);
            return plugin;
        }
    }
}
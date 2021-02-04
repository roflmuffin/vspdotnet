using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.Loader;
using System.Threading.Tasks;
using CSGONET.API.Modules.Commands;
using CSGONET.API.Modules.Cvars;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Listeners;
using McMaster.NETCore.Plugins;

namespace CSGONET.API.Core
{
    public class PluginContext
    {
        private BasePlugin _plugin;
        private PluginLoader _assemblyLoader;

        public string Name => _plugin?.ModuleName;
        public string Version => _plugin?.ModuleVersion;
        public Type PluginType => _plugin?.GetType();

        private readonly string _path;

        /*
        private List<CallbackSubscription> _gameEventSubscribers = new List<CallbackSubscription>();
        private List<CallbackSubscription> _listenerSubscriptions = new List<CallbackSubscription>();
        private List<CallbackSubscription> _conCommandSubscriptions = new List<CallbackSubscription>();
        private List<CallbackSubscription> _convars = new List<CallbackSubscription>();*/

        public PluginContext(string path)
        {
            _path = path;
            Console.WriteLine($"Initializing plugin context for path: {_path}");

            _assemblyLoader = PluginLoader.CreateFromAssemblyFile(path, new[] {typeof(IPlugin) }, config =>
            {
                config.EnableHotReload = true;
                config.IsUnloadable = true;
            });

            _assemblyLoader.Reloaded += async (s, e) => await OnReloadedAsync(s, e);
        }

        private async Task OnReloadedAsync(object sender, PluginReloadedEventArgs eventargs)
        {
            Console.WriteLine($"Reloading plugin {Name}");
            _assemblyLoader = eventargs.Loader;
            Unload(hotReload: true);
            Load(hotReload: true);
        }

        public void Load(bool hotReload = false)
        {
            using (_assemblyLoader.EnterContextualReflection())
            {
                Type pluginType = _assemblyLoader.LoadDefaultAssembly().GetTypes()
                    .FirstOrDefault(t => typeof(IPlugin).IsAssignableFrom(t));

                if (pluginType == null) throw new Exception("Unable to find plugin in DLL");

                Console.WriteLine($"Loading plugin: {pluginType.Name}");
                _plugin = (BasePlugin) Activator.CreateInstance(pluginType);
                _plugin.Load(hotReload);

                Console.WriteLine($"Finished loading plugin: {Name}");
            }
        }

        public void Unload(bool hotReload = false)
        {
            var cachedName = Name;

            Console.WriteLine($"Unloading plugin {Name}");

            _plugin.Unload(hotReload);

            foreach (var kv in _plugin.handlers)
            {
                var data = kv.Value.GetValue() as object[];
                _plugin.DeregisterEventHandler(data[0].ToString(), (Action<GameEvent>)kv.Key, Convert.ToBoolean(data[1]));
            }

            foreach (var kv in _plugin.commandHandlers)
            {
                _plugin.RemoveCommand((string)kv.Value.GetValue(), (CommandInfo.CommandCallback) kv.Key);
            }

            foreach (var kv in _plugin.convarChangeHandlers)
            {
                var convar = (ConVar) kv.Value.GetValue();
                _plugin.UnhookConVarChange((ConVar)kv.Value.GetValue(), (ConVar.ConVarChangedCallback)kv.Key);
                convar.Unregister();
            }

            foreach (var kv in _plugin.listeners)
            {
                _plugin.RemoveListener((string) kv.Value.GetValue(), kv.Key);
            }

            foreach (var timer in _plugin.timers)
            {
                timer.Kill();
            }

            /*
            _gameEventSubscribers.ForEach(x => GameEvent.Unsubscribe(x.Name, x.Handler));
            _gameEventSubscribers.Clear();

            _listenerSubscriptions.ForEach(x => Listeners.RemoveListener(x.Name, x.Handler));
            _listenerSubscriptions.Clear();

            _conCommandSubscriptions.ForEach(x => ConsoleCommand.RemoveCommand(x.Name, x.Handler));
            _conCommandSubscriptions.Clear();
            */

            /*
            _convars.ForEach(x =>
            {
                var cvar = ConVar.Find(x.Name);
                cvar.RemoveListener(x.Handler);
                cvar.Unregister();
            });
            _convars.Clear();*/

            if (!hotReload) _assemblyLoader.Dispose();

            Console.WriteLine($"Finished unloading plugin {cachedName}");
        }

        public void AddGameEventSubscriber(string name, Delegate handler)
        {
            //_gameEventSubscribers.Add(new CallbackSubscription(name, handler));
        }

        public void AddListenerSubscriber(string name, Delegate handler)
        {
           // _listenerSubscriptions.Add(new CallbackSubscription(name, handler));
        }

        public void AddCommandSubscriber(string name, Delegate handler)
        {
            //_conCommandSubscriptions.Add(new CallbackSubscription(name, handler));
        }

        public void AddConVarSubscriber(string name, Delegate handler)
        {
            //_convars.Add(new CallbackSubscription(name, handler));
        }
    }
}
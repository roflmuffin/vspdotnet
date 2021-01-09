using System;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Security;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Listeners;

namespace CSGONET.API.Core
{
    public class MethodAttribute<T> where T : Attribute
    {
        public MethodAttribute(T attribute, MethodInfo method)
        {
            Attribute = attribute;
            Method = method;
        }

        public T Attribute;
        public MethodInfo Method;
    }

    public static class Helpers
    {
        private static MethodAttribute<T>[] FindMethodAttributes<T>(BasePlugin plugin) where T: Attribute
        {
            return plugin
                .GetType()
                .GetMethods()
                .Where(m => m.GetCustomAttributes(typeof(T), false).Length > 0)
                .Select(x => new MethodAttribute<T>(x.GetCustomAttribute<T>(), x))
                .ToArray();   
        }

        public static void PrintLoadedAssemblies()
        {
            Console.WriteLine("Assemblies loaded into app context " + AppDomain.CurrentDomain.FriendlyName);
            foreach (var assembly in AppDomain.CurrentDomain.GetAssemblies())
            {
                Console.WriteLine(assembly.FullName);
            }
        }

        private const string dllPath = "Testing";

        [SecurityCritical]
        [DllImport(dllPath, EntryPoint = "InvokeNative")]
        public static extern void InvokeNative(IntPtr ptr);



        [UnmanagedCallersOnly]
        public static int LoadAllPlugins()
        {
            try
            {
                var globalContext = new GlobalContext();
                globalContext.LoadAll();
                return 1;
            }
            catch (Exception e)
            {
                Console.WriteLine(e.StackTrace);
                return 0;
            }
        }

        public static object InstantitatePlugin()
        {
            var pluginType = AppDomain.CurrentDomain
                .GetAssemblies()
                .SelectMany(x => x.GetTypes())
                .FirstOrDefault(x => typeof(BasePlugin).IsAssignableFrom(x) && x != typeof(BasePlugin));

            if (pluginType == null) throw new Exception("Unable to find plugin in DLL");

            string codeBase = pluginType.Assembly.CodeBase;
            var assemblyPath = new Uri(codeBase).LocalPath;

            Server.PrintToConsole("Loading plugin at path: " + assemblyPath);

            var plugin = (BasePlugin)Activator.CreateInstance(pluginType);

            return plugin;
        }

        /*public static void AddListeners(BasePlugin plugin)
        {
            Server.PrintToConsole("Loading listeners from attributes");

            var eventRegistrations = FindMethodAttributes<EventAttribute>(plugin);

            Server.PrintToConsole($"Found {eventRegistrations.Length} events to listen for.");

            foreach (var eventRegistration in eventRegistrations)
            {
                Server.PrintToConsole($"Method: {eventRegistration.Method.Name}, Event: {eventRegistration.Attribute.Name}");
                GameEvent.Subscribe(eventRegistration.Attribute.Name, @event =>
                {
                    eventRegistration.Method.Invoke(plugin, new[] {@event});
                });
            }
        }*/

        public delegate void Callback();

        public static void AddFileWatcher(string path)
        {
            var watcher = new FileSystemWatcher(Path.GetDirectoryName(path));
            watcher.Filter = Path.GetFileName(path);
            watcher.EnableRaisingEvents = true;
            watcher.Changed += (sender, args) =>
            {
                NativePINVOKE.PrintToConsole($"File {path} has been changed. {args.ChangeType.ToString()}");
                NativePINVOKE.ReloadDomain(path);
                /*var ptr = Marshal.GetDelegateForFunctionPointer<Callback>(new IntPtr(functionPtr));
                ptr?.Invoke();*/
            };
        }
    }
}
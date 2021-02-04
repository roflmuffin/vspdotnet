using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using CSGONET.API.Modules.Commands;
using CSGONET.API.Modules.Players;

namespace CSGONET.API.Modules.Listeners
{
    public static class Extensions
    {
        public static HandleRef ToHandle(this Listeners.MarshalCallback callback)
        {
            return new HandleRef(callback, Marshal.GetFunctionPointerForDelegate(callback));
        }
    }


    public partial class Listeners
    {
        public delegate void SourceEventHandler(object e);
        public delegate void SourceEventHandler<T>(T e);

        public class MapStartArgs : EventArgs
        {
            public string MapName { get; set; }
        }

        public class PlayerArgs : EventArgs
        {
            public Player Player { get; internal set; }
            public bool Cancel { get; set; }
            public string CancelReason { get; set; }
        }

        public class EntityArgs : EventArgs
        {
            public int EntityIndex { get; internal set; }
            public string Classname { get; set; }
        }

        public class PlayerConnectArgs : EventArgs
        {
            public int PlayerIndex { get; internal set; }
            public string Name { get; internal set; }
            public string Address { get; internal set; }
            public bool Cancel { get; set; }
            public string CancelReason { get; set; }
        }

        /*public static event SourceEventHandler<PlayerArgs> OnClientConnect
        {
            add => RegisterListener("OnClientConnect", value,
                (args, marshal) =>
                {
                    args.Player = marshal.GetValue<Player>();
                },
                (args, marshal) =>
                {
                    marshal.PushInt(args.Cancel ? 1 : 0);
                    marshal.PushString(args.CancelReason);
                });
            remove => RemoveListener("OnClientConnect", value);
        }*/

        /*public static event SourceEventHandler<PlayerArgs> OnClientConnected
        {
            add => RegisterListener("OnClientConnected", value,
                (args, marshal) => args.Player = marshal.GetValue<Player>());
            remove => RemoveListener("OnClientConnected", value);
        }
        
        public static event SourceEventHandler<PlayerArgs> OnClientDisconnect
        {
            add => RegisterListener("OnClientDisconnect", value,
                (args, marshal) => args.Player = marshal.GetValue<Player>());
            remove => RemoveListener("OnClientDisconnect", value);
        }

        public static event SourceEventHandler<PlayerArgs> OnClientDisconnectPost
        {
            add => RegisterListener("OnClientDisconnectPost", value,
                (args, Marshal) => args.Player = Marshal.GetValue<Player>());
            remove => RemoveListener("OnClientDisconnectPost", value);
        }

        public static event SourceEventHandler<PlayerArgs> OnClientPutInServer
        {
            add => RegisterListener("OnClientPutInServer", value,
                (args, Marshal) => args.Player = Marshal.GetValue<Player>());
            remove => RemoveListener("OnClientPutInServer", value);
        }*/


        private static readonly HandleRef _listenerManager = new HandleRef(null, NativePINVOKE.g_callback_manager_get());

        private static Dictionary<Delegate, MarshalCallback> subscribers = new Dictionary<Delegate, MarshalCallback>();

        public static void RegisterListener<T>(string name, SourceEventHandler<T> handler, Action<T, CMarshalObject> input = null, Action<T, CMarshalObject> output = null) where T : EventArgs, new()
        {
            var callback = Utilities.SafeExecute((ptr) =>
            {
                var marshal = new CMarshalObject(ptr, true);

                var eventArgs = new T();
                input?.Invoke(eventArgs, marshal);

                handler?.Invoke(eventArgs);

                if (output != null)
                {
                    var responseMarshal = new CMarshalObject();
                    output.Invoke(eventArgs, responseMarshal);
                    return responseMarshal.GetPointer();
                }

                return IntPtr.Zero;
            });

            subscribers.Add(handler, callback);

            var handleRef = new HandleRef(null, Marshal.GetFunctionPointerForDelegate(callback));

            /*var callingAssembly = handler.Method.DeclaringType.Assembly;
            var moduleClass = callingAssembly.GetTypes().FirstOrDefault(x => typeof(BasePlugin).IsAssignableFrom(x));
            var plugin = GlobalContext.Instance.FindPluginByType(moduleClass);

            plugin.AddListenerSubscriber(name, handler);*/

            NativePINVOKE.CCallbackManager_TryAddFunction(_listenerManager, name, handleRef);
        }

        public static void RemoveListener<T>(string name, SourceEventHandler<T> handler)
            where T : EventArgs, new()
        {
            var foundHandler = subscribers.ContainsKey(handler) ? subscribers[handler] : null;

            if (foundHandler != null)
            {
                var handleRef = new HandleRef(null, Marshal.GetFunctionPointerForDelegate(foundHandler));
                NativePINVOKE.CCallbackManager_TryRemoveFunction(_listenerManager, name, handleRef);
            }
        }

        public static void RemoveListener(string name, Delegate handler)
        {
            var foundHandler = subscribers.ContainsKey(handler) ? subscribers[handler] : null;

            if (subscribers.ContainsKey(handler))
            {
                var handleRef = new HandleRef(null, Marshal.GetFunctionPointerForDelegate(foundHandler));
                NativePINVOKE.CCallbackManager_TryRemoveFunction(_listenerManager, name, handleRef);
                subscribers.Remove(handler);
            }

            Server.PrintToConsole($"Deregistering listener {name} for plugin");

        }
    }
}

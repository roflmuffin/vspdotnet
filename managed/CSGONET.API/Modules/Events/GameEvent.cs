using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using CSGONET.API.Core;
using CSGONET.API.Modules.Listeners;
using CSGONET.API.Modules.Players;

namespace CSGONET.API.Modules.Events
{
    public class EventAttribute : Attribute
    {
        public EventAttribute(string name)
        {
            Name = name;
        }

        public string Name { get; set; }
    }

    public class GameEvent
    {
        public IntPtr Handle { get; private set; }

        protected GameEvent()
        {
        }

        public GameEvent(string name, bool force) : this(NativeAPI.CreateEvent(name, force))
        {

        }

        internal GameEvent(IntPtr pointer)
        {
            Handle = pointer;
        }

        public string Name => NativeAPI.GetEventName(Handle);

        public bool GetBool(string name) => NativeAPI.GetEventBool(Handle, name);
        public float GetFloat(string name) => NativeAPI.GetEventFloat(Handle, name);
        public string GetString(string name) => NativeAPI.GetEventString(Handle, name);
        public int GetInt(string name) => NativeAPI.GetEventInt(Handle, name);

        public Player GetPlayer(string name) => Player.FromUserId(GetInt(name));

        public void SetBool(string name, bool value) => NativeAPI.SetEventBool(Handle, name, value);
        public void SetFloat(string name, float value) => NativeAPI.SetEventFloat(Handle, name, value);
        public void SetString(string name, string value) => NativeAPI.SetEventString(Handle, name, value);
        public void SetInt(string name, int value) => NativeAPI.SetEventInt(Handle, name, value);

        public void FireEvent(bool dontBroadcast) => NativeAPI.FireEvent(Handle, dontBroadcast);
        public void FireEventToClient(int clientId, bool dontBroadcast) => NativeAPI.FireEventToClient(Handle, clientId);
    }

    /*public class GameEvent : BaseNative
    {
        public GameEvent(IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn)
        {
        }

        protected override void OnDispose()
        {
            
        }

        public string GetValue() => NativePINVOKE.IGameEvent_GetName(ptr);
        public string GetString(string name) => NativePINVOKE.IGameEvent_GetString__SWIG_1(ptr, name);
        public int GetInt(string name) => NativePINVOKE.IGameEvent_GetInt__SWIG_1(ptr, name);

        public Player GetPlayer() => Player.FromUserId(GetInt("userid"));

        private static HandleRef _eventManager = new HandleRef(null, IntPtr.Zero);
        //internal static List<Listeners.Listeners.MarshalCallback> list = new List<Listeners.Listeners.MarshalCallback>();

        internal static Dictionary<Delegate, Listeners.Listeners.MarshalCallback> callbacks = new Dictionary<Delegate, Listeners.Listeners.MarshalCallback>();

        public static void Subscribe(string name, Action<GameEvent> handler)
        {
            if (_eventManager.Handle == IntPtr.Zero)
            {
                _eventManager = new HandleRef(null, NativePINVOKE.g_event_manager_get());
            }

            //var callingAssembly = handler.Method.DeclaringType.Assembly;
            //var moduleClass = callingAssembly.GetTypes().FirstOrDefault(x => typeof(BasePlugin).IsAssignableFrom(x));
            /*var plugin = GlobalContext.Instance.FindPluginByType(moduleClass);
            plugin.AddGameEventSubscriber(name, handler);#1#
            //Server.PrintToConsole($"Registering event {name} for plugin {plugin.ModuleName} {plugin.ModuleVersion}");

            var callback = Utilities.SafeExecute((ptr) =>
            {
                var marshal = new CMarshalObject(ptr, true);

                var gameEvent = marshal.GetValue<GameEvent>();
                handler?.Invoke(gameEvent);

                return IntPtr.Zero;
            });
            callbacks.Add(handler, callback);

            var handleRef = new HandleRef(null, Marshal.GetFunctionPointerForDelegate(callback));
            NativePINVOKE.CEventManager_HookEvent(_eventManager, name, handleRef, false);
        }

        public static void Unsubscribe(string name, Delegate handler)
        {
            if (_eventManager.Handle == IntPtr.Zero)
            {
                _eventManager = new HandleRef(null, NativePINVOKE.g_event_manager_get());
            }

            Server.PrintToConsole($"Deregistering event {name} for plugin");

            // TODO: needs to also check the event name
            if (callbacks.ContainsKey(handler))
            {
                var handleRef = new HandleRef(null, Marshal.GetFunctionPointerForDelegate(callbacks[handler]));
                NativePINVOKE.CEventManager_UnhookEvent(_eventManager, name, handleRef, false);
                callbacks.Remove(handler);
            }

        }
    }*/
}
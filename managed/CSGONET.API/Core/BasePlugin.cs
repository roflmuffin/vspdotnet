using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using CSGONET.API.Modules.Commands;
using CSGONET.API.Modules.Cvars;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Listeners;
using CSGONET.API.Modules.Players;

namespace CSGONET.API.Core
{ public abstract class BasePlugin : IPlugin
    {
        public BasePlugin()
        {

        }

        public abstract string ModuleName { get; }
        public abstract string ModuleVersion { get; }

        public virtual void Load(bool hotReload)
        {
        }

        public virtual void Unload(bool hotReload)
        {
        }

        public class CallbackSubscriber
        {
            private Delegate _underlyingMethod;
            private int _functionReferenceIdentifier;
            private object _value;

            private InputArgument _inputArgument;

            public CallbackSubscriber(object value, Delegate underlyingMethod, Delegate wrapperMethod)
            {
                _value = value;
                _underlyingMethod = underlyingMethod;

                var functionReference = FunctionReference.Create(wrapperMethod);
                _inputArgument = (InputArgument)functionReference;

                _functionReferenceIdentifier = functionReference.Identifier;
            }

            public InputArgument GetInputArgument()
            {
                return _inputArgument;
            }

            public int GetReferenceIdentifier()
            {
                return _functionReferenceIdentifier;
            }

            public object GetValue()
            {
                return _value;
            }
        }

        public Dictionary<Delegate, CallbackSubscriber> handlers = new Dictionary<Delegate, CallbackSubscriber>();
        public Dictionary<Delegate, CallbackSubscriber> commandHandlers = new Dictionary<Delegate, CallbackSubscriber>();
        public Dictionary<Delegate, CallbackSubscriber> convarChangeHandlers = new Dictionary<Delegate, CallbackSubscriber>();
        public Dictionary<Delegate, CallbackSubscriber> listeners = new Dictionary<Delegate, CallbackSubscriber>();

        public void RegisterEventHandler(string name, Action<GameEvent> handler)
        {
            var wrappedHandler = new Action<IntPtr>((IntPtr pointer) =>
            {
                Console.WriteLine("Received pointer on C# side: " + String.Format("0x{0:X}", pointer));
                handler.Invoke(new GameEvent(pointer));
            });

            var subscriber = new CallbackSubscriber(name, handler, wrappedHandler);
            NativeAPI.HookEvent(name, subscriber.GetInputArgument());
            handlers[handler] = subscriber;
        }

        public void DeregisterEventHandler(string name, Action<GameEvent> handler)
        {
            if (handlers.ContainsKey(handler))
            {
                var subscriber = handlers[handler];

                NativeAPI.UnhookEvent(name, subscriber.GetInputArgument());
                FunctionReference.Remove(subscriber.GetReferenceIdentifier());
                handlers.Remove(handler);
            }
        }

        public void AddCommand(string name, string description, CommandInfo.CommandCallback handler)
        {
            var wrappedHandler = new Action<int, IntPtr>((i, ptr) =>
            {
                var command = new CommandInfo(ptr);
                handler?.Invoke(i, command);
            });

            var subscriber = new CallbackSubscriber(name, handler, wrappedHandler);
            NativeAPI.AddCommand(name, description, false, 0, subscriber.GetInputArgument());
            commandHandlers[handler] = subscriber;
        }

        public void RemoveCommand(string name, CommandInfo.CommandCallback handler)
        {
            if (commandHandlers.ContainsKey(handler))
            {
                var subscriber = commandHandlers[handler];

                NativeAPI.RemoveCommand(name, subscriber.GetInputArgument());

                FunctionReference.Remove(subscriber.GetReferenceIdentifier());
                commandHandlers.Remove(handler);
            }
        }

        public void HookConVarChange(ConVar convar, ConVar.ConVarChangedCallback handler)
        {
            var wrappedHandler = new Action<IntPtr, string, string>((ptr, oldVal, newVal) =>
            {
                handler?.Invoke(new ConVar(ptr), oldVal, newVal);
            });

            var subscriber = new CallbackSubscriber(convar, handler, wrappedHandler);
            NativeAPI.HookConvarChange(convar.Handle, subscriber.GetInputArgument());
            convarChangeHandlers[handler] = subscriber;
        }
        public void UnhookConVarChange(ConVar convar, ConVar.ConVarChangedCallback handler)
        {
            if (convarChangeHandlers.ContainsKey(handler))
            {
                var subscriber = convarChangeHandlers[handler];

                NativeAPI.UnhookConvarChange(convar.Handle, subscriber.GetInputArgument());
                FunctionReference.Remove(subscriber.GetReferenceIdentifier());
                commandHandlers.Remove(handler);
            }
        }

        private void AddListener<T>(string name, Listeners.SourceEventHandler<T> handler, Action<T, ScriptContext> input = null, Action<T, ScriptContext> output = null) where T : EventArgs, new()
        {
            var wrappedHandler = new Action<ScriptContext>((ScriptContext context) =>
            {
                var eventArgs = new T();

                // Before crossing the border, gets all the correct data from the context
                input?.Invoke(eventArgs, context);

                // Invoke the actual event.
                handler?.Invoke(eventArgs);

                // After crossing the border, puts all the correct "return" data back onto the context
                output?.Invoke(eventArgs, context);
            });

            var subscriber = new CallbackSubscriber(name, handler, wrappedHandler);
            NativeAPI.AddListener(name, subscriber.GetInputArgument());
            listeners[handler] = subscriber;
        }

        public void RemoveListener<T>(string name, Listeners.SourceEventHandler<T> handler)
            where T : EventArgs, new()
        {
            if (listeners.ContainsKey(handler))
            {
                var subscriber = listeners[handler];

                NativeAPI.RemoveListener(name, subscriber.GetInputArgument());
                FunctionReference.Remove(subscriber.GetReferenceIdentifier());
                listeners.Remove(handler);
            }
        }

        public void RemoveListener(string name, Delegate handler)
        {
            if (listeners.ContainsKey(handler))
            {
                var subscriber = listeners[handler];

                NativeAPI.RemoveListener(name, subscriber.GetInputArgument());
                FunctionReference.Remove(subscriber.GetReferenceIdentifier());
                listeners.Remove(handler);
            }
        }

        public event Listeners.SourceEventHandler<Listeners.PlayerConnectArgs> OnClientConnect
        {
            add => AddListener("OnClientConnect", value,
                (args, context) =>
                {
                    args.PlayerIndex = context.GetArgument<int>(0);
                    args.Name = context.GetArgument<string>(1);
                    args.Address = context.GetArgument<string>(2);
                },
                ((args, context) =>
                {
                    context.Reset();
                    context.Push(args.Cancel);
                    context.Push(args.CancelReason);
                })
            );
            remove => RemoveListener("OnClientConnect", value);
        }

        public event Listeners.SourceEventHandler<Listeners.PlayerArgs> OnClientConnected
        {
            add => AddListener("OnClientConnected", value,
                (args, context) => args.Player = context.GetArgument<Player>(0));
            remove => RemoveListener("OnClientConnected", value);
        }

        public event Listeners.SourceEventHandler<Listeners.PlayerArgs> OnClientDisconnect
        {
            add => AddListener("OnClientDisconnect", value,
                (args, context) => args.Player = context.GetArgument<Player>(0));
            remove => RemoveListener("OnClientDisconnect", value);
        }

        public event Listeners.SourceEventHandler<Listeners.MapStartArgs> OnMapStart
        {
            add => AddListener("OnMapStart", value,
                (args, context) => args.MapName = context.GetArgument<string>(0));
            remove => RemoveListener("OnMapStart", value);
        }

        public event Listeners.SourceEventHandler<EventArgs> OnTick
        {
            add => AddListener("OnTick", value);
            remove => RemoveListener("OnTick", value);
        }

        public event Listeners.SourceEventHandler<Listeners.PlayerArgs> OnClientDisconnectPost
        {
            add => AddListener("OnClientDisconnectPost", value,
                (args, context) => args.Player = context.GetArgument<Player>(0));
            remove => RemoveListener("OnClientDisconnectPost", value);
        }

        public event Listeners.SourceEventHandler<Listeners.PlayerArgs> OnClientPutInServer
        {
            add => AddListener("OnClientPutInServer", value,
                (args, context) => args.Player = context.GetArgument<Player>(0));
            remove => RemoveListener("OnClientPutInServer", value);
        }
    }
}
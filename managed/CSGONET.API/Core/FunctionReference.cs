using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

namespace CSGONET.API.Core
{
    public class FunctionReference
    {
        private readonly Delegate m_method;

        public unsafe delegate void CallbackDelegate(fxScriptContext* context);
        private CallbackDelegate s_callback;

        private FunctionReference(Delegate method)
        {
            m_method = method;

            unsafe
            {
                var dg = new CallbackDelegate((fxScriptContext* context) =>
                {
                    try
                    {
                        var scriptContext = new ScriptContext(context);

                        if (method.Method.GetParameters().FirstOrDefault()?.ParameterType == typeof(ScriptContext))
                        {
                            var returnO = m_method.DynamicInvoke(scriptContext);
                            if (returnO != null)
                            {
                                scriptContext.SetResult(returnO, context);
                            }

                            return;
                        }

                        var paramsList = method.Method.GetParameters().Select((x, i) =>
                        {
                            var param = method.Method.GetParameters()[i];
                            object obj = null;
                            if (typeof(NativeObject).IsAssignableFrom(param.ParameterType))
                            {
                                obj = Activator.CreateInstance(param.ParameterType,
                                    new[] {scriptContext.GetArgument(typeof(IntPtr), i)});
                            }
                            else
                            {
                                obj = scriptContext.GetArgument(param.ParameterType, i);
                            }
                            return obj;
                        }).ToArray();

                        var returnObj = m_method.DynamicInvoke(paramsList);

                        if (returnObj != null)
                        {
                            scriptContext.SetResult(returnObj, context);
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                    }

                    /*try
                    {
                        var scriptContext = new ScriptContext(fxScriptContext);

                        object returnObj = null;

                        if (method.Method.GetParameters().FirstOrDefault()?.ParameterType == typeof(ScriptContext))
                        {
                            returnObj = m_method.DynamicInvoke(scriptContext);
                        }
                        else
                        {
                            var paramsList = method.Method.GetParameters().Select((x, i) =>
                            {
                                var param = method.Method.GetParameters()[i];
                                var obj = scriptContext.GetArgument(param.ParameterType, i);
                                return obj;
                            }).ToArray();

                            returnObj = m_method.DynamicInvoke(paramsList);
                        }

                        if (returnObj != null)
                        {
                            scriptContext.SetResult(returnObj, fxScriptContext);
                        }
                    }
                    catch (Exception e)
                    {
                        Console.WriteLine(e);
                    }*/
                });
                s_callback = dg;
            }

        }

        public int Identifier { get; private set; }

        public static FunctionReference Create(Delegate method)
        {
            if (references.ContainsKey(method))
            {
                return references[method];
            }

            var reference = new FunctionReference(method);
            var referenceId = Register(reference);

            reference.Identifier = referenceId;

            Console.BackgroundColor = ConsoleColor.Blue;
            Console.ForegroundColor = ConsoleColor.White;
            Console.WriteLine($"Created function/callback reference: {referenceId}, {method.Method}");
            Console.ResetColor();

            return reference;
        }

        private static Dictionary<int, FunctionReference> ms_references = new Dictionary<int, FunctionReference>();
        private static int ms_referenceId;

        private static Dictionary<Delegate, FunctionReference> references =
            new Dictionary<Delegate, FunctionReference>();

        private static int Register(FunctionReference reference)
        {
            var thisRefId = ms_referenceId;
            ms_references[thisRefId] = reference;
            references[reference.m_method] = reference;

            unchecked { ms_referenceId++; }

            return thisRefId;
        }

        public static FunctionReference Get(int reference)
        {
            if (ms_references.ContainsKey(reference))
            {
                return ms_references[reference];
            }

            return null;
        }

        /*
        [UnmanagedCallersOnly]
        public static unsafe void Invoke(int reference, fxScriptContext scriptContext)
        {
            Console.WriteLine("Invoking function reference: " + reference);
            if (!ms_references.TryGetValue(reference, out var funcRef))
            {
                Debug.WriteLine("No such reference for {0}.", reference);

                // return nil
            }
        }*/

        public IntPtr GetFunctionPointer()
        {
            IntPtr cb = Marshal.GetFunctionPointerForDelegate(s_callback);
            return cb;
        }

        public static void Remove(int reference)
        {
            if (ms_references.TryGetValue(reference, out var funcRef))
            {
                ms_references.Remove(reference);

                Console.BackgroundColor = ConsoleColor.Blue;
                Console.ForegroundColor = ConsoleColor.White;
                Console.WriteLine($"Removing function/callback reference: {reference}");
                Console.ResetColor();
            }
        }

        /*public static byte[] Invoke(int reference, byte[] arguments)
        {
            if (!ms_references.TryGetValue(reference, out var funcRef))
            {
                Debug.WriteLine("No such reference for {0}.", reference);

                // return nil
                return new byte[] { 0xC0 };
            }

            var method = funcRef.m_method;

            // deserialize the passed arguments
            var argList = (List<object>)MsgPackDeserializer.Deserialize(arguments);
            var argArray = CallUtilities.GetPassArguments(method.Method, argList.ToArray(), string.Empty);

            // the Lua runtime expects this to be an array, so it be an array.
            var rv = method.DynamicInvoke(argArray);

            // is this actually an asynchronous method?
            if (rv is Task)
            {
                dynamic rt = rv;

                rv = new
                {
                    __cfx_async_retval = new Action<dynamic>(rvcb =>
                    {
                        rt.ContinueWith(new Action<Task>(t =>
                        {
                            rvcb(new object[] { rt.Result }, false);
                        }));
                    })
                };
            }

            return MsgPackSerializer.Serialize(new[] { rv });
        }

        public static int Duplicate(int reference)
        {
            FunctionReference funcRef;

            if (ms_references.TryGetValue(reference, out funcRef))
            {
                funcRef.m_refCount++; // TODO: interlocked?
                return reference;
            }

            return -1;
        }

        */
    }
}
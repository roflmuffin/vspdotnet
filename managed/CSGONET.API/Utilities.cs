using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using CSGONET.API.Modules.Commands;
using CSGONET.API.Modules.Listeners;

namespace CSGONET.API
{
    public static class Utilities
    {
        public static HandleRef ToHandle(this IntPtr ptr)
        {
            return new HandleRef(null, ptr);
        }

        public static T ToObject<T>(this IntPtr ptr, bool ownMemory = false) where T : BaseNative
        {
            if (ptr == IntPtr.Zero) return null;

            if (ownMemory)
            {
                return (T)Activator.CreateInstance(typeof(T), ptr, true);
            }
            
            return (T)Activator.CreateInstance(typeof(T), ptr, false);

        }

        public static T ToObjectNativeObject<T>(this IntPtr ptr, bool ownMemory = false) where T : NativeObject
        {
            if (ptr == IntPtr.Zero) return null;

            if (ownMemory)
            {
                return (T)Activator.CreateInstance(typeof(T), ptr, true);
            }

            return (T)Activator.CreateInstance(typeof(T), ptr, false);

        }

        public static Listeners.MarshalCallback SafeExecute(Listeners.MarshalCallback callback)
        {
            return (ptr) =>
            {
                try
                {
                    return callback.Invoke(ptr);
                }
                catch (Exception e)
                {
                    /*var callingAssembly = callback.Method.DeclaringType.Assembly;
                    var moduleClass = callingAssembly.GetTypes().FirstOrDefault(x => typeof(BasePlugin).IsAssignableFrom(x));
                    var plugin = GlobalContext.Instance.FindPluginByType(moduleClass);*/
                    Server.PrintToConsole($"Plugin threw error: {e.Message}\n{e.StackTrace}");
                }

                return IntPtr.Zero; ;
            };
        }

        public static IEnumerable<T> FlagsToList<T>(this T flags) where T : Enum
        {
            return Enum.GetValues(typeof(T)).Cast<T>()
                .Where(x => flags.HasFlag(x)).AsEnumerable();
        }
    }
}

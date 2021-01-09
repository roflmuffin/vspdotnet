using System.Runtime.InteropServices;

namespace CSGONET.API.Modules.Errors
{
    public class ExceptionHelper
    {
        public delegate void ExceptionDelegate(string message);

        static ExceptionDelegate applicationDelegate = SetPendingException;

        static void SetPendingException(string message)
        {
            PendingException.Set(new global::System.Exception(message, PendingException.Retrieve()));
        }

        static ExceptionHelper()
        {
            NativePINVOKE.RegisterExceptionCallback(new HandleRef(applicationDelegate, Marshal.GetFunctionPointerForDelegate(applicationDelegate)));
        }

        public string Test { get; set; }

        public static void CheckError()
        {
            if (PendingException.Pending)
            {
                throw PendingException.Retrieve();
            }
        }

        public static ExceptionHelper exceptionHelper = new ExceptionHelper();
    }

    public class PendingException
    {
        [global::System.ThreadStatic]
        private static global::System.Exception pendingException = null;
        private static int numExceptionsPending = 0;
        private static global::System.Object exceptionsLock = null;

        public static bool Pending
        {
            get
            {
                bool pending = false;
                if (numExceptionsPending > 0)
                    if (pendingException != null)
                        pending = true;
                return pending;
            }
        }

        public static void Set(global::System.Exception e)
        {
            if (pendingException != null)
                throw new global::System.ApplicationException("FATAL: An earlier pending exception from unmanaged code was missed and thus not thrown (" + pendingException.ToString() + ")", e);
            pendingException = e;
            lock (exceptionsLock)
            {
                numExceptionsPending++;
            }
        }

        public static global::System.Exception Retrieve()
        {
            global::System.Exception e = null;
            if (numExceptionsPending > 0)
            {
                if (pendingException != null)
                {
                    e = pendingException;
                    pendingException = null;
                    lock (exceptionsLock)
                    {
                        numExceptionsPending--;
                    }
                }
            }
            return e;
        }

        static PendingException()
        {
            exceptionsLock = new global::System.Object();
        }
    }
}
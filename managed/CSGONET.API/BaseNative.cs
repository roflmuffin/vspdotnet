using System;
using System.Collections.Generic;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;
using System.Text;

namespace CSGONET.API
{
    public abstract class NativeObject
    {
        public IntPtr Handle { get; protected set; }

        protected NativeObject(IntPtr pointer)
        {
            Handle = pointer;
        }
    }

    public abstract class BaseNative : IDisposable
    {
        protected HandleRef ptr;
        protected bool swigCMemOwn;

        public HandleRef Handle() => ptr;
        public IntPtr Pointer => Handle().Handle;

        public BaseNative(global::System.IntPtr cPtr, bool cMemoryOwn)
        {
            swigCMemOwn = cMemoryOwn;
            ptr = new global::System.Runtime.InteropServices.HandleRef(this, cPtr);
        }

        internal static global::System.Runtime.InteropServices.HandleRef getCPtr(BaseNative obj)
        {
            return (obj == null) ? new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero) : obj.ptr;
        }

        ~BaseNative()
        {
            Dispose(false);
        }

        public void Dispose()
        {
            Dispose(true);
            global::System.GC.SuppressFinalize(this);
        }

        protected abstract void OnDispose();

        protected virtual void Dispose(bool disposing)
        {
            lock (this)
            {
                if (ptr.Handle != global::System.IntPtr.Zero)
                {
                    if (swigCMemOwn)
                    {
                        swigCMemOwn = false;
                        OnDispose();
                    }
                    ptr = new global::System.Runtime.InteropServices.HandleRef(null, global::System.IntPtr.Zero);
                }
            }
        }
    }
}

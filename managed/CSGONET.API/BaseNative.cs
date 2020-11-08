/*
 *  This file is part of VSP.NET.
 *  VSP.NET is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  VSP.NET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with VSP.NET.  If not, see <https://www.gnu.org/licenses/>. *
 */

using System;
using System.Collections.Generic;
using System.Reflection.Metadata;
using System.Runtime.InteropServices;
using System.Text;

namespace CSGONET.API
{
    public abstract class NativeObject
    {
        public IntPtr Handle { get; internal set; }

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

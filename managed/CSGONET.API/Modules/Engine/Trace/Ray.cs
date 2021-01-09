using System;

namespace CSGONET.API.Modules.Engine.Trace
{
    public class Ray : BaseNative
    {
        public Ray(IntPtr cPtr, bool ownMemory) : base(cPtr, ownMemory)
        {
        }

        protected override void OnDispose()
        {
            NativePINVOKE.delete_Ray_t(ptr);
        }
    }
}
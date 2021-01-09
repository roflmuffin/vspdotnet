using System;

namespace CSGONET.API.Modules.Engine.Trace
{
    public class TraceSurface : BaseNative
    {
        public TraceSurface(IntPtr cPtr, bool ownMemory) : base(cPtr, ownMemory)
        {
        }

        protected override void OnDispose()
        {
            NativePINVOKE.delete_csurface_t(ptr);
        }

        public string Name => NativePINVOKE.csurface_t_name_get(ptr);
        public int Flags => NativePINVOKE.csurface_t_flags_get(ptr);
        public int Properties => NativePINVOKE.csurface_t_surfaceProps_get(ptr);
    }
}
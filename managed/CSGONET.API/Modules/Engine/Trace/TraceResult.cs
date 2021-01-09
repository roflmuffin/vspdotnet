using System;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Utils;

namespace CSGONET.API.Modules.Engine.Trace
{
    public class TraceResult : NativeObject
    {
        public TraceResult(IntPtr cPtr) : base(cPtr)
        {
        }

        public TraceResult() : base(NativeAPI.NewTraceResult())
        {

        }

        public bool DidHit()
        {
            return NativeAPI.TraceDidHit(Handle);
        }

        public BaseEntity Entity
        {
            get
            {
                var entity = new BaseEntity(NativeAPI.TraceResultEntity(Handle));
                if (entity?.IsNetworked == true)
                {
                    return entity;
                }

                return null;
            }
        }

        /*public TraceSurface Surface => NativePINVOKE.CGameTrace_surface_get(ptr).ToObject<TraceSurface>();
        public int Hitbox => NativePINVOKE.CGameTrace_hitbox_get(ptr);
        public int Hitgroup => NativePINVOKE.CGameTrace_hitgroup_get(ptr);
        public float FractionLeftSolid => NativePINVOKE.CGameTrace_fractionleftsolid_get(ptr);
        public int PhysicsBone => NativePINVOKE.CGameTrace_physicsbone_get(ptr);*/

        /*public Vector StartPosition => NativePINVOKE.CBaseTrace_startpos_get(ptr).ToObject<Vector>();
        public Vector EndPosition => NativePINVOKE.CBaseTrace_endpos_get(ptr).ToObject<Vector>();*/
    }
}
using System;
using System.Linq;
using System.Numerics;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Errors;
using CSGONET.API.Modules.Utils;
using Vector = CSGONET.API.Modules.Utils.Vector;

namespace CSGONET.API.Modules.Entities
{
    public class BaseEntity : NativeObject
    {
        internal BaseEntity(IntPtr pointer) : base(pointer)
        {
        }

        public override string ToString()
        {
            return string.Format("{0:X}", Handle);
        }

        #region Props
        public int GetProp(PropType type, string name) => NativeAPI.EntityGetPropInt(Handle, (int) type, name);

        public T GetProp<T>(PropType type, string name) => (T) (NativeAPI.EntityGetPropInt(Handle, (int) type, name) as object);

        public void SetProp(PropType type, string name, int value) => NativeAPI.EntitySetPropInt(Handle, (int)type, name, value);

        public void SetProp<T>(PropType type, string name, T value) => NativeAPI.EntitySetPropInt(Handle, (int)type, name, (int)(object)value);

        public bool GetPropBool(PropType type, string name) => GetProp(type, name) == 1;

        public void SetPropBool(PropType type, string name, bool value) => SetProp(type, name, value ? 1 : 0);

        public float GetPropFloat(PropType type, string name) => NativeAPI.EntityGetPropFloat(Handle, (int) type, name);

        public void SetPropFloat(PropType type, string name, float value) => NativeAPI.EntitySetPropFloat(Handle, (int)type, name, value);

        public Vector GetPropVector(PropType type, string name) => new(NativeAPI.EntityGetPropVector(Handle, (int) type, name));

        public void SetPropVector(PropType type, string name, Vector vector) => NativeAPI.EntitySetPropVector(Handle, (int) type, name, vector.Handle);

        public string GetPropString(PropType type, string name) => NativeAPI.EntityGetPropString(Handle, (int)type, name);

        public void SetPropString(PropType type, string name, string value) => NativeAPI.EntitySetPropString(Handle, (int)type, name, value);

        public BaseEntity GetPropEnt(PropType type, string name)
        {
            var returnVal = NativeAPI.EntityGetPropEnt(Handle, (int) type, name);
            if (returnVal < 0) return null;
            return BaseEntity.FromIndex(returnVal);
        }

        public void SetPropEnt(PropType type, string name, int index) => NativeAPI.EntitySetPropEnt(Handle, (int) type, name, index);

        #endregion

        #region KeyValues
        public string GetKeyValue(string name) => NativeAPI.EntityGetKeyvalue(Handle, name);

        public Vector GetKeyValueVector(string name)
        {
            var values = GetKeyValue(name).Split(new []{" "}, StringSplitOptions.None).Select(float.Parse).ToArray();
            return new Vector(values[0], values[1], values[2]);
        }

        public float GetKeyValueFloat(string name)
        {
            return Convert.ToSingle(GetKeyValue(name));
        }

        public void SetKeyValue(string name, string value) => NativeAPI.EntitySetKeyvalue(Handle, name, value);

        public void SetKeyValueFloat(string name, float value) => SetKeyValue(name, value.ToString());

        public void SetKeyValueVector(string name, Vector vec)
        {
            string strValue = $"{vec.X} {vec.Y} {vec.Z}";
            NativeAPI.EntitySetKeyvalue(Handle, name, strValue);
        }

        #endregion

        public bool IsPlayer => NativeAPI.EntityIsPlayer(Handle);
        public bool IsWeapon => NativeAPI.EntityIsWeapon(Handle);

        public bool IsNetworked => NativeAPI.EntityIsNetworked(Handle);
        /*public bool IsPlayer => NativePINVOKE.CBaseEntityWrapper_IsPlayer(ptr);
        public bool IsWeapon => NativePINVOKE.CBaseEntityWrapper_IsWeapon(ptr);
        public bool IsMoving => NativePINVOKE.CBaseEntityWrapper_IsMoving(ptr);*/

        
        public Vector Origin
        {
            get => GetKeyValueVector("origin");
            set => SetKeyValueVector("origin", value);
        }
        

        public Vector Maxs
        {
            get => GetPropVector(PropType.Send, "m_Collision.m_vecMaxs");
            set => SetPropVector(PropType.Send, "m_Collision.m_vecMaxs", value);
        }

        public Vector Mins
        {
            get => GetPropVector(PropType.Send, "m_Collision.m_vecMins");
            set => SetPropVector(PropType.Send, "m_Collision.m_vecMins", value);
        }

        public int EntityFlags
        {
            get => GetProp(PropType.Data, "m_iEFlags");
            set => SetProp(PropType.Data, "m_iEFlags", value);
        }

        public SolidType SolidType
        {
            get => (SolidType)GetProp(PropType.Send, "m_Collision.m_nSolidType");
            set => SetProp(PropType.Send, "m_Collision.m_nSolidType", (int)value);
        }

        public SolidFlags SolidFlags
        {
            get => GetProp<SolidFlags>(PropType.Send, "m_Collision.m_usSolidFlags");
            set => SetProp(PropType.Send, "m_Collision.m_usSolidFlags", value);
        }

        public CollisionGroup CollisionGroup
        {
            get => GetProp<CollisionGroup>(PropType.Send, "m_CollisionGroup");
            set => SetProp(PropType.Send, "m_CollisionGroup", value);
        }

        // TODO: ENTITY RENDER COLOR

        public float Elasticity
        {
            get => GetPropFloat(PropType.Send, "m_flElasticity");
            set => SetPropFloat(PropType.Send, "m_flElasticity", value);
        }

        public BaseEntity GroundEntity
        {
            get => GetPropEnt(PropType.Data, "m_hGroundEntity");
            set => SetPropEnt(PropType.Data, "m_hGroundEntity", value.Index);
        }

        public Team Team
        {
            get => GetProp<Team>(PropType.Send, "m_iTeamNum");
            set => SetProp(PropType.Send, "m_iTeamNum", value);
        }

        public RenderFx RenderFx
        {
            get => (RenderFx)GetProp(PropType.Send, "m_nRenderFX");
            set => SetProp(PropType.Send, "m_nRenderFX", (int)value);
        }

        public RenderMode RenderMode
        {
            get => (RenderMode)GetProp(PropType.Send, "m_nRenderMode");
            set => SetProp(PropType.Send, "m_nRenderMode", (int)value);
        }

        public MoveType MoveType
        {
            get => (MoveType)GetProp(PropType.Send, "movetype");
            set => SetProp(PropType.Send, "movetype", (int)value);
        }

        public int ParentHandle
        {
            get
            {
                try
                {
                    return GetProp(PropType.Data, "m_pParent");
                }
                catch (Exception)
                {
                    return -1;
                }
            }
            set => SetProp(PropType.Data, "m_pParent", value);
        }

        public Vector Angles
        {
            get => GetKeyValueVector("angles");
            set => SetKeyValueVector("angles", value);
        }

        public string TargetName
        {
            get => GetKeyValue("targetname");
            set => SetKeyValue("targetname", value);
        }

        // TODO: Entity Owner Handle

        public Vector AngVelocity
        {
            get => GetPropVector(PropType.Data, "m_vecAngVelocity");
            set => SetPropVector(PropType.Data, "m_vecAngVelocity", value);
        }

        public Vector BaseVelocity
        {
            get => GetPropVector(PropType.Data, "m_vecBaseVelocity");
            set => SetPropVector(PropType.Data, "m_vecBaseVelocity", value);
        }

        public string DamageFilter
        {
            get => GetKeyValue("damagefilter");
            set => SetKeyValue("damagefilter", value);
        }

        public int Effects
        {
            get => GetProp(PropType.Data, "m_fEffects");
            set => SetProp(PropType.Data, "m_fEffects", value);
        }

        public float Friction
        {
            get => GetPropFloat(PropType.Data, "m_flFriction");
            set => SetPropFloat(PropType.Data, "m_flFriction", value);
        }

        public string GlobalName
        {
            get => GetKeyValue("globalname");
            set => SetKeyValue("globalname", value);
        }

        public float Gravity
        {
            get => GetPropFloat(PropType.Data, "m_flGravity");
            set => SetPropFloat(PropType.Data, "m_flGravity", value);
        }

        public int HammerId
        {
            get => GetProp(PropType.Data, "m_iHammerID");
            set => SetProp(PropType.Data, "m_iHammerID", value);
        }
        public int Health
        {
            get => GetProp(PropType.Data, "m_iHealth");
            set => SetProp(PropType.Data, "m_iHealth", value);
        }

        public float LocalTime
        {
            get => GetPropFloat(PropType.Data, "m_flLocalTime");
            set => SetPropFloat(PropType.Data, "m_flLocalTime", value);
        }

        public int MaxHealth
        {
            get => GetProp(PropType.Data, "m_iMaxHealth");
            set => SetProp(PropType.Data, "m_iMaxHealth", value);
        }

        public string ParentName
        {
            get => GetKeyValue("parentname");
            set => SetKeyValue("parentname", value);
        }

        public float ShadowCastDistance
        {
            get => GetPropFloat(PropType.Send, "m_flShadowCastDistance");
            set => SetPropFloat(PropType.Send, "m_flShadowCastDistance", value);
        }
        public int SpawnFlags
        {
            get => GetProp(PropType.Data, "m_spawnflags");
            set => SetProp(PropType.Data, "m_spawnflags", value);
        }

        public float Speed
        {
            get
            {
                try
                {
                    return GetPropFloat(PropType.Data, "m_flLaggedMovementValue");
                }
                catch (Exception)
                {
                    return GetKeyValueFloat("speed");
                }
            }

            set
            {
                try
                {
                    SetPropFloat(PropType.Data, "m_flLaggedMovementValue", value);
                }
                catch (Exception)
                {
                    SetKeyValueFloat("speed", value);
                }
            }
        }

        public string Target
        {
            get => GetKeyValue("target");
            set => SetKeyValue("target", value);
        }

        public Vector Velocity
        {
            get => GetPropVector(PropType.Data, "m_vecVelocity");
            set => SetPropVector(PropType.Data, "m_vecVelocity", value);
        }

        public int WaterLevel
        {
            get => GetProp(PropType.Data, "m_nWaterLevel");
            set => SetProp(PropType.Data, "m_nWaterLevel", value);
        }

        public Vector Rotation
        {
            get => GetPropVector(PropType.Data, "m_angRotation");
            set => SetPropVector(PropType.Data, "m_angRotation", value);
        }

        public int Index => NativeAPI.IndexFromBaseentity(Handle);

        public string ClassName => NativeAPI.EntityGetClassname(Handle);

        public Vector AbsVelocity
        {
            get => GetPropVector(PropType.Data, "m_vecAbsVelocity");
            set => SetPropVector(PropType.Data, "m_vecAbsVelocity", value);
        }

        public Vector AbsOrigin
        {
            get => GetPropVector(PropType.Data, "m_vecAbsOrigin");
            set => SetPropVector(PropType.Data, "m_vecAbsOrigin", value);
        }


        public void Spawn() => NativeAPI.EntitySpawn(Handle);

        public static BaseEntity Create(string className)
        {
            var newPtr = NativeAPI.EntityCreateByClassname(className);
            if (newPtr == IntPtr.Zero) return null;
            return new BaseEntity(newPtr);
        }

        public static BaseEntity FromIndex(int index)
        {
            var newPtr = NativeAPI.BaseentityFromIndex(index);
            if (newPtr == IntPtr.Zero) return null;
            return new BaseEntity(newPtr);
        }

        public static BaseEntity FindByClassname(int startIndex, string className)
        {
            var newPtr = NativeAPI.EntityFindByClassname(startIndex, className);
            if (newPtr == IntPtr.Zero) return null;
            return new BaseEntity(newPtr);
        }

        public static BaseEntity FindByNetClass(int startIndex, string className)
        {
            var newPtr = NativeAPI.EntityFindByNetclass(startIndex, className);
            if (newPtr == IntPtr.Zero) return null;
            return new BaseEntity(newPtr);
        }
    }
}
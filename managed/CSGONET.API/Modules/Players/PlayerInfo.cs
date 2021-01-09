using System;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Utils;

namespace CSGONET.API.Modules.Players
{
    public class PlayerInfo : NativeObject
    {
        internal PlayerInfo(IntPtr cHandle) : base(cHandle)
        {
        }

        public string Name => NativeAPI.PlayerinfoGetName(Handle);

        public int UserId => NativeAPI.PlayerinfoGetUserid(Handle);
        public string SteamId => NativeAPI.PlayerinfoGetSteamid(Handle);

        public Team Team
        {
            get => (Team)NativeAPI.PlayerinfoGetTeam(Handle);
            set => NativeAPI.PlayerinfoSetTeam(Handle, (int) value);
        }

        public int Kills => NativeAPI.PlayerinfoGetKills(Handle);
        public int Deaths => NativeAPI.PlayerinfoGetKills(Handle);

        public bool IsConnected => NativeAPI.PlayerinfoIsConnected(Handle);

        public int Armor => NativeAPI.PlayerinfoGetArmor(Handle);
        public bool IsHLTV => NativeAPI.PlayerinfoIsHltv(Handle);
        public bool IsPlayer => NativeAPI.PlayerinfoIsPlayer(Handle);
        public bool IsFakeClient => NativeAPI.PlayerinfoIsFakeclient(Handle);
        public bool IsDead => NativeAPI.PlayerinfoIsDead(Handle);
        public bool IsInAVehicle => NativeAPI.PlayerinfoIsInVehicle(Handle);
        public bool IsObserver => NativeAPI.PlayerinfoIsObserver(Handle);
        /*public Angle Origin => NativePINVOKE.IPlayerInfo_GetAbsOrigin(Handle).ToObject<Angle>();
        public Angle Angles => NativePINVOKE.IPlayerInfo_GetAbsAngles(Handle).ToObject<Angle>();
        public Angle MinSize => NativePINVOKE.IPlayerInfo_GetPlayerMins(Handle).ToObject<Angle>();
        public Angle MaxSize => NativePINVOKE.IPlayerInfo_GetPlayerMaxs(Handle).ToObject<Angle>();*/
        public string WeaponName => NativeAPI.PlayerinfoGetWeaponName(Handle);
        public string ModelName => NativeAPI.PlayerinfoGetModelName(Handle);
        public int Health => NativeAPI.PlayerinfoGetHealth(Handle);
        public int MaxHealth => NativeAPI.PlayerinfoGetMaxHealth(Handle);
        //public CBotCmd LastUserCommand => NativePINVOKE.IPlayerInfo_GetLastUserCommand(Handle);
    }
}
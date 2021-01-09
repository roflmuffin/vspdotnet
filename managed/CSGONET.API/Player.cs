/*using CSGONET.API;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Entities.Constants;

public class Player : BaseNative
{

    public string Name => NativePINVOKE.CPlayer_GetName(ptr);

    public string GetAuthString()
    {
        string ret = NativePINVOKE.CPlayer_GetAuthString(ptr);
        return ret;
    }

    public bool IsConnected => NativePINVOKE.CPlayer_IsConnected(ptr);

    public bool IsFakeClient => NativePINVOKE.CPlayer_IsFakeClient(ptr);

    public float TimeConnected => NativePINVOKE.CPlayer_GetTimeConnected(ptr);

    public float Latency => NativePINVOKE.CPlayer_GetLatency(ptr);

    public void PrintToConsole(string message)
    {
        NativePINVOKE.CPlayer_PrintToConsole(ptr, message);
    }

    public void PrintToChat(string message)
    {
        NativePINVOKE.CPlayer_PrintToChat(ptr, message);
    }

    public void PrintToHint(string message) => NativePINVOKE.CPlayer_PrintToHint(ptr, message);
    public void PrintToCenter(string message) => NativePINVOKE.CPlayer_PrintToCenter(ptr, message);

    public QAngle AbsAngles
    {
        get => new QAngle(NativePINVOKE.CPlayer_GetAbsAngles(ptr), true);
    }

    public QAngle AbsOrigin
    {
        get => new QAngle(NativePINVOKE.CPlayer_GetAbsOrigin(ptr), true);
    }

    public void Kick(string reason = null) => NativePINVOKE.CPlayer_Kick(ptr, reason);

    public string WeaponName => NativePINVOKE.CPlayer_GetWeaponName(ptr);

    public bool Alive => NativePINVOKE.CPlayer_IsAlive(ptr);

    public BaseEntity Entity
    {
        get
        {
            var intPtr = NativePINVOKE.CPlayer_GetEntity(ptr);
            if (intPtr == IntPtr.Zero) return null;
            return new BaseEntity(intPtr, false);
        }
    }

    public Team Team
    {
        get => (Team)NativePINVOKE.CPlayer_GetTeam(ptr);
        set => NativePINVOKE.CPlayer_ChangeTeam(ptr, (int)value);
    }

    public string IpAddress => NativePINVOKE.CPlayer_GetIpAddress(ptr);

    public string ModelName => NativePINVOKE.CPlayer_GetModelName(ptr);

    public PlayerButtons Buttons => (PlayerButtons)Entity.GetProp(PropType.Data, "m_nButtons");

    public IEnumerable<PlayerButtons> SelectedButtons => Buttons.FlagsToList();

    public ObserverMode ObserverMode
    {
        get => (ObserverMode)Entity.GetProp(PropType.Send, "m_iObserverMode");
        set => Entity.SetProp(PropType.Send, "m_iObserverMode", (int) value);
    }

    public Player ObserverTarget 
    {
        get
        {
            int target = Entity.GetPropEnt(PropType.Send, "m_hObserverTarget");
            if (target > 0)
            {
                return PlayerManager.GetPlayerByIndex(target);
            }

            return null;
        }
    }

    protected override void OnDispose()
    {
        //NativePINVOKE.delete_CPlayer(ptr);
    }

    public Player() : base(NativePINVOKE.new_CPlayer(), true) { }

    public Player(IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn) { }
}*/
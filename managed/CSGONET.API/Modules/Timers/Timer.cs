using System;
using CSGONET.API.Core;

namespace CSGONET.API.Modules.Timers
{
    [Flags]
    public enum TimerFlags
    {
        REPEAT = (1 << 0), // Timer will repeat until stopped
        STOP_ON_MAPCHANGE = (1 << 1)

    }

    public class Timer : NativeObject
    {
        public Timer(float interval, Action callback, TimerFlags? flags = null) : base(IntPtr.Zero)
        {
            Handle = NativeAPI.CreateTimer(interval, callback, (int)(flags ?? 0));
        }

        public void Kill()
        {
            NativeAPI.KillTimer(Handle);
        }
    }
}
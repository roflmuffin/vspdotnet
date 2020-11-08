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
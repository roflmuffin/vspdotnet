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

using System.Drawing;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Players;
using CSGONET.API.Modules.Utils;

namespace ClassLibrary2.Effects
{
    public abstract class WarcraftEffect
    {
        protected WarcraftEffect(Player owner, Player target, float duration)
        {
            Owner = owner;
            Target = target;
            Duration = duration;
            RemainingDuration = duration;
        }

        public Player Owner { get; }
        public Player Target { get; }

        public float Duration { get; }
        public float RemainingDuration { get; set; }

        public virtual void OnStart() { }
        public virtual void OnTick() { }
        public virtual void OnFinish() { }
    }
}
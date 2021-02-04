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
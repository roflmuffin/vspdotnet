using System.Collections.Generic;
using System.Runtime.CompilerServices;
using CSGONET.API.Modules.Timers;

namespace ClassLibrary2.Effects
{
    public class EffectManager
    {
        private List<WarcraftEffect> _effects = new List<WarcraftEffect>();
        private float _tickRate = 0.25f;

        public void Initialize()
        {
            WarcraftPlugin.Instance.AddTimer(_tickRate, EffectTick, TimerFlags.REPEAT);
        }

        public void AddEffect(WarcraftEffect effect)
        {
            _effects.Add(effect);
            effect.OnStart();
        }

        private void EffectTick()
        {
            for(int i = _effects.Count - 1; i >= 0; i--)
            {
                var effect = _effects[i];

                effect.RemainingDuration -= _tickRate;
                effect.OnTick();

                if (effect.RemainingDuration <= 0)
                {
                    effect.RemainingDuration = 0;
                    effect.OnFinish();
                    _effects.RemoveAt(i);
                }

            }
        }

        public void ClearEffects(int index)
        {
            for (int i = _effects.Count - 1; i >= 0; i--)
            {
                var effect = _effects[i];
                if (effect.Target?.Index == index)
                {
                    effect.OnFinish();
                    _effects.RemoveAt(i);
                }
            }
        }
    }
}
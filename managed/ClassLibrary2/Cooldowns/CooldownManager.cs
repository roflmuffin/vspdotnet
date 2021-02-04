using CSGONET.API.Modules.Sound;
using CSGONET.API.Modules.Timers;
using CSGONET.API.Modules.Utils;

namespace ClassLibrary2.Cooldowns
{
    public class CooldownManager
    {
        private float _tickRate = 0.25f;

        public void Initialize()
        {
            WarcraftPlugin.Instance.AddTimer(_tickRate, CooldownTick, TimerFlags.REPEAT);
        }

        private void CooldownTick()
        {
            foreach (var player in WarcraftPlugin.Instance.Players)
            {
                if (player == null) continue;
                for (int i = 0; i < 4; i++)
                {
                    if (player.AbilityCooldowns[i] >= 0)
                    {
                        var oldCooldown = player.AbilityCooldowns[i];
                        player.AbilityCooldowns[i] -= 0.25f;

                        if (oldCooldown > 0 && player.AbilityCooldowns[i] <= 0.0)
                        {
                            player.AbilityCooldowns[i] = 0.0f;
                            PlayEffects(player, i);
                        }
                    }
                }
            }
        }

        public bool IsAvailable(WarcraftPlayer player, int abilityIndex)
        {
            return player.AbilityCooldowns[abilityIndex] <= 0.0f;
        }

        public void StartCooldown(WarcraftPlayer player, int abilityIndex, float abilityCooldown)
        {
            player.AbilityCooldowns[abilityIndex] = abilityCooldown;
        }

        private void PlayEffects(WarcraftPlayer player, int abilityIndex)
        {
            var ability = player.GetRace().GetAbility(abilityIndex);

            Sound.EmitSound(player.Index, "items/battery_pickup.wav", origin: player.GetPlayer().AbsOrigin);

            player.SetStatusMessage($"{ChatColors.Red}{ability.DisplayName}{ChatColors.Default} ready.");
        }
    }
}
using System;
using CSGONET.API;
using CSGONET.API.Core;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Utils;

namespace ClassLibrary2
{
    public class EventSystem
    {
        private WarcraftPlugin _plugin;

        public EventSystem(WarcraftPlugin plugin)
        {
            _plugin = plugin;
        }

        public void Initialize()
        {
            _plugin.RegisterEventHandler("player_death", PlayerDeathHandler);
            _plugin.RegisterEventHandler("player_spawn", PlayerSpawnHandler);
            _plugin.RegisterEventHandler("player_hurt", PlayerHurtHandler);
        }

        private void PlayerHurtHandler(GameEvent @event)
        {
            var victim = @event.GetPlayer("userid");
            var attacker = @event.GetPlayer("attacker");

            victim?.GetWarcraftPlayer()?.GetRace()?.InvokeEvent("player_hurt", @event);
            attacker?.GetWarcraftPlayer()?.GetRace()?.InvokeEvent("player_hurt_other", @event);
        }

        private void PlayerSpawnHandler(GameEvent @event)
        {
            var player = @event.GetPlayer("userid");
            var race = player.GetWarcraftPlayer()?.GetRace();

            if (race != null)
            {
                var name = @event.Name;
                Server.NextFrame(() =>
                {
                    WarcraftPlugin.Instance.EffectManager.ClearEffects(player.Index);
                    race.InvokeEvent(name, @event);
                });
            }
        }

        private void PlayerDeathHandler(GameEvent @event)
        {
            var attacker = @event.GetPlayer("attacker");
            var victim = @event.GetPlayer("userid");
            var headshot = @event.GetBool("headshot");

            var victimIndex = NativeAPI.IndexFromUserid(@event.GetInt("userid"));

            if (attacker != null && victim != null && attacker.Index != victim.Index && !attacker.IsFakeClient)
            {
                var weaponName = attacker.ActiveWeapon?.ClassName;

                int xpToAdd = 0;
                int xpHeadshot = 0;
                int xpKnife = 0;

                xpToAdd = _plugin.XpPerKill.IntValue;

                if (headshot)
                    xpHeadshot = Convert.ToInt32(_plugin.XpPerKill.IntValue * _plugin.XpHeadshotModifier.FloatValue);

                if (weaponName == "weapon_knife")
                    xpKnife = Convert.ToInt32(_plugin.XpPerKill.IntValue * _plugin.XpKnifeModifier.FloatValue);

                xpToAdd += xpHeadshot + xpKnife;

                _plugin.XpSystem.AddXp(attacker, xpToAdd);

                string hsBonus = "";
                if (xpHeadshot != 0)
                {
                    hsBonus = $"(+{xpHeadshot} HS bonus)";
                }

                string knifeBonus = "";
                if (xpKnife != 0)
                {
                    knifeBonus = $"(+{xpKnife} knife bonus)";
                }

                string xpString = $" {ChatColors.Gold}+{xpToAdd} XP {ChatColors.Default}for killing {ChatColors.Green}{victim.Name} {ChatColors.Default}{hsBonus}{knifeBonus}";

                _plugin.GetWcPlayer(attacker.Index).SetStatusMessage(xpString);
                attacker.PrintToChat(xpString);
            }

            victim?.GetWarcraftPlayer()?.GetRace()?.InvokeEvent("player_death", @event);
            attacker?.GetWarcraftPlayer()?.GetRace()?.InvokeEvent("player_kill", @event);

            WarcraftPlugin.Instance.EffectManager.ClearEffects(victimIndex);
        }
    }
}
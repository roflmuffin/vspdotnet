using System;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Memory;
using CSGONET.API.Modules.Sound;
using CSGONET.API.Modules.Sound.Constants;
using CSGONET.API.Modules.Timers;
using CSGONET.API.Modules.Utils;

namespace ClassLibrary2.Races
{
    public class RaceUndeadScourge : WarcraftRace
    {
        private Timer _beepTimer;
        public override string InternalName => "undead_scourge";
        public override string DisplayName => "Undead Scourge";

        public override void Register()
        {
            AddAbility(new SimpleWarcraftAbility("vampiric_aura", "Vampiric Aura",
                i => $"Heal for {ChatColors.Green}10/20/30/40/50%{ChatColors.Default} of damage dealt to enemies."));
            AddAbility(new SimpleWarcraftAbility("unholy_aura", "Unholy Aura",
                i => $"Move {ChatColors.Blue}10/20/30/40/50%{ChatColors.Default} faster."));
            AddAbility(new SimpleWarcraftAbility("levitation", "Levitation",
                i => $"Experience {ChatColors.LightBlue}90/80/70/60/50%{ChatColors.Default} of gravity."));
            AddAbility(new SimpleWarcraftAbility("suicide_bomber", "Suicide Bomber",
                i =>
                    $"When you die, you create a {ChatColors.Red}miniature C4{ChatColors.Default} that will detonate after 1.5 seconds."));

            HookEvent("player_death", PlayerDeath);
            HookEvent("player_spawn", PlayerSpawn);
            HookEvent("player_hurt_other", PlayerHurtOther);

            HookAbility(3, Ultimate);
        }

        private void Ultimate()
        {
            Player.PrintToChat("You just pressed ultimate!");
        }

        public override void PlayerChangingToAnotherRace()
        {
            Player.Gravity = 1.0f;
        }

        private void PlayerHurtOther(GameEvent @event)
        {
            var victim = @event.GetPlayer("userid");

            int vampiricAuraLevel = WarcraftPlayer.GetAbilityLevel(0);
            float lifeStealPercentage = (vampiricAuraLevel * 10.0f) / 100.0f;

            int amountToHeal = Convert.ToInt32(@event.GetInt("dmg_health") * lifeStealPercentage);
            int newHealth = Player.Health + amountToHeal;

            if (newHealth >= 200)
            {
                amountToHeal = 200 - newHealth + amountToHeal;
                newHealth = 200;
            }

            if (victim != null & !string.IsNullOrEmpty(victim.Name) && amountToHeal > 0)
                WarcraftPlayer.SetStatusMessage($" {ChatColors.Green}+{amountToHeal} HP");
            //Player.PrintToChat($" {ChatColors.Yellow} You just healed {ChatColors.Green}{amountToHeal} health {ChatColors.Yellow}from damaging {victim.Name}.");

            Player.Health = newHealth;
        }

        private void PlayerSpawn(GameEvent @event)
        {
            // Handle levitation/low gravity
            int levitationLevel = WarcraftPlayer.GetAbilityLevel(2);
            float levitationModifier = 1f - (levitationLevel * 0.1f);
            Player.Gravity = levitationModifier;

            // Handle unholy aura/speed boost
            int unholyAuraLevel = WarcraftPlayer.GetAbilityLevel(1);
            float speedModifier = 1.0f + (0.1f * unholyAuraLevel);
            Player.Speed = speedModifier;
        }

        private void PlayerDeath(GameEvent @event)
        {
            if (WarcraftPlayer.GetAbilityLevel(3) > 0 && @event.GetInt("userid") != @event.GetInt("attacker"))
            {
                var ent = SpawnC4Model();

                WarcraftPlugin.Instance.AddTimer(1.5f, () => DetonateC4(ent));
                _beepTimer = WarcraftPlugin.Instance.AddTimer(0.3f, () => BeepTimer(ent), TimerFlags.REPEAT);
            }
        }

        private void BeepTimer(BaseEntity ent)
        {
            if (ent.IsValid)
            {
                var position = ent.AbsOrigin;
                Sound.EmitSoundToAll("weapons/c4/c4_click.wav", 0, origin: position, level: SoundLevel.SNDLVL_90dB);
            }
        }

        private void DetonateC4(BaseEntity ent)
        {
            var explosion = BaseEntity.Create("env_explosion");
            if (explosion?.IsValid == true)
            {
                explosion.SetProp(PropType.Data, "m_spawnflags", 16384);
                explosion.SetProp(PropType.Data, "m_iMagnitude", 200);
                explosion.SetProp(PropType.Data, "m_iRadiusOverride", 1600);


                var activateEntity = VirtualFunction.Create<IntPtr>(explosion.Handle, 38);
                activateEntity(explosion.Handle);

                var dest = new Vector(ent.AbsOrigin.X, ent.AbsOrigin.Y, ent.AbsOrigin.Z);

                var teleportEntity =
                    VirtualFunction.Create<IntPtr, IntPtr, IntPtr, IntPtr, bool>(explosion.Handle, 114);
                teleportEntity(explosion.Handle, dest.Handle, IntPtr.Zero, IntPtr.Zero, false);

                explosion.SetPropEnt(PropType.Send, "m_hOwnerEntity", Player.Index);
                explosion.Team = Player.Team;

                explosion.Spawn();

                explosion.AcceptInput("InputExplode");

                Sound.EmitSoundToAll("weapons/hegrenade/explode3.wav", 0, origin: explosion.AbsOrigin,
                    level: SoundLevel.SNDLVL_120dB);

                ent.AcceptInput("InputKill");
            }

            _beepTimer?.Kill();
        }

        private BaseEntity SpawnC4Model()
        {
            var entName = "suicide_bomb";
            var modelName = "models/weapons/w_ied.mdl";

            var ent = BaseEntity.Create("prop_physics_override");

            ent.SetKeyValueFloat("physdamagescale", 0.0f);
            ent.SetKeyValue("model", modelName);
            ent.SetKeyValue("targetname", entName);
            ent.Spawn();

            var eyeAngle = (Math.PI / 180) * Player.EyeAngle.Y;
            var destinationX = Player.AbsOrigin.X + Convert.ToSingle(50 * Math.Cos(eyeAngle));
            var destinationY = Player.AbsOrigin.Y + Convert.ToSingle(50 * Math.Sin(eyeAngle));
            var destinationZ = Player.AbsOrigin.Z;

            var teleportEntity = VirtualFunction.Create<IntPtr, IntPtr, IntPtr, IntPtr, bool>(ent.Handle, 114);
            teleportEntity(ent.Handle, Player.AbsOrigin.Handle, IntPtr.Zero, IntPtr.Zero, false);

            ent.MoveType = MoveType.MOVETYPE_VPHYSICS;

            return ent;
        }
    }
}
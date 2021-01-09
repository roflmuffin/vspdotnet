using CSGONET.API;
using CSGONET.API.Modules.Cvars;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Listeners;
using Newtonsoft.Json;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;
using System.Text;
using System.Text.Unicode;
using System.Threading;
using System.Threading.Tasks;
using CSGONET.API.Core;
using CSGONET.API.Modules.Commands;
using CSGONET.API.Modules.Engine.Trace;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Players;
using CSGONET.API.Modules.Players.Constants;
using CSGONET.API.Modules.Utils;
using Vector = CSGONET.API.Modules.Utils.Vector;

namespace TestModule
{
    public class TestPlugin : BasePlugin
    {
        private BaseEntity _gamerules;
        public override string ModuleName => "My Module";

        public override string ModuleVersion => "1.2.5";

        private IEnumerable<BaseEntity> GetPlayerEntities()
        {
            List<BaseEntity> playerEntities = new List<BaseEntity>();
            int lastIndex = -1;
            do
            {
                var entity = BaseEntity.FindByClassname(lastIndex, "player");
                if (entity == null)
                {
                    lastIndex = -1;
                }
                else
                {
                    lastIndex = entity.Index;
                    playerEntities.Add(entity);
                }
            } while (lastIndex > -1);

            return playerEntities.AsEnumerable();
        }

        public override void Load()
        {
            HookCallbacks();
            SetupConVars(); 
            HookGameEvents();
            SetupCommands();

            /*
            if (hotReload)
            {
                _gamerules = BaseEntity.FindByClassname(-1, "cs_gamerules");
            }*/
        }

        private void SetupCommands()
        {
            ConsoleCommand.AddCommand("testtwo", "this is my command", false, 0,(client, args) =>
            { 
                var message = $"My new command was just run by client {client} with num args {args.Length}";

                for (int i = 0; i < args.Length; i++)
                {
                    message = message + $"\n{i}:{args[i]}";
                }
                
                if (client > 0) 
                {
                    Player.FromIndex(client).PrintToConsole(message + "\n\0");
                }
                else
                {
                    Server.PrintToConsole(message);
                }
            });

            ConsoleCommand.AddCommand("ultimate", "Cast ultimate ability", false, 0, (i, strings) =>
            {
                var player = Player.FromIndex(i);

//                player.Entity.AbsOrigin = player.Entity.AbsOrigin.With(z: player.Entity.AbsOrigin.z + 100);
               // player.Entity.AbsOrigin.z += 100;
                /*var qOrigin = new QAngle();
                qOrigin.x = player.Entity.AbsOrigin.x;
                qOrigin.y = player.Entity.AbsOrigin.y;
                qOrigin.z = player.Entity.AbsOrigin.z + 100;

                player.Entity.AbsOrigin = qOrigin;*/
                //player.AbsVelocity = player.AbsVelocity.With(z: 300f);
                

                /*var qAngle = new QAngle();
                qAngle.x = player.Entity.AbsVelocity.x;// + Convert.ToSingle(new Random().NextDouble() * 250);
                qAngle.y = player.Entity.AbsVelocity.y;// + Convert.ToSingle(new Random().NextDouble() * 250);
                qAngle.z = player.Entity.AbsVelocity.z + 100f + Convert.ToSingle(new Random().NextDouble() * 550);

                player.Entity.AbsVelocity = qAngle;*/
                //player.PrintToChat($" \x09JUMP! \x08{player.AbsVelocity.Length}");
            });

        }

        private void SetupConVars()
        {
            var myConVar = new ConVar("test_var", "1.0", "my_desc", ConVarFlags.Notify | ConVarFlags.Replicated);
            
            ConVar anotherCvar = ConVar.Find("test_var");

            ConVar cvWinConditions = ConVar.Find("mp_ignore_round_win_conditions");

            Server.PrintToConsole($"The value of {cvWinConditions.Name} is: {cvWinConditions.BoolValue}");


            if (anotherCvar != null)
            {
                anotherCvar.Public = true;

                Server.PrintToConsole($"Test Var is public: {anotherCvar.Public}");
                 
                anotherCvar.Public = false;

                Server.PrintToConsole($"Test Var is public: {anotherCvar.Public}");
            }

            ConVar testanotherCvar = ConVar.Find("test_var");
            Server.PrintToConsole("Test Var Value: " + testanotherCvar.StringValue);

            testanotherCvar.HookChange((convar, old, newVal) =>
            {
                Server.PrintToConsole($"{convar.Name} var changed from {old} to {newVal}");
            });
        }
         
        private void HookGameEvents()
        {
            /*GameEvent.Subscribe("weapon_fire", (@event) =>
            {
                var player = @event.GetPlayer();
                //player.PrintToCenter($"FIRE! {Server.CurrentTime}");
                player.PrintToChat($"FIRE! {Server.CurrentTime}");
                player.PrintToHint($"FIRE! {Server.CurrentTime}");
            });*/
            GameEvent.Subscribe("player_say", (@event) =>
            {
                var player = Player.FromUserId(@event.GetInt("userid"));
                var createdProp = BaseEntity.Create("prop_physics_multiplayer");


                Server.PrecacheModel("models/props_c17/oildrum001.mdl");
                createdProp.SetKeyValue("model", "models/props_c17/oildrum001.mdl");
                createdProp.AbsOrigin = player.AbsOrigin.With(z: player.AbsOrigin.Z + 100);
                createdProp.AbsVelocity = new Vector(z: 300f);

                player.PrintToChat($"Just created prop of type {createdProp.ClassName} and index {createdProp.Index}, {createdProp.Origin}");

                createdProp.Spawn();

            });

            GameEvent.Subscribe("player_jump", JumpHandler);
            
            GameEvent.Subscribe("grenade_thrown", (@event) =>
            {
                var playerWhoThrew = Player.FromUserId(@event.GetInt("userid"));
                Server.PrintToConsole(
                    $"Event fired: {@event.GetName()}, type of grenade: {@event.GetString("weapon")}, thrower: {playerWhoThrew.Name}");

                var hexValue = Encoding.UTF8.GetString(new byte[] {(byte) new Random().Next(3, 13)});

                playerWhoThrew.PrintToChat($"You just threw a {hexValue}[{@event.GetString("weapon")}]\x10 {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToHint($"You just threw a [{@event.GetString("weapon")}] {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToCenter($"You just threw a [{@event.GetqString("weapon")}] {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToCenter("#SFUI_Notice_YouDroppedWeapon");
                //playerWhoThrew.PrintToCenter("This is a <font color=\"#FF0000\">test</font>");
            });

            GameEvent.Subscribe("player_hurt", (@event) =>
            {
                var victim = Player.FromUserId(@event.GetInt("userid"));
                var attacker = Player.FromUserId(@event.GetInt("attacker"));
                var hitgroup = (HitGroup)@event.GetInt("hitgroup");

                if (attacker == null) return;
                //Server.PrintToConsole($"Someone got hurt! Victim {victim.Name}, Attacker {attacker.Name}");
                attacker.PrintToChat($"Damage: \x02{@event.GetInt("dmg_health")}, \x04{hitgroup}");
                attacker.Health += @event.GetInt("dmg_health") / 2;

                if (victim != null)
                {
                    attacker.PrintToChat($"Health: {victim.Health}");

                    /*var qAngle = new QAngle();
                    qAngle.x = victim.Entity.AbsVelocity.x + Convert.ToSingle(new Random().NextDouble() * 250);
                    qAngle.y = victim.Entity.AbsVelocity.y + Convert.ToSingle(new Random().NextDouble() * 250);
                    qAngle.z = victim.Entity.AbsVelocity.z + 700f;

                    victim.Entity.AbsVelocity = qAngle;*/
                }
            });


        }

        private void JumpHandler(GameEvent @event)
        {
            var player = Player.FromUserId(@event.GetInt("userid"));
            //player.Entity.Health += 5;
            int health = player.GetProp(PropType.Send, "m_iAccount");
            int hasDefuser = player.GetProp(PropType.Send, "m_bHasDefuser");
            player.SetProp(PropType.Data, "m_iHealth", player.Health + 10);
            player.SetProp(PropType.Send, "m_iAccount", player.GetProp(PropType.Send, "m_iAccount") + 150);

            
            //player.Entity.SetPropFloat(PropType.Data, "m_flGravity", 0.5f);
            //player.Gravity -= 0.05f;

            /*player.Entity.Gravity -= 0.05f;
            player.Entity.Speed *= 1.05f;
            player.Entity.Velocity.z += 100f;*/
            player.PrintToChat($" \x04$ \x07{health} \x08{hasDefuser}");
            //player.Cash += 100;
            player.PrintToChat($"You just jumped?! \x10{Server.CurrentTime} \x01on \x09{Server.MapName} \x04{player.Gravity.ToString("n2")} \x0B{player.Speed.ToString("n2")}");

            player.PrintToChat(player.ActiveWeapon.ClassName);
            Task.Run(() =>
            {
                Task.Delay(1);
                var qAngle = new Vector();
                qAngle.X = player.AbsVelocity.X;// + Convert.ToSingle(new Random().NextDouble() * 250);
                qAngle.Y = player.AbsVelocity.Y;// + Convert.ToSingle(new Random().NextDouble() * 250);
                qAngle.Z = player.AbsVelocity.Z + Convert.ToSingle(new Random().NextDouble() * 550);

                player.AbsVelocity = qAngle;

                player.PrintToChat("Jump boost applied at frametime: " + Server.CurrentTime);

            });
            /*var qAngle = new QAngle();
            qAngle.z = 100f;

            player.Entity.Velocity = qAngle;*/
        }

        private void HookCallbacks()
        {
            async void OnListenersOnOnMapStart(EventArgs args)
            {
                Server.PrintToConsole("Server ACTIVATE (should never be seen)!");
            }

            Listeners.OnMapStart += OnListenersOnOnMapStart;
            Listeners.OnMapStart -= OnListenersOnOnMapStart;

            Listeners.OnMapStart += (args) =>
            {
                Server.PrintToConsole("Before waiting");
                Server.PrintToConsole("Server ACTIVATE!");

                var worldspawn = BaseEntity.FindByClassname(0, "worldspawn");
                Server.PrintToConsole($"Entity Index {0}, Name: {worldspawn?.ClassName}, {worldspawn == null}");

                _gamerules = BaseEntity.FindByClassname(1, "cs_gamerules");
                //throw new Exception("TEST"); 
            };

            /*Listeners.OnClientConnect += (args) =>
            {
                Server.PrintToConsole("The player is trying to connect: " + args.Player.Name);
                args.Cancel = false;
                //args.CancelReason = $"I don't like you... the time is: {DateTime.Now.ToLongTimeString()}";
            };*/

            /*Listeners.OnClientConnected += (args) =>
            {
                Server.PrintToConsole($"Player has connected: {args.Player.Name} []");
            };

            Listeners.OnClientPutInServer += (args) =>
            {
                Server.PrintToConsole("Player has been put in server: " + args.Player.Name);
            };*/

            /*
            Listeners.OnClientDisconnect += (args) =>
            {
                Server.PrintToConsole($"Client disconnected: {args.Player.Name}");
            };

            Listeners.OnClientDisconnectPost += (args) =>
            {
                Server.PrintToConsole($"Client disconnectedPost: {args.Player.Name}");
            };
            */

            Listeners.OnTick += (args) =>
            {
                if (PlayerManager.NumPlayers > 0)
                {
                    
                    if (Server.TickCount % 10 == 0)
                    {
                        var player = Player.FromIndex(1);
                        //var player = PlayerManager.GetPlayers().FirstOrDefault(x => !x.IsFakeClient && x.IsConnected);
                        if (player == null) return;

                        if (!player.IsAlive)
                        {
                            player.PrintToHint($"Observer Mode: {player.ObserverMode}, Target: {player.ObserverTarget?.Name}, Team: {player.ObserverTarget?.PlayerInfo?.Team}, Wep: {player.ObserverTarget?.ActiveWeapon?.ClassName}");
                            return;
                        }

                        var lastWeapon = player.GetPropEnt(PropType.Data, "m_hLastWeapon");
                        var nextAttack = player.ActiveWeapon.GetPropFloat(PropType.Data, "m_flNextPrimaryAttack");
                        var isInBurstMode = player.ActiveWeapon.GetProp(PropType.Send, "m_iClip1");
                        player.PrintToHint(
                            $"Speed: {player.AbsVelocity.Length().ToString("n2")}, Sqr: {player.Velocity.LengthSqr().ToString("n2")}\n" +
                            $"Time: {Server.CurrentTime.ToString("n2")}, Latency: {(player.NetInfo.GetLatency(NetFlow.FLOW_OUTGOING)).ToString("n2")}\n" +
                            $"Life State: {player.LifeState}\n" +
                            $"Buttons: {string.Join(", ", player.SelectedButtons)}\n" +
                            $"Flags: {string.Join(", ", player.Flags.FlagsToList())}\n" +
                            /*$"RenderMode: {player.RenderMode}\n" +
                            $"RenderFx: {player.RenderFx}\n" +
                            $"MoveType: {player.MoveType}\n" +*/
                            //$"ViewOffset: {player.Entity.EyeLocation}, Origin: {player.Entity.GetKeyValueVector("origin")}\n" +
                            $"Name: {player.Name}\n" +
                           // $"EyeAngle: {player.EyeAngle}, ViewVec: {player.ViewVector}\n" +
                            //$"Life State: {player.LifeState}, Stamina: {player.GetPropFloat(PropType.Send, "cslocaldata.m_flStamina")}\n" +
                            $"Bomb Zone: {player.GetProp(PropType.Send, "m_bInBombZone")}\n" +
                           // $"Is Freeze Time: {_gamerules?.ClassName} {_gamerules?.GetProp(PropType.Send, "cs_gamerules_data.m_bBombPlanted")}\n" +
                            //$"Ground Entity: {player.GetPropEnt(PropType.Data, "m_hGroundEntity")}\n" +
                            $"Last Weapon: {lastWeapon.ClassName}, Active: {player.ActiveWeapon.ClassName} ({player.ActiveWeapon.Pointer.ToString("X8")})\n" +
                            $"TickCount:{Server.TickCount}, FrameTime:{Server.GameFrameTime.ToString("n2")}, EngineTime:{Server.EngineTime.ToString("n2")}, CurTime: {Server.CurrentTime.ToString("n2")}\n" +
                            $"Time until attack: {Math.Clamp(nextAttack - Server.CurrentTime, 0, 1000).ToString("n2")}\n" +
                            $"IsMoving: {!player.Velocity.IsZero()}");
                    }
                }
            };
        }

        public override void Unload()
        {
        }
    }
}

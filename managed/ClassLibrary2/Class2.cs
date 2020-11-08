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

using CSGONET.API;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Players;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using CSGONET.API.Modules.Cvars;
using CSGONET.API.Modules.Engine.Trace;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Memory;
using CSGONET.API.Modules.Memory.Constants;
using CSGONET.API.Modules.Menus;
using CSGONET.API.Modules.Sound;
using CSGONET.API.Modules.Sound.Constants;
using CSGONET.API.Modules.Utils;
using Newtonsoft.Json;

namespace ClassLibrary2
{

    public static class PlayerExtensions
    {
        public static BaseEntity GetWeapon(this Player player, string className)
        {
            if (!player.IsAlive) return null;

            int offset = NativeAPI.FindDatamapInfo(player.Index, "m_hMyWeapons") - 4;
            for (int i = 0; i < 48; i++)
            {
                offset += 4;

                var weapon = player.GetPropEntByOffset(offset);

                if (weapon != null && weapon.IsValid && weapon.ClassName == className)
                    return weapon;
            }

            return null;
        }

        public static BaseEntity GiveWeapon(this Player player, string className, int slot = 0)
        {
            var giveNamedItem = VirtualFunction.CreateFunc<IntPtr, string, int, IntPtr, bool, ulong, IntPtr>(player.Handle, 457);
            var wep = giveNamedItem(player.Handle, className, slot, IntPtr.Zero, true, 0);
            return new BaseEntity(wep);
        }
    }

    /*public class Class1 : BasePlugin
    {
        /*private void OnSelectOption(Player player, IRadioMenuOption option)
        {
            player.PrintToChat($"You selected option:\x0B {option.Text}:{option.Value}");
            var targetPlayer = Player.FromIndex(Convert.ToInt32(option.Value));

            targetPlayer.AbsVelocity = player.AbsVelocity.With(z: 1000f);
        }#1#

        [Event("player_jump")]
        public void OnPlayerJump(GameEvent args)
        {
            //var player = args.GetPlayer();
            //player?.PrintToChat("You just jumped!");
        }

        [UnmanagedCallersOnly]
        public static void Callback()
        {
            Console.WriteLine("Task should have run next frame..");
        }

        private double lastTickOnGround = 1;

        private ConVar playHeadshotSound = new ConVar("hs_sound", "0", "Should play headshots sound?",
            ConVarFlags.Replicated | ConVarFlags.Notify);

        public override void Load(bool hotReload)
        {
            base.Load(hotReload);

            new ConVar("kztimer_version", "1.95", "kztimer Version.", ConVarFlags.Replicated | ConVarFlags.Notify);

            AddCommand("toggle_viewmodel", "toggles vm", (client, args) =>
            {
                var player = Player.FromIndex(client);
                if (!player.IsAlive) return;

                var value = player.GetPropBool(PropType.Send, "m_bDrawViewmodel");
                player.SetPropBool(PropType.Send, "m_bDrawViewmodel", !value);
            });

            OnMapStart += (e) =>
            {
                Server.PrecacheModel("models/weapons/w_ied.mdl");
                Server.PrecacheSound("weapons/c4/c4_click.wav");
                Server.PrecacheSound("weapons/hegrenade/explode3.wav");
            };

            /*RegisterEventHandler("player_death", @event =>
            {
                var victimUserId = @event.GetInt("userid");
                var attackerUserid = @event.GetInt("attacker");

                //if (victimUserId == attackerUserid) return;

                var victim = Player.FromUserId(victimUserId);

                var entName = "suicide_bomb";
                var modelName = "models/weapons/w_ied.mdl";

                var ent = BaseEntity.Create("prop_physics_override");

                ent.SetKeyValueFloat("physdamagescale", 0.0f);
                ent.SetKeyValue("model", modelName);
                ent.SetKeyValue("targetname", entName);
                ent.Spawn();

                var eyeAngle = (Math.PI / 180) * victim.EyeAngle.Y;
                var destinationX = victim.AbsOrigin.X + Convert.ToSingle(50 * Math.Cos(eyeAngle));
                var destinationY = victim.AbsOrigin.Y + Convert.ToSingle(50 * Math.Sin(eyeAngle));
                var destinationZ = victim.AbsOrigin.Z;

                //var destination = new Vector(destinationX, destinationY, destinationZ);
                                
                /*var teleportEntity = VirtualFunction.Create<IntPtr, IntPtr, IntPtr, IntPtr, bool>(ent.Handle, 114);
                teleportEntity(ent.Handle, victim.AbsOrigin.Handle, IntPtr.Zero, IntPtr.Zero, false);
                #2#
                ent.AbsOrigin = victim.AbsOrigin;
                ent.Origin = victim.AbsOrigin;

                ent.MoveType = MoveType.MOVETYPE_VPHYSICS;

                Task.Run(() =>
                {
                    try
                    {
                        Task.Delay(5000).Wait();

                        ent.AcceptInput("InputKill");

                        var explosion = BaseEntity.Create("env_explosion");
                        if (explosion?.IsValid == true)
                        {
                            explosion.SetProp(PropType.Data, "m_spawnflags", 16384);
                            explosion.SetProp(PropType.Data, "m_iMagnitude", 200);
                            explosion.SetProp(PropType.Data, "m_iRadiusOverride", 800);

                            explosion.Spawn();

                            /*var activateEntity = VirtualFunction.Create<IntPtr>(explosion.Handle, 38);
                            activateEntity(explosion.Handle);#2#

                            var dest = new Vector(ent.Origin.X, ent.Origin.Y, ent.Origin.Z);

                            /*teleportEntity =
                                VirtualFunction.Create<IntPtr, IntPtr, IntPtr, IntPtr, bool>(explosion.Handle, 114);
                            teleportEntity(explosion.Handle, dest.Handle, IntPtr.Zero, IntPtr.Zero, false);#2#
                            /*
                            explosion.SetPropEnt(PropType.Send, "m_hOwnerEntity", victim.Index);
                            explosion.Team = victim.Team;
    
                        //ent.AcceptInput("InputKill");
    
                        //explosion.AcceptInput("InputExplode");
                        //explosion.AcceptInput("InputKill");
    
                        /*
                        for (int i = 1; i < 65; i++)
                        {
                            var client = Player.FromIndex(i);
                            if (client?.IsValid == true)
                            {
                                Sound.EmitSound(i, "weapons/hegrenade/explode3.wav", 0, origin: explosion.AbsOrigin, level: SoundLevel.SNDLVL_120dB);
                            }
                        }
    
                        ent.AcceptInput("InputKill");#2#
                        }
                    } catch (Exception e)
                    {
                        Console.WriteLine(e.Message);
                        Console.WriteLine(e.StackTrace);
                    }

                    // Detonate C4;
                });

                Task.Run(() =>
                {
                    while (ent.IsValid)
                    {
                        ent = BaseEntity.FromIndex(ent.Index);
                        Task.Delay(300).Wait();
                        Console.WriteLine("TICK : " + ent.Index);
                        var position = ent.AbsOrigin;

                        for (int i = 1; i < 65; i++)
                        {
                            var client = Player.FromIndex(i);
                            if (client != null)
                            {
                                Sound.EmitSound(i, "weapons/c4/c4_click.wav", 0, origin:position, level: SoundLevel.SNDLVL_90dB);
                            }
                        }
                    }
                });
            });#1#

            AddCommand("usp", "spawns usp", (client, args) =>
            {
                var player = Player.FromIndex(client);
                if (!player.IsAlive) return;

                var weapon = player.GetWeapon("weapon_hkp2000");
                if (weapon != null)
                {
                    Console.WriteLine("Cur time: " + Server.CurrentTime);
                    // Switch to weapon
                    player.SetPropEnt(PropType.Data, "m_hActiveWeapon", weapon.Index);
                    weapon.SetPropFloat(PropType.Data, "m_flNextPrimaryAttack", Server.CurrentTime);
                    player.SetPropFloat(PropType.Data, "m_flNextAttack", Server.CurrentTime);

                    var viewModel = player.GetPropEnt(PropType.Send, "m_hViewModel");
                    viewModel.SetProp(PropType.Send, "m_nSequence", 0);
                }
                else
                {
                    player.GiveWeapon("weapon_usp_silencer");
                    //NativeAPI.GiveNamedItem(player.Index, "weapon_hkp2000");
                    // Give weapon
                }

                //GivePlayerItem(player, "weapon_usp_silencer");
            });

            Console.WriteLine("Hello World! hotReload:" + hotReload);

            /*RegisterEventHandler("player_connect", Handler);
            DeregisterEventHandler("player_connect", Handler);

            AddCommand("fuckyou", "This is my command", (client, command) =>
            {
                if (command.ArgCount() > 1)
                {
                    Console.WriteLine($"Client: {client} just invoked command with argument: {command.ArgByIndex(1)}");
                }
            });#1#

            OnEntityCreated += (e) =>
            {
                Console.WriteLine($"Entity created: {e.EntityIndex}|{e.Classname}");
            };

            OnEntitySpawned += (e) =>
            {
                Console.WriteLine($"Entity spawned: {e.EntityIndex}|{e.Classname}");
            };

            OnEntityDeleted += (e) =>
            {
                Console.WriteLine($"Entity deleted: {e.EntityIndex}");
            };

            OnTick += (e) =>
            {
                try
                {
                    if (Server.TickCount % 16 == 0)
                    {
                        for (int i = 1; i < 65; i++)
                        {
                            var entity = BaseEntity.FromIndex(i);

                            var foundPlayer = Player.FromIndex(i);
                            if (foundPlayer == null || !foundPlayer.IsAlive) continue;

                            if (foundPlayer.AbsVelocity.Z > 100)
                            {
                                /*var blind = VirtualFunction.Create<IntPtr, float, float, float>(player.Handle, 547);
                                blind(player.Handle, 1f, 1f, 255f);#1#

                                // "55 8b ec 83 e4 f8 83 ec 64 53 8b 5d 08 56 57 8b f9 85 db 0f 84 0c"
                                var giveNamedItem = VirtualFunction.CreateFunc<IntPtr, string, int, IntPtr, bool, ulong, IntPtr>(foundPlayer.Handle, 457);
                                var wep = giveNamedItem(foundPlayer.Handle, "weapon_awp", 0, IntPtr.Zero, true, 0);
                                /*
                                var switchTeam = VirtualFunction.Create<IntPtr, int>(player.Handle,
                                    "55 8B EC 83 EC 10 53 56 8B 75 08 57 8B F9 85 F6");
                                switchTeam(player.Handle, player.Team == Team.TEAM_CT ? (int)Team.TEAM_TERRORIST : (int)Team.TEAM_CT);
                                #1#

                                /*
                                var deafen = VirtualFunction.Create<IntPtr>(player.Handle,
                                    "55 8B EC 83 EC 28 56 57 8B F9 F3 0F 11 4D FC");
                                deafen(player.Handle);#1#
                            }
                        }
                    }

                    var player = Player.FromIndex(1);
                    if (player != null)
                    {
                        // They are on the ground.
                        if (player.GroundEntity != null)
                        {
                            if (Server.EngineTime - lastTickOnGround > 0.1)
                            {
                                NativeAPI.PrintToChat(1,
                                    "Just completed jump of size: " + (Server.EngineTime - lastTickOnGround));
                            }

                            lastTickOnGround = Server.EngineTime;
                        }

                        if (Server.TickCount % 16 == 0)
                        {
                            if (!player.IsAlive)
                            {
                                player.PrintToHint(
                                    $"Observer Mode: {player.ObserverMode}, Target: {player.ObserverTarget?.Name}, Team: {player.ObserverTarget?.PlayerInfo?.Team}, Wep: {player.ObserverTarget?.ActiveWeapon?.ClassName}");
                                return;
                            }

                            /*if (player.Buttons.HasFlag(PlayerButtons.Jump))
                            {
                                /*var blind = VirtualFunction.Create<IntPtr, float, float, float>(player.Handle, 547);
                                blind(player.Handle, 1f, 1f, 255f);#2#

                                var giveNamedItem = VirtualFunction.CreateFunc<IntPtr, string, int, IntPtr, bool, ulong, IntPtr>(player.Handle,"55 8b ec 83 e4 f8 83 ec 64 53 8b 5d 08 56 57 8b f9 85 db 0f 84 0c");
                                var wep = giveNamedItem(player.Handle, "weapon_awp", 0, IntPtr.Zero, true, 0);
                                /*
                                var switchTeam = VirtualFunction.Create<IntPtr, int>(player.Handle,
                                    "55 8B EC 83 EC 10 53 56 8B 75 08 57 8B F9 85 F6");
                                switchTeam(player.Handle, player.Team == Team.TEAM_CT ? (int)Team.TEAM_TERRORIST : (int)Team.TEAM_CT);
                                #2#

                                /*
                                var deafen = VirtualFunction.Create<IntPtr>(player.Handle,
                                    "55 8B EC 83 EC 28 56 57 8B F9 F3 0F 11 4D FC");
                                deafen(player.Handle);#2#
                            }#1#


                            var lastWeapon = player.GetPropEnt(PropType.Data, "m_hLastWeapon");
                            var nextAttack = player.ActiveWeapon.GetPropFloat(PropType.Data, "m_flNextPrimaryAttack");
                            var isInBurstMode = player.ActiveWeapon.GetProp(PropType.Send, "m_iClip1");

                            player.PrintToHint(
                                $"Location: {player.AbsOrigin}\n" +
                                $"Speed: {player.AbsVelocity.Length().ToString("n2")}, Sqr: {player.Velocity.LengthSqr().ToString("n2")}\n" +
                                $"Time: {Server.CurrentTime.ToString("n2")}\n" +
                                $"Life State: {player.LifeState}\n" +
                                $"Buttons: {string.Join(", ", player.SelectedButtons)}\n" +
                                $"Flags: {string.Join(", ", player.Flags.FlagsToList())}\n" +
                                /*$"RenderMode: {player.RenderMode}\n" +
                                $"RenderFx: {player.RenderFx}\n" +
                                $"MoveType: {player.MoveType}\n" +#1#
                                //$"ViewOffset: {player.Entity.EyeLocation}, Origin: {player.Entity.GetKeyValueVector("origin")}\n" +
                                $"Name: {player.Name}\n" +
                                // $"EyeAngle: {player.EyeAngle}, ViewVec: {player.ViewVector}\n" +
                                //$"Life State: {player.LifeState}, Stamina: {player.GetPropFloat(PropType.Send, "cslocaldata.m_flStamina")}\n" +
                                $"Bomb Zone: {player.GetProp(PropType.Send, "m_bInBombZone")}\n" +
                                // $"Is Freeze Time: {_gamerules?.ClassName} {_gamerules?.GetProp(PropType.Send, "cs_gamerules_data.m_bBombPlanted")}\n" +
                                //$"Ground Entity: {player.GetPropEnt(PropType.Data, "m_hGroundEntity")}\n" +
                                $"Last Weapon: {lastWeapon.ClassName}, Active: {player.ActiveWeapon.ClassName} ({player.ActiveWeapon.Handle.ToString("X8")}, {player.ActiveWeapon.Index})\n" +
                                $"TickCount:{Server.TickCount}, FrameTime:{Server.GameFrameTime.ToString("n2")}, EngineTime:{Server.EngineTime.ToString("n2")}, CurTime: {Server.CurrentTime.ToString("n2")}\n" +
                                $"Time until attack: {Math.Clamp(nextAttack - Server.CurrentTime, 0, 1000).ToString("n2")}\n" +
                                $"IsMoving: {!player.Velocity.IsZero()}");
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.Message);
                    Console.WriteLine(ex.StackTrace);
                }
            };

            OnMapStart += (e) =>
            {
                var jsonDict = new Dictionary<string, string[]>();

                int numTables = StringTables.NumberOfTables;
                Console.WriteLine($"Number of tables: {numTables}");

                for (int i = 0; i < numTables; i++)
                {
                    var table = new StringTable(i);
                    var numTableStrings = table.NumStrings;

                    var list = new List<string>();

                    Console.WriteLine($"Table {table.Name}, size: {numTableStrings}");

                    for (int j = 0; j < numTableStrings; j++)
                    {
                        //Console.Write(table.Read(j) + " | ");
                        list.Add(table.Read(j));
                    }

                    jsonDict[table.Name] = list.ToArray();
                }

                var json = JsonConvert.SerializeObject(jsonDict);
                File.WriteAllText(@"C:\temp\dump.json", json);
            };


            RegisterEventHandler("bullet_impact", (@event) =>
            {
                var player = Player.FromUserId(@event.GetInt("userid"));

                /*var createdProp = BaseEntity.Create("prop_physics_multiplayer");
                var x = @event.GetFloat("x");
                var y = @event.GetFloat("y");
                var z = @event.GetFloat("z");#1#
                 
                /*Server.PrecacheModel("models/props_junk/watermelon01.mdl");
                createdProp.SetKeyValue("model", "models/props_junk/watermelon01.mdl");
                var random = new Random();
                createdProp.SetKeyValue("rendercolor", $"{random.Next(256)} {random.Next(256)} {random.Next(256)}");
                createdProp.AbsOrigin = new Vector(x, y, z +20);

                player.PrintToChat(
                    $"Just created prop of type {createdProp.ClassName} and index {createdProp.Index}, {createdProp.Origin}");

                createdProp.Spawn();#1#

            });
            RegisterEventHandler("player_connect", @event =>
            {
                var userid = @event.GetInt("userid");
                Console.WriteLine("Connect: " + userid);
                var player = Player.FromUserId(userid);

            });

            /*RegisterEventHandler("player_jump", @event =>
            {
                Console.WriteLine("@event ptr: " + @event.Handle);
                var index = NativeAPI.IndexFromUserid(@event.GetInt("userid"));
                if (index == 1)
                {
                    var entity = BaseEntity.FromIndex(1);

                    NativeAPI.PrintToChat(entity.Index, $"{entity.ClassName} | {entity.Health}/{entity.MaxHealth}");
                    entity.Health -= 1;

                    var groundEntity = entity.GroundEntity;

                    entity.SetProp(PropType.Send, "m_iAccount", entity.GetProp(PropType.Send, "m_iAccount") + 150);

                    Task.Run(() =>
                    {
                        Task.Delay(1);
                        var qAngle = new Vector();
                        // entity.AbsVelocity.X;// + Convert.ToSingle(new Random().NextDouble() * 250);
                        //entity.AbsVelocity.Y;// + Convert.ToSingle(new Random().NextDouble() * 250);
                        entity.AbsVelocity.Z += Convert.ToSingle(new Random().NextDouble() * 550);

                        //entity.AbsVelocity = qAngle;
                    });

                    /*var player = Player.FromIndex(entity.Index);
                    if (player != null)
                    {
                        var vfunc = NativeAPI.CreateVirtualFunction(player.Handle, 547, 4, (int)DataType.DATA_TYPE_VOID, new[] { (int)DataType.DATA_TYPE_POINTER, (int)DataType.DATA_TYPE_FLOAT, (int)DataType.DATA_TYPE_FLOAT, (int)DataType.DATA_TYPE_FLOAT }.Cast<object>().ToArray());

                        var funcArgs = new object[] { player.Handle, 0.5f, 0.5f, 255.0f };
                        NativeAPI.ExecuteVirtualFunction(vfunc, funcArgs.Cast<object>().ToArray());
                    }#2#
                }
            });#1#

            RegisterEventHandler("grenade_thrown", (@event) =>
            {
                var index = NativeAPI.IndexFromUserid(@event.GetInt("userid"));
                var playerWhoThrew = BaseEntity.FromIndex(index);

                /*Server.PrintToConsole(
                    $"Event fired: {@event.Name}, type of grenade: {@event.GetString("weapon")}, thrower:");#1#

                var hexValue = Encoding.UTF8.GetString(new byte[] {(byte) new Random().Next(3, 13)});

                NativeAPI.PrintToChat(index, $"You just threw a {hexValue}[{@event.GetString("weapon")}]\x10");
                //playerWhoThrew.PrintToHint($"You just threw a [{@event.GetString("weapon")}] {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToCenter($"You just threw a [{@event.GetqString("weapon")}] {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToCenter("#SFUI_Notice_YouDroppedWeapon");
                //playerWhoThrew.PrintToCenter("This is a <font color=\"#FF0000\">test</font>");
            });

            RegisterEventHandler("player_spawn", (@event) =>
            {
                var player = Player.FromUserId(@event.GetInt("userid"));
                Server.NextFrame(() =>
                {
                    player.Gravity = 0.5f;
                });

                player.RenderMode = RenderMode.RENDER_TRANSCOLOR;
                player.Color = Color.FromArgb(150, 255, 255, 255);

                Console.WriteLine(player.Color);
//                player.Speed = 2.0f;
            });

            RegisterEventHandler("player_hurt", (@event) =>
            {
                var victimIndex = NativeAPI.IndexFromUserid(@event.GetInt("userid"));
                var victim = BaseEntity.FromIndex(victimIndex);

                var attackerIndex = NativeAPI.IndexFromUserid(@event.GetInt("attacker"));
                var attacker = BaseEntity.FromIndex(attackerIndex);

                var hitgroup = (HitGroup) @event.GetInt("hitgroup");

                if (attacker == null) return;

                //Server.PrintToConsole($"Someone got hurt! Victim {victim.Name}, Attacker {attacker.Name}");
                NativeAPI.PrintToChat(attackerIndex, ($"Damage: \x02{@event.GetInt("dmg_health")}, \x04{hitgroup}"));
                attacker.Health += @event.GetInt("dmg_health") / 2;

                if (hitgroup == HitGroup.HITGROUP_HEAD)
                {
                    if (playHeadshotSound.BoolValue)
                    {
                        Sound.EmitSound(attackerIndex, "buttons/bell1.wav", origin: Player.FromIndex(1).AbsOrigin);
                    }
                    playHeadshotSound.BoolValue = !playHeadshotSound.BoolValue;
                }

                if (victim != null)
                {
                    NativeAPI.PrintToChat(attackerIndex, $"Health: {victim.Health}");

                    victim.MoveType = MoveType.MOVETYPE_NONE;
                    victim.Color = Color.FromArgb(255, 50, 50, 255);

                    Task.Run(() =>
                    {
                        Task.Delay(1000).Wait();

                        victim.MoveType = MoveType.MOVETYPE_ISOMETRIC;
                        victim.Color = Color.FromArgb(255, 255, 255, 255);
                    });

                    /*var qAngle = new QAngle();
                    qAngle.x = victim.Entity.AbsVelocity.x + Convert.ToSingle(new Random().NextDouble() * 250);
                    qAngle.y = victim.Entity.AbsVelocity.y + Convert.ToSingle(new Random().NextDouble() * 250);
                    qAngle.z = victim.Entity.AbsVelocity.z + 700f;

                    victim.Entity.AbsVelocity = qAngle;#1#
                }
            });

            /*OnClientConnected += (playerArgs) =>
            {
                Console.WriteLine($"Connected: Player Index: {playerArgs.Player.Index}, Ptr: {playerArgs.Player.Handle}");
            };

            OnClientPutInServer += (playerArgs) =>
            {
                Console.WriteLine($"Put in server: Player Index: {playerArgs.Player.Index}, Ptr: {playerArgs.Player.Handle}");
            };#1#

            /*#1#
            /*NativeAPI.AddListener("OnMapStart", new Action(() =>
            {
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
            }));#1#

            /*NativeAPI.AddListener("OnClientConnect", new Action<IntPtr>((IntPtr ptr) =>
            {
            }));#1#


            /*HookConVarChange(ConVar.Find("sv_cheats"), (convar, value, newValue) =>
            {
                Console.WriteLine($"sv_cheats changed from {value} to {newValue}");
            });#1#
            /*#1#

            /*API.HookEvent("player_jump", new Action<IntPtr>(i =>
            {
                int userid = API.GetEventInt(i, "userid");
                IntPtr baseEntity = API.BaseentityFromUserid(userid);
                IntPtr playerInfo = API.PlayerinfoFromBaseentity(baseEntity);
                API.PrintToChat(1, $"A player jumpeds... ({userid})");
            }));

            /*GameEvent.RegisterEventHandler("player_hurt", @event =>
            {
                int victim = @event.GetInt("userid");
                int attacker = @event.GetInt("attacker");
                string weaponName = @event.GetString("weapon");

                NativeAPI.PrintToChat(1, $"[{victim}] was shot by [{attacker}] with {weaponName} | {NativeAPI.GetEventName(@event.Handle)}");

            })#1#
            ;

            Task.Run(async () =>
            {
                for (int i = 0; i < 50; i++)
                {
                    Task.Delay(250).Wait();
                }
            });

            /*public override void Load(bool hotReload)
            {
                var menu = new RadioMenu("My Radio Menu!");
                menu.Handler += (player, option) =>
                {
                    OnSelectOption(player, option);
                };
    
                /*
                var menuOption = new RadioMenuOption($"First Option {Server.CurrentTime.ToString("n0")}", false, Server.CurrentTime);
                menu.AddMenuOption(menuOption);
                #3#
    
                /*for (int i = 1; i <= 25; i++)
                {
                    menu.AddMenuOption(new RadioMenuOption($"Option {i}", false, i));
                }#3#
    
                /*menu.AddMenuOption(new RadioMenuOption("Second Option", false));
                menu.AddMenuOption(new RadioMenuOption("Third Option", false, 3));#3#
                /*GameEvent.Subscribe("player_jump", (@event =>
                {
                    var player = Player.FromUserId(@event.GetInt("userid"));
                    menu.Display(player.Index);
                }));#3##1#

            var menu = new RadioMenu("My Menu!");

            menu.AddMenuOption($"AWP", $"weapon_awp");
            menu.AddMenuOption($"M4a1", $"weapon_m4a1");
            menu.AddMenuOption($"XM1014", $"weapon_xm1014");

            menu.Handler = (player, option) =>
            {
                player.GiveWeapon(option.Value);
            };

            AddCommand("ultimate", "option", (client, info) =>
            {
                var player = Player.FromIndex(client);

                menu.Display(player.Index);

                var start = player.EyeLocation;
                var end = player.EyeLocation + player.ViewVector * 10000;
                var ray = TraceEngine.CreateRay(RayType.EndPoint, start, end);

                var filter = new ExclusionTraceFilter(player.Index);

                TraceResult result = TraceEngine.TraceRay(ray, 4294967295, filter);

                if (result != null && result.DidHit() && result.Entity != null)
                {
                    if (result.Entity.Index > 0)
                    {
                        var foundPlayer = Player.FromIndex(result.Entity.Index);
                        if (foundPlayer != null)
                        {
                            player.PrintToChat($"Sending \x0C{foundPlayer.Name} \x01skyhigh!");// {(HitGroup)result.Hitgroup}:{result.Hitbox}");
                            foundPlayer.AbsVelocity = foundPlayer.AbsVelocity.With(z: 500f);
                        }
                        else
                        {
                            var scale = result.Entity.GetKeyValueFloat("modelscale");
                            result.Entity.SetKeyValueFloat("modelscale", scale * 1.1f);
                        }

                    }
                }
            });

            AddCommand("test", "testing", (client, info) =>
            {
                Server.PrintToConsole("Is Precached: " + Sound.IsSoundPrecached("buttons/bell1.wav"));
                Server.PrintToConsole("Tried to Precache: " + Sound.PrecacheSound("buttons/bell1.wav"));
                Server.PrintToConsole("Sound Duration: " + Sound.GetSoundDuration("buttons/bell1.wav"));
                Server.PrintToConsole("Is Precached: " + Sound.IsSoundPrecached("buttons/bell1.wav"));

                Sound.EmitSound(1, "buttons/bell1.wav", origin: Player.FromIndex(1).AbsOrigin);
            });
        }

        /*var createdProp = BaseEntity.Create("prop_physics_multiplayer");

        Server.PrecacheModel("models/props_c17/oildrum001.mdl");
        createdProp.SetKeyValue("model", "models/props_c17/oildrum001.mdl");
        createdProp.AbsOrigin = result.EndPosition;

        player.PrintToChat($"Just created prop of type {createdProp.ClassName} and index {createdProp.Index}, {createdProp.Origin}");

        createdProp.Spawn();#3#
    });


    ConsoleCommand.AddCommand("test", "", true, 0, (client, arguments) =>
    {
        Server.PrintToConsole("Is Precached: " + Sound.IsSoundPrecached("buttons/bell1.wav"));
        Server.PrintToConsole("Tried to Precache: " + Sound.PrecacheSound("buttons/bell1.wav"));
        Server.PrintToConsole("Sound Duration: " + Sound.GetSoundDuration("buttons/bell1.wav"));
        Server.PrintToConsole("Is Precached: " + Sound.IsSoundPrecached("buttons/bell1.wav"));
    });

    Listeners.OnTick += args =>
    {
        //menuOption.Text = $"First Option {Server.CurrentTime.ToString("n2")}";
        //menuOption.Value = Server.CurrentTime;
       // menu.Refresh();
    };

    //var sayCmd = ConsoleCommand.FindCommand("say");
    //var menuSelectCmd = ConsoleCommand.FindCommand("menuselect");

}
public override void Unload(bool hotReload)
{ 
    Server.PrintToConsole("Class 2 unloading");
}#2#
 

}#1#
        public override string ModuleName => "Class2";
        public override string ModuleVersion => "0.1";
    }*/
}

using CSGONET.API;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Players;
using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using CSGONET.API.Modules.Engine.Trace;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Sound;
using CSGONET.API.Modules.Utils;

namespace ClassLibrary2
{

    public interface IRadioMenuOption
    {
        string Text { get; set; }
        object Value { get; set; }
        bool Disabled { get; set; }
    }

    public class RadioMenuOption : IRadioMenuOption
    {
        public RadioMenuOption(string text, bool disabled, object value = null)
        {
            Text = text;
            Disabled = disabled;
            Value = value ?? text;
        }

        public string Text { get; set; }
        public bool Disabled { get; set; }
        public object Value { get; set; }
    }

    public interface IRadioMenu
    {
        string Title { get; set; }
        void AddMenuOption(IRadioMenuOption option);
        void Refresh();
        void Display(int client);
        void Close();
        Action<Player, IRadioMenuOption> Handler { get; set; }
    }

    /*public class RadioMenu : IRadioMenu
    {
        private List<IRadioMenuOption> _options = new List<IRadioMenuOption>();
        private int _keys = 0;

        private int _page = 0;
        private int _currentOffset = 0;
        private int _numDisplayed = 0;
        private int _numPerPage = 6;

        private Stack<int> _prevPageOffsets = new Stack<int>();
        
        int NumPerPage() => _numPerPage + 2 - (RequiresNextPage()  ? 1 : 0) - (RequiresPrevPage() ? 1 : 0);

        bool RequiresPrevPage() => _page > 0;
        bool RequiresNextPage() => (_currentOffset + _numPerPage) < _options.Count;


        private ConcurrentDictionary<int, byte> _openMenus = new ConcurrentDictionary<int, byte>();

        public string Title { get; set; }

        public RadioMenu(string title)
        {
            Title = title;

            /*ConsoleCommand.AddCommand("menuselect", "", false, 0, (client, arguments) =>
            {
                var selectedValue = Convert.ToInt32(arguments.FirstOrDefault());

                if (selectedValue == 8)
                {
                    if (RequiresNextPage())
                    {
                        NextPage();
                    }

                    return;
                }

                if (RequiresPrevPage() && selectedValue == 1)
                {
                    PrevPage();
                    return;
                }


                _openMenus.TryRemove(client, out var _);

                var offset = _currentOffset;

                if (RequiresPrevPage()) selectedValue = selectedValue - 1;

                var option = _options[offset + selectedValue - 1];

                Handler?.Invoke(Player.FromIndex(client), option);
            });#1#
        }

        private void PrevPage()
        {
            _page--;
            _currentOffset = _prevPageOffsets.Pop();

            Refresh();
        }

        private void NextPage()
        {
            _prevPageOffsets.Push(_currentOffset);
            _currentOffset = _currentOffset + NumPerPage();
            _page++;


            Refresh();
        }

        public void Clear()
        {
            _page = 0;
            _currentOffset = 0;
            _options.Clear();
        }

        public void AddMenuOption(IRadioMenuOption option)
        {
            //if (_options.Count >= 10) throw new Exception("Menus can only have at most 10 items.");

            _options.Add(option);
        }

        public void Refresh()
        {
            foreach (var player in _openMenus)
            {
                Player.FromIndex(player.Key).ShowMenu(GetMessage(), GetKeys(), 5);
            }
        }

        public void Display(int client)
        {
            var player = Player.FromIndex(client);
            if (player == null) return;

            if (!_openMenus.ContainsKey(client))
            {
                _openMenus[client] = 1;
            }

            _page = 0;
            _currentOffset = 0;


            player.ShowMenu(GetMessage(), GetKeys(), 5);
        }

        public void Close()
        {
            // TODO: How do I close it?
        }

        private string GetMessage()
        {
            var sb = new StringBuilder();

            Title = $"Page: {_page}, Offset: {_currentOffset}";

            if (!string.IsNullOrEmpty(Title))
            {
                sb.Append($"{Title}\n \n");
            }

            int keyOffset = 1;

            if (RequiresPrevPage())
            {
                sb.Append($"->1. Prev\n");
                _keys |= (1 << 1 - 1);
                keyOffset++;
            }


            _numDisplayed = 0;
            for (int i = _currentOffset; i < Math.Min(_currentOffset + NumPerPage(), _options.Count); i++)
            {
                var option = _options[i];
                sb.Append($"{(!option.Disabled ? "->" : "")}{keyOffset++}. {option.Text}\n");
                _keys |= (1 << (keyOffset - 2));

                _numDisplayed++;
            }

            if (RequiresNextPage())
            {
                sb.Append($"->8. Next\n");
                _keys |= (1 << 8 - 1);
            }

            sb.Append($"->9. Close\n");
            _keys |= (1 << 9 - 1);

            return sb.ToString();
        }

        private int GetKeys()
        {
            return _keys;
        }

        public Action<Player, IRadioMenuOption> Handler { get; set; }
    }*/



    public class Class1 : BasePlugin
    {
        /*private void OnSelectOption(Player player, IRadioMenuOption option)
        {
            player.PrintToChat($"You selected option:\x0B {option.Text}:{option.Value}");
            var targetPlayer = Player.FromIndex(Convert.ToInt32(option.Value));

            targetPlayer.AbsVelocity = player.AbsVelocity.With(z: 1000f);
        }*/

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

        public override void Load(bool hotReload)
        {
            base.Load(hotReload);

            Console.WriteLine("Hello World! hotReload:" + hotReload);

            /*RegisterEventHandler("player_connect", Handler);
            DeregisterEventHandler("player_connect", Handler);

            AddCommand("fuckyou", "This is my command", (client, command) =>
            {
                if (command.ArgCount() > 1)
                {
                    Console.WriteLine($"Client: {client} just invoked command with argument: {command.ArgByIndex(1)}");
                }
            });*/

            OnTick += (e) =>
            {
                try
                {
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
                                $"RenderMode: {player.RenderMode}\n" +
                                $"RenderFx: {player.RenderFx}\n" +
                                $"MoveType: {player.MoveType}\n" +
                                //$"ViewOffset: {player.Entity.EyeLocation}, Origin: {player.Entity.GetKeyValueVector("origin")}\n" +
                                $"Name: {player.Name}\n" +
                                // $"EyeAngle: {player.EyeAngle}, ViewVec: {player.ViewVector}\n" +
                                //$"Life State: {player.LifeState}, Stamina: {player.GetPropFloat(PropType.Send, "cslocaldata.m_flStamina")}\n" +
                                $"Bomb Zone: {player.GetProp(PropType.Send, "m_bInBombZone")}\n" +
                                // $"Is Freeze Time: {_gamerules?.ClassName} {_gamerules?.GetProp(PropType.Send, "cs_gamerules_data.m_bBombPlanted")}\n" +
                                //$"Ground Entity: {player.GetPropEnt(PropType.Data, "m_hGroundEntity")}\n" +
                                $"Last Weapon: {lastWeapon.ClassName}, Active: {player.ActiveWeapon.ClassName} ({player.ActiveWeapon.Handle.ToString("X8")})\n" +
                                $"TickCount:{Server.TickCount}, FrameTime:{Server.GameFrameTime.ToString("n2")}, EngineTime:{Server.EngineTime.ToString("n2")}, CurTime: {Server.CurrentTime.ToString("n2")}\n" +
                                $"Time until attack: {Math.Clamp(nextAttack - Server.CurrentTime, 0, 1000).ToString("n2")}\n" +
                                $"IsMoving: {!player.Velocity.IsZero()}");
                        }
                    }
                }
                catch (Exception ex)
                {
                    Console.WriteLine(ex.StackTrace);
                }
            };

            OnMapStart += (e) =>
            {
                Console.WriteLine("map started: " + e.MapName);
                Console.WriteLine("map started: " + e.MapName);
                Console.WriteLine("map started: " + e.MapName);
                Console.WriteLine("map started: " + e.MapName);
                Console.WriteLine("map started: " + e.MapName);
            };


            RegisterEventHandler("player_say", (@event) =>
            {
                var player = Player.FromUserId(@event.GetInt("userid"));
                var createdProp = BaseEntity.Create("prop_physics_multiplayer");


                Server.PrecacheModel("models/props_c17/oildrum001.mdl");
                createdProp.SetKeyValue("model", "models/props_c17/oildrum001.mdl");
                createdProp.AbsOrigin = new Vector(player.AbsOrigin.X, player.AbsOrigin.Y, player.AbsOrigin.Z + 100);
                createdProp.AbsVelocity = new Vector(z: 300f);

                player.PrintToChat(
                    $"Just created prop of type {createdProp.ClassName} and index {createdProp.Index}, {createdProp.Origin}");

                createdProp.Spawn();

            });
            RegisterEventHandler("player_connect", @event =>
            {
                var userid = @event.GetInt("userid");
                Console.WriteLine("Connect: " + userid);
            });

            RegisterEventHandler("player_jump", @event =>
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
                }
            });

            RegisterEventHandler("grenade_thrown", (@event) =>
            {
                var index = NativeAPI.IndexFromUserid(@event.GetInt("userid"));
                var playerWhoThrew = BaseEntity.FromIndex(index);

                /*Server.PrintToConsole(
                    $"Event fired: {@event.Name}, type of grenade: {@event.GetString("weapon")}, thrower:");*/

                var hexValue = Encoding.UTF8.GetString(new byte[] {(byte) new Random().Next(3, 13)});

                NativeAPI.PrintToChat(index, $"You just threw a {hexValue}[{@event.GetString("weapon")}]\x10");
                //playerWhoThrew.PrintToHint($"You just threw a [{@event.GetString("weapon")}] {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToCenter($"You just threw a [{@event.GetqString("weapon")}] {playerWhoThrew.Name}");
                //playerWhoThrew.PrintToCenter("#SFUI_Notice_YouDroppedWeapon");
                //playerWhoThrew.PrintToCenter("This is a <font color=\"#FF0000\">test</font>");
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
                    Sound.EmitSound(attackerIndex, "buttons/bell1.wav", origin: Player.FromIndex(1).AbsOrigin);
                }

                if (victim != null)
                {
                    NativeAPI.PrintToChat(attackerIndex, $"Health: {victim.Health}");

                    /*var qAngle = new QAngle();
                    qAngle.x = victim.Entity.AbsVelocity.x + Convert.ToSingle(new Random().NextDouble() * 250);
                    qAngle.y = victim.Entity.AbsVelocity.y + Convert.ToSingle(new Random().NextDouble() * 250);
                    qAngle.z = victim.Entity.AbsVelocity.z + 700f;

                    victim.Entity.AbsVelocity = qAngle;*/
                }
            });

            /*OnClientConnected += (playerArgs) =>
            {
                Console.WriteLine($"Connected: Player Index: {playerArgs.Player.Index}, Ptr: {playerArgs.Player.Handle}");
            };

            OnClientPutInServer += (playerArgs) =>
            {
                Console.WriteLine($"Put in server: Player Index: {playerArgs.Player.Index}, Ptr: {playerArgs.Player.Handle}");
            };*/

            /**/
            /*NativeAPI.AddListener("OnMapStart", new Action(() =>
            {
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
                Console.WriteLine("On Map Start!");
            }));*/

            /*NativeAPI.AddListener("OnClientConnect", new Action<IntPtr>((IntPtr ptr) =>
            {
            }));*/


            /*HookConVarChange(ConVar.Find("sv_cheats"), (convar, value, newValue) =>
            {
                Console.WriteLine($"sv_cheats changed from {value} to {newValue}");
            });*/
            /**/

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

            })*/
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
                #2#
    
                /*for (int i = 1; i <= 25; i++)
                {
                    menu.AddMenuOption(new RadioMenuOption($"Option {i}", false, i));
                }#2#
    
                /*menu.AddMenuOption(new RadioMenuOption("Second Option", false));
                menu.AddMenuOption(new RadioMenuOption("Third Option", false, 3));#2#
                /*GameEvent.Subscribe("player_jump", (@event =>
                {
                    var player = Player.FromUserId(@event.GetInt("userid"));
                    menu.Display(player.Index);
                }));#2#*/

            AddCommand("ultimate", "option", (client, info) =>
            {
                var player = Player.FromIndex(client);

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

        createdProp.Spawn();#2#
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
}#1#
 

}*/
        public override string ModuleName => "Class2";
        public override string ModuleVersion => "0.1";
    }
}

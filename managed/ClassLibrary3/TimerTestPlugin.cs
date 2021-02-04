using System;
using System.Linq;
using CSGONET.API;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Entities.Constants;
using CSGONET.API.Modules.Events;
using CSGONET.API.Modules.Memory;
using CSGONET.API.Modules.Memory.Constants;
using CSGONET.API.Modules.Players;
using CSGONET.API.Modules.Timers;

namespace ClassLibrary3
{
    public class TimerTestPlugin : BasePlugin
    {
        private Timer _timer;
        public override string ModuleName => "F";
        public override string ModuleVersion => "F";

        public override void Load(bool hotReload)
        {
            base.Load(hotReload);

            OnClientPutInServer += (e) =>
            {
                var playerHandle = e.Player.Handle;


                /*var vFunc = NativeAPI.CreateVirtualFunction(playerHandle, 288, 3, (int) DataType.DATA_TYPE_VOID,
                    new[] {DataType.DATA_TYPE_POINTER, DataType.DATA_TYPE_POINTER, DataType.DATA_TYPE_INT}
                        .Cast<object>().ToArray());*/

                
                
                var giveNamedItem = NativeAPI.CreateVirtualFunction(playerHandle, 457, 6, (int)DataType.DATA_TYPE_POINTER,
                    new[] {DataType.DATA_TYPE_POINTER, DataType.DATA_TYPE_STRING, DataType.DATA_TYPE_INT, DataType.DATA_TYPE_POINTER, DataType.DATA_TYPE_BOOL, DataType.DATA_TYPE_ULONG}.Cast<object>().ToArray());

                if (e.Player.Index == 1)
                {
                    /*NativeAPI.HookFunction(vFunc, e.Player.Index, false, new Action<IntPtr, IntPtr>((ptr, weaponPtr) =>
                    {
                        
                    }));*/

                    var switchWeapon = VirtualFunction.CreateObject<Player, BaseEntity, int>(playerHandle, 288);
                    switchWeapon.Hook(e.Player.Index, false, Callback);

                    switchWeapon.Hook(e.Player.Index, true, Action);

                    NativeAPI.HookFunction(giveNamedItem, e.Player.Index, false, new Action<IntPtr>(ptr =>
                    {
                        if (ptr != IntPtr.Zero)
                        {
                            var player = Player.FromIndex(NativeAPI.IndexFromBaseentity(ptr));
                            Console.WriteLine("Give Named Item drops " + player?.Name);
                        }
                    }));

                    /*
                    var touch = VirtualFunction.CreateObject<Player, BaseEntity>(e.Player.Handle, 102);
                    touch.Hook(e.Player.Index, false, (player, i) =>
                    {
                        Console.WriteLine($"Player {player?.Name} is touching {i?.ClassName}");
                    });
                    */

                    var triggerTeleport = BaseEntity.FindByClassname(1, "trigger_teleport");
                    if (triggerTeleport != null)
                    {
                        var touch = VirtualFunction.CreateObject<BaseEntity, BaseEntity>(triggerTeleport.Handle, 102);
                        touch.Hook(triggerTeleport.Index, false, (player, i) =>
                        {
                            Console.WriteLine($"Player {player?.ClassName} is touching {i?.ClassName}");
                        });
                    }

                    /*var deafen = VirtualFunction.CreateObject<Player>(e.Player.Handle,
                        "55 89 E5 83 EC 48 89 7D FC 8B 7D 08 89 5D F4 89 75 F8 8B 07");
                    deafen.Hook(e.Player.Index, true, (player) =>
                    {
                        Console.WriteLine($"Player deafened {player?.Name}");
                    });*/
                    /*var switchTeam = VirtualFunction.CreateObject<Player, int>(e.Player.Handle,
                        "55 8B EC 83 EC 10 53 56 8B 75 08 57 8B F9 85 F6");
                    switchTeam.Hook(e.Player.Index, true, (player, i) =>
                    {
                        Console.WriteLine($"Player {player?.Name} switched to team {(Team)i}");
                    });*/
                }

                /*NativeAPI.HookFunction(vFunc, e.Player.Index, false, new Action<IntPtr, IntPtr>((ptr, weaponPtr) =>
                {
                    if (ptr != IntPtr.Zero)
                    {
                        var player = Player.FromIndex(NativeAPI.IndexFromBaseentity(ptr));
                        var weapon = new BaseEntity(weaponPtr);
                        Console.WriteLine("Weapon drop alt " + player?.Name + " | " + weapon?.ClassName);
                    }
                }));

                NativeAPI.HookFunction(vFunc, e.Player.Index, true, new Action<IntPtr, IntPtr>((ptr, weaponPtr) =>
                {
                    if (ptr != IntPtr.Zero)
                    {
                        var player = Player.FromIndex(NativeAPI.IndexFromBaseentity(ptr));
                        var weapon = new BaseEntity(weaponPtr);
                        Console.WriteLine("Weapon drop post " + player?.Name + " | " + weapon?.ClassName);
                    }
                }));*/
            };
        }

        private void Action(Player player, BaseEntity weapon, int viewModelIndex)
        {
            Console.WriteLine($"Player {player?.Name} has switched to weapon {weapon?.ClassName}");
        }

        private void Callback(Player player, BaseEntity weapon, int viewModelIndex)
        {
            Console.WriteLine($"Player {player?.Name} is switching to weapon {weapon?.ClassName} from {player?.ActiveWeapon?.ClassName}");
        }

        public override void Unload(bool hotReload)
        {
            base.Unload(hotReload);
        }
    }
}

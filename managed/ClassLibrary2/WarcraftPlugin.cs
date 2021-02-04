using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using ClassLibrary2.Cooldowns;
using ClassLibrary2.Effects;
using ClassLibrary2.Races;
using CSGONET.API;
using CSGONET.API.Core;
using CSGONET.API.Modules.Commands;
using CSGONET.API.Modules.Cvars;
using CSGONET.API.Modules.Listeners;
using CSGONET.API.Modules.Memory;
using CSGONET.API.Modules.Menus;
using CSGONET.API.Modules.Players;
using CSGONET.API.Modules.Sound;
using CSGONET.API.Modules.Sound.Constants;
using CSGONET.API.Modules.Timers;
using CSGONET.API.Modules.Utils;
using Microsoft.Data.Sqlite;
using Dapper;

namespace ClassLibrary2
{
    public static class WarcraftPlayerExtensions
    {
        public static WarcraftPlayer GetWarcraftPlayer(this Player player)
        {
            return WarcraftPlugin.Instance.GetWcPlayer(player.Index);
        }

        public static void CommitSuicide(this Player player, bool bool1, bool bool2)
        {
            if (!player.IsAlive) return;

            var commitSuicide = VirtualFunction.Create<IntPtr, bool, bool>(player.Handle, 507);
            commitSuicide(player.Handle, bool1, bool2);
        }
    }

    public class XpSystem
    {
        private readonly WarcraftPlugin _plugin;

        public XpSystem(WarcraftPlugin plugin)
        {
            _plugin = plugin;
        }

        private List<int> _levelXpRequirement = new List<int>(new int[256]);

        public void GenerateXpCurve(int initial, float modifier, int maxLevel)
        {
            for (int i = 1; i <= maxLevel; i++)
            {
                if (i == 1)
                    _levelXpRequirement[i] = initial;
                else
                    _levelXpRequirement[i] = Convert.ToInt32(_levelXpRequirement[i - 1] * modifier);
            }
        }

        public int GetXpForLevel(int level)
        {
            return _levelXpRequirement[level];
        }

        public void AddXp(Player attacker, int xpToAdd)
        {
            var wcPlayer = _plugin.GetWcPlayer(attacker.Index);
            if (wcPlayer == null) return;

            if (wcPlayer.GetLevel() >= WarcraftPlugin.MaxLevel) return;

            wcPlayer.currentXp += xpToAdd;

            while (wcPlayer.currentXp >= wcPlayer.amountToLevel)
            {
                wcPlayer.currentXp = wcPlayer.currentXp - wcPlayer.amountToLevel;
                GrantLevel(wcPlayer);

                if (wcPlayer.GetLevel() >= WarcraftPlugin.MaxLevel) return;
            }
        }

        public void GrantLevel(WarcraftPlayer wcPlayer)
        {
            if (wcPlayer.GetLevel() >= WarcraftPlugin.MaxLevel) return;

            wcPlayer.currentLevel += 1;

            RecalculateXpForLevel(wcPlayer);
            PerformLevelupEvents(wcPlayer);
        }

        private void PerformLevelupEvents(WarcraftPlayer wcPlayer)
        {
            if (GetFreeSkillPoints(wcPlayer) > 0)
                WarcraftPlugin.Instance.ShowSkillPointMenu(wcPlayer);

            var soundToPlay = "warcraft/ui/questcompleted.mp3";
            if (wcPlayer.currentLevel == WarcraftPlugin.MaxLevel)
                soundToPlay = "warcraft/ui/gamefound.mp3";


            Sound.EmitSound(wcPlayer.Index, soundToPlay);
        }

        public void RecalculateXpForLevel(WarcraftPlayer wcPlayer)
        {
            if (wcPlayer.currentLevel == WarcraftPlugin.MaxLevel)
            {
                wcPlayer.amountToLevel = 0;
                return;
            }

            wcPlayer.amountToLevel = GetXpForLevel(wcPlayer.currentLevel);
        }

        public int GetFreeSkillPoints(WarcraftPlayer wcPlayer)
        {
            int totalPointsUsed = 0;

            for (int i = 0; i < 4; i++)
            {
                totalPointsUsed += wcPlayer.GetAbilityLevel(i);
            }

            int level = wcPlayer.GetLevel();
            if (level > WarcraftPlugin.MaxLevel)
                level = WarcraftPlugin.MaxSkillLevel;

            return level - totalPointsUsed;
        }
    }

    public class WarcraftPlayer
    {
        private int _playerIndex;
        public int Index => _playerIndex;
        public bool IsMaxLevel => currentLevel == WarcraftPlugin.MaxLevel;
        public Player GetPlayer() => Player.FromIndex(_playerIndex);

        public int currentXp;
        public int currentLevel;
        public int amountToLevel;
        public string raceName;
        public string statusMessage;

        private List<int> _abilityLevels = new List<int>(new int[4]);
        public List<float> AbilityCooldowns = new List<float>(new float[4]);

        private WarcraftRace _race;

        public WarcraftPlayer(int playerIndex)
        {
            _playerIndex = playerIndex;
        }

        public void LoadFromDatabase(DatabaseRaceInformation dbRace, XpSystem xpSystem)
        {
            currentLevel = dbRace.CurrentLevel;
            currentXp = dbRace.CurrentXp;
            raceName = dbRace.RaceName;
            amountToLevel = xpSystem.GetXpForLevel(currentLevel);

            _abilityLevels[0] = dbRace.Ability1Level;
            _abilityLevels[1] = dbRace.Ability2Level;
            _abilityLevels[2] = dbRace.Ability3Level;
            _abilityLevels[3] = dbRace.Ability4Level;

            _race = WarcraftPlugin.Instance.raceManager.InstantiateRace(raceName);
            _race.WarcraftPlayer = this;
            _race.Player = Player.FromIndex(_playerIndex);
        }

        public int GetLevel()
        {
            if (currentLevel > WarcraftPlugin.MaxLevel) return WarcraftPlugin.MaxLevel;

            return currentLevel;
        }

        public override string ToString()
        {
            return
                $"[{_playerIndex}]: {{raceName={raceName}, currentLevel={currentLevel}, currentXp={currentXp}, amountToLevel={amountToLevel}}}";
        }

        public int GetAbilityLevel(int abilityIndex)
        {
            return _abilityLevels[abilityIndex];
        }

        public void SetAbilityLevel(int abilityIndex, int value)
        {
            _abilityLevels[abilityIndex] = value;
        }

        public WarcraftRace GetRace()
        {
            return _race;
        }

        public void SetStatusMessage(string status, float duration = 2f)
        {
            statusMessage = status;
            new Timer(duration, () => statusMessage = null, 0);
            GetPlayer().PrintToChat(" " + status);
        }

        public void GrantAbilityLevel(int abilityIndex)
        {
            _abilityLevels[abilityIndex] += 1;
        }
    }

    public class WarcraftPlugin : BasePlugin
    {
        private static WarcraftPlugin _instance;
        public static WarcraftPlugin Instance => _instance;

        public override string ModuleName => "Warcraft";
        public override string ModuleVersion => "0.0.1";

        public static int MaxLevel = 16;
        public static int MaxSkillLevel = 5;
        public static int maxUltimateLevel = 5;

        private List<WarcraftPlayer> WarcraftPlayers = new(new WarcraftPlayer[65]);
        private EventSystem _eventSystem;
        public XpSystem XpSystem;
        public RaceManager raceManager;
        public EffectManager EffectManager;
        public CooldownManager CooldownManager;
        private Database _database;

        public ConVar XpKnifeModifier { get; set; }
        public ConVar XpHeadshotModifier { get; set; }
        public ConVar XpPerKill { get; set; }
        public List< WarcraftPlayer> Players => WarcraftPlayers;

        public WarcraftPlayer GetWcPlayer(int index)
        {
            var wcPlayer = WarcraftPlayers[index];
            if (wcPlayer == null)
            {
                var player = Player.FromIndex(index);
                if (player != null && !player.IsFakeClient)
                {
                    WarcraftPlayers[index] = _database.LoadClientFromDatabase(Player.FromIndex(index), XpSystem);
                }
                else
                {
                    return null;
                }
            }

            return WarcraftPlayers[index];
        }

        public void SetWcPlayer(int index, WarcraftPlayer wcPlayer)
        {
            WarcraftPlayers[index] = wcPlayer;
        }

        public override void Load(bool hotReload)
        {
            base.Load(hotReload);

            if (_instance == null) _instance = this;

            XpSystem = new XpSystem(this);
            XpSystem.GenerateXpCurve(110, 1.07f, MaxLevel);

            _database = new Database();
            raceManager = new RaceManager();
            raceManager.Initialize();

            EffectManager = new EffectManager();
            EffectManager.Initialize();

            CooldownManager = new CooldownManager();
            CooldownManager.Initialize();

            AddCommand("ability1", "ability1", Ability1Pressed);
            AddCommand("ability2", "ability2", Ability2Pressed);
            AddCommand("ability3", "ability3", Ability3Pressed);
            AddCommand("ultimate", "ultimate", Ability4Pressed);

            AddCommand("changerace", "changerace", CommandChangeRace);
            AddCommand("raceinfo", "raceinfo", CommandRaceInfo);
            AddCommand("resetskills", "resetskills", CommandResetSkills);
            AddCommand("addxp", "addxp", CommandAddXp);
            AddCommand("skills", "skills", (client, _) => ShowSkillPointMenu(GetWcPlayer(client)));

            XpPerKill = new ConVar("wcgo_xp_kill", "20", "Base amount of xp granted for a kill",
                ConVarFlags.None);
            XpHeadshotModifier = new ConVar("wcgo_xp_headshot", "0.25", "% bonus xp granted for headshot",
                ConVarFlags.None);
            XpKnifeModifier =
                new ConVar("wcgo_xp_knife", "0.50", "% bonus xp granted for knife", ConVarFlags.None);

            OnClientPutInServer += OnClientPutInServerHandler;
            OnMapStart += OnMapStartHandler;
            OnClientDisconnect += OnClientDisconnectHandler;

            if (hotReload)
            {
                OnMapStartHandler(null);
            }

            _eventSystem = new EventSystem(this);
            _eventSystem.Initialize();

            _database.Initialize();
        }

        private void CommandAddXp(int client, CommandInfo commandinfo)
        {
            var wcPlayer = GetWcPlayer(client);

            if (string.IsNullOrEmpty(commandinfo.ArgByIndex(1))) return;

            var xpToAdd = Convert.ToInt32(commandinfo.ArgByIndex(1));

            XpSystem.AddXp(Player.FromIndex(client), xpToAdd);
        }

        private void CommandResetSkills(int client, CommandInfo commandinfo)
        {
            var wcPlayer = GetWcPlayer(client);

            for (int i = 0; i < 4; i++)
            {
                wcPlayer.SetAbilityLevel(i, 0);
            }

            if (XpSystem.GetFreeSkillPoints(wcPlayer) > 0)
            {
                ShowSkillPointMenu(wcPlayer);
            }
        }

        private void OnClientDisconnectHandler(Listeners.PlayerArgs e)
        {
            if (e.Player.IsFakeClient) return;

            e.Player.GetWarcraftPlayer().GetRace().PlayerChangingToAnotherRace();
            SetWcPlayer(e.Player.Index, null);
            _database.SaveClientToDatabase(e.Player);
        }

        private void OnMapStartHandler(Listeners.MapStartArgs e)
        {
            AddTimer(0.25f, StatusUpdate, TimerFlags.REPEAT | TimerFlags.STOP_ON_MAPCHANGE);
            AddTimer(60.0f, _database.SaveClients, TimerFlags.REPEAT | TimerFlags.STOP_ON_MAPCHANGE);

            StringTables.AddFileToDownloadsTable("sound/warcraft/ui/questcompleted.mp3");
            StringTables.AddFileToDownloadsTable("sound/warcraft/ui/gamefound.mp3");

            Server.PrecacheSound("warcraft/ui/questcompleted.mp3");
            Server.PrecacheSound("warcraft/ui/gamefound.mp3");

            Server.PrecacheModel("models/weapons/w_ied.mdl");
            Server.PrecacheSound("weapons/c4/c4_click.wav");
            Server.PrecacheSound("weapons/hegrenade/explode3.wav");
            Server.PrecacheSound("items/battery_pickup.wav");
        }

        private void StatusUpdate()
        {
            for (int i = 1; i < 65; i++)
            {
                var player = Player.FromIndex(i);
                if (player == null || !player.IsValid || !player.IsAlive) continue;

                var wcPlayer = GetWcPlayer(i);

                if (wcPlayer == null) continue;

                var message = $"{wcPlayer.GetRace().DisplayName} ({wcPlayer.currentLevel})\n" +
                              (wcPlayer.IsMaxLevel ? "" : $"Experience: {wcPlayer.currentXp}/{wcPlayer.amountToLevel}\n") +
                              $"{wcPlayer.statusMessage}";

                player.PrintToHint(message);
            }
        }

        private void OnClientPutInServerHandler(Listeners.PlayerArgs e)
        {
            // No bots, invalid clients or non-existent clients.
            if (!e.Player.IsValid || e.Player.Index == 0) return;

            // TODO: Hook friendly fire damage.
            // SDK_OnTakeDamage

            if (e.Player.IsFakeClient) return;

            if (!_database.ClientExistsInDatabase(e.Player.PlayerInfo.SteamId))
            {
                _database.AddNewClientToDatabase(e.Player);
            }

            WarcraftPlayers[e.Player.Index] = _database.LoadClientFromDatabase(e.Player, XpSystem);

            Console.WriteLine("Player just connected: " + WarcraftPlayers[e.Player.Index]);
        }

        private void CommandRaceInfo(int client, CommandInfo commandinfo)
        {

            var menu = new RadioMenu("Race Information");
            var races = raceManager.GetAllRaces();
            foreach (var race in races.OrderBy(x => x.DisplayName))
            {
                menu.AddMenuOption(race.DisplayName, race.InternalName);
            }

            menu.Handler = RaceInfoHandler;
            menu.Display(client);
        }

        private void RaceInfoHandler(Player player, IRadioMenuOption menuOption)
        {
            var race = raceManager.GetRace(menuOption.Value);

            player.PrintToChat("--------");
            for (int i = 0; i < 4; i++)
            {
                var ability = race.GetAbility(i);
                char color = i == 3 ? ChatColors.Gold : ChatColors.Purple;

                player.PrintToChat($" {color}{ability.DisplayName}{ChatColors.Default}: {ability.GetDescription(0)}");
            }
            player.PrintToChat("--------");
        }

        private void CommandChangeRace(int client, CommandInfo commandinfo)
        {
            var menu = new RadioMenu("Change Race");
            var races = raceManager.GetAllRaces();
            foreach (var race in races.OrderBy(x => x.DisplayName))
            {
                menu.AddMenuOption(race.DisplayName, race.InternalName);
            }

            menu.Handler = ChangeRaceHandler;
            menu.Display(client);
        }

        private void ChangeRaceHandler(Player player, IRadioMenuOption menuOption)
        {
            _database.SaveClientToDatabase(player);

            // Dont do anything if were already that race.
            if (menuOption.Value == player.GetWarcraftPlayer().raceName) return;

            player.GetWarcraftPlayer().GetRace().PlayerChangingToAnotherRace();
            player.GetWarcraftPlayer().raceName = menuOption.Value;

            _database.SaveCurrentRace(player);
            _database.LoadClientFromDatabase(player, XpSystem);

            player.GetWarcraftPlayer().GetRace().PlayerChangingToRace();

            player.CommitSuicide(false, false);
        }

        private void Ability1Pressed(int client, CommandInfo commandinfo)
        {
            GetWcPlayer(client)?.GetRace()?.InvokeAbility(0);
        }

        private void Ability2Pressed(int client, CommandInfo commandinfo)
        {
            GetWcPlayer(client)?.GetRace()?.InvokeAbility(1);
        }

        private void Ability3Pressed(int client, CommandInfo commandinfo)
        {
            GetWcPlayer(client)?.GetRace()?.InvokeAbility(2);
        }

        private void Ability4Pressed(int client, CommandInfo commandinfo)
        {
            GetWcPlayer(client)?.GetRace()?.InvokeAbility(3);
        }

        public override void Unload(bool hotReload)
        {
            base.Unload(hotReload);
        }

        public void ShowSkillPointMenu(WarcraftPlayer wcPlayer)
        {
            var menu = new RadioMenu($"Level up skills ({XpSystem.GetFreeSkillPoints(wcPlayer)} available)");
            var race = wcPlayer.GetRace();

            for (int i = 0; i < 4; i++)
            {
                var ability = race.GetAbility(i);

                var displayString = $"{ability.DisplayName} ({wcPlayer.GetAbilityLevel(i)})";

                bool disabled = false;
                if (i == 3)
                {
                    if (wcPlayer.currentLevel < WarcraftPlugin.MaxLevel) disabled = true;
                    if (wcPlayer.GetAbilityLevel(i) >= 1) disabled = true;
                }
                else
                {
                    if (wcPlayer.GetAbilityLevel(i) >= WarcraftPlugin.MaxSkillLevel) disabled = true;
                }

                if (XpSystem.GetFreeSkillPoints(wcPlayer) == 0) disabled = true;

                menu.AddMenuOption(displayString, i.ToString(), disabled ? RadioMenuOptionFlags.DISABLED : 0);
            }

            menu.Handler = (player, option) =>
            {
                var wcPlayer = player.GetWarcraftPlayer();
                var abilityIndex = Convert.ToInt32(option.Value);

                if (XpSystem.GetFreeSkillPoints(wcPlayer) > 0)
                    wcPlayer.GrantAbilityLevel(abilityIndex);

                if (XpSystem.GetFreeSkillPoints(wcPlayer) > 0)
                {
                    //Server.NextFrame(() => ShowSkillPointMenu(wcPlayer));
                    ShowSkillPointMenu(wcPlayer);
                }
            };

            menu.Display(wcPlayer.Index);
        }
    }
}
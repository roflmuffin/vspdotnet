using System;
using CSGONET.API;
using CSGONET.API.Modules.Players;
using Dapper;
using Microsoft.Data.Sqlite;

namespace ClassLibrary2
{
    public class Database
    {
        private SqliteConnection _connection;

        public void Initialize()
        {
            _connection =
                new SqliteConnection(
                    "Data Source=I:\\csgo_server\\csgo\\addons\\csgodotnet\\modules\\ClassLibrary2\\database.db");

            _connection.Execute(@"
CREATE TABLE IF NOT EXISTS `players` (
	`steamid` VARCHAR(32) NOT NULL,
	`currentRace` VARCHAR(32) NOT NULL DEFAULT 'undead_scourge',
  `name` VARCHAR(64),
	PRIMARY KEY (`steamid`));");

            _connection.Execute(@"
CREATE TABLE IF NOT EXISTS `raceinformation` (
  `steamid` VARCHAR(32) NOT NULL,
  `racename` VARCHAR(32) NOT NULL,
  `currentXP` INT NULL DEFAULT 0,
  `currentLevel` INT NULL DEFAULT 1,
  `amountToLevel` INT NULL DEFAULT 100,
  `ability1level` TINYINT NULL DEFAULT 0,
  `ability2level` TINYINT NULL DEFAULT 0,
  `ability3level` TINYINT NULL DEFAULT 0,
  `ability4level` TINYINT NULL DEFAULT 0,
  PRIMARY KEY (`steamid`, `racename`));
");
        }

        public bool ClientExistsInDatabase(string steamid)
        {
            return _connection.ExecuteScalar<int>("select count(*) from players where steamid = @steamid",
                new {steamid}) > 0;
        }

        public void AddNewClientToDatabase(Player player)
        {
            _connection.Execute(@"
            INSERT INTO players (`steamid`, `currentRace`)
	        VALUES(@steamid, 'undead_scourge')",
                new {steamid = player.PlayerInfo.SteamId});
        }

        public WarcraftPlayer LoadClientFromDatabase(Player player, XpSystem xpSystem)
        {
            var dbPlayer = _connection.QueryFirst<DatabasePlayer>(@"
            SELECT * FROM `players` WHERE `steamid` = @steamid",
                new {steamid = player.PlayerInfo.SteamId});

            var raceInformationExists = _connection.ExecuteScalar<int>(@"
            select count(*) from `raceinformation` where steamid = @steamid AND racename = @racename",
                new {steamid = player.PlayerInfo.SteamId, racename = dbPlayer.CurrentRace}
            ) > 0;

            if (!raceInformationExists)
            {
                _connection.Execute(@"
                insert into `raceinformation` (steamid, racename)
                values (@steamid, @racename);",
                    new {steamid = player.PlayerInfo.SteamId, racename = dbPlayer.CurrentRace});
            }

            var raceInformation = _connection.QueryFirst<DatabaseRaceInformation>(@"
            SELECT * from `raceinformation` where `steamid` = @steamid AND `racename` = @racename",
                new {steamid = player.PlayerInfo.SteamId, racename = dbPlayer.CurrentRace});

            var wcPlayer = new WarcraftPlayer(player.Index);
            wcPlayer.LoadFromDatabase(raceInformation, xpSystem);
            WarcraftPlugin.Instance.SetWcPlayer(player.Index, wcPlayer);

            return wcPlayer;
        }

        public void SaveClientToDatabase(Player player)
        {
            var wcPlayer = WarcraftPlugin.Instance.GetWcPlayer(player.Index);
            Server.PrintToConsole($"Saving {player.Name} to database...");

            var raceInformationExists = _connection.ExecuteScalar<int>(@"
            select count(*) from `raceinformation` where steamid = @steamid AND racename = @racename",
                new { steamid = player.PlayerInfo.SteamId, racename = wcPlayer.raceName }
            ) > 0;

            if (!raceInformationExists)
            {
                _connection.Execute(@"
                insert into `raceinformation` (steamid, racename)
                values (@steamid, @racename);",
                    new { steamid = player.PlayerInfo.SteamId, racename = wcPlayer.raceName });
            }

            _connection.Execute(@"
UPDATE `raceinformation` SET `currentXP` = @currentXp,
 `currentLevel` = @currentLevel,
 `ability1level` = @ability1Level,
 `ability2level` = @ability2Level,
 `ability3level` = @ability3Level,
 `ability4level` = @ability4Level,
 `amountToLevel` = @amountToLevel WHERE `steamid` = @steamid AND `racename` = @racename;",
                new
                {
                    currentXp = wcPlayer.currentXp,
                    currentLevel = wcPlayer.currentLevel,
                    ability1Level = wcPlayer.GetAbilityLevel(0),
                    ability2Level = wcPlayer.GetAbilityLevel(1),
                    ability3Level = wcPlayer.GetAbilityLevel(2),
                    ability4Level = wcPlayer.GetAbilityLevel(3),
                    amountToLevel = wcPlayer.amountToLevel,
                    steamid = player.PlayerInfo.SteamId,
                    racename = wcPlayer.raceName
                });

        }

        public void SaveClients()
        {
            for (int i = 1; i < 65; i++)
            {
                var player = Player.FromIndex(i);
                if (player == null || !player.IsValid || !player.IsAlive) continue;

                var wcPlayer = WarcraftPlugin.Instance.GetWcPlayer(i);
                if (wcPlayer == null) continue;

                SaveClientToDatabase(player);
            }
        }

        public void SaveCurrentRace(Player player)
        {
            var wcPlayer = WarcraftPlugin.Instance.GetWcPlayer(player.Index);

            _connection.Execute(@"
            UPDATE `players` SET `currentRace` = @currentRace, `name` = @name WHERE `steamid` = @steamid;",
                new
                {
                    currentRace = wcPlayer.raceName,
                    name = player.Name,
                    steamid = player.PlayerInfo.SteamId
                });
        }
    }

    public class DatabasePlayer
    {
        public string SteamId { get; set; }
        public string CurrentRace { get; set; }
        public string Name { get; set; }
    }

    public class DatabaseRaceInformation
    {
        public string SteamId { get; set; }
        public string RaceName { get; set; }
        public int CurrentXp { get; set; }
        public int CurrentLevel { get; set; }
        public int AmountToLevel { get; set; }
        public int Ability1Level { get; set; }
        public int Ability2Level { get; set; }
        public int Ability3Level { get; set; }
        public int Ability4Level { get; set; }
    }
}
using System;
using System.Collections.Generic;
using System.Linq;

namespace ClassLibrary2.Races
{
    public class RaceManager
    {
        private Dictionary<string, Type> _races = new Dictionary<string, Type>();
        private Dictionary<string, WarcraftRace> _raceObjects = new Dictionary<string, WarcraftRace>();

        public void Initialize()
        {
            RegisterRace<RaceUndeadScourge>();
            RegisterRace<RaceHumanAlliance>();
        }

        private void RegisterRace<T>() where T : WarcraftRace, new()
        {
            var race = new T();
            race.Register();
            _races[race.InternalName] = typeof(T);
            _raceObjects[race.InternalName] = race;
        }

        public WarcraftRace InstantiateRace(string name)
        {
            if (!_races.ContainsKey(name)) throw new Exception("Race not found: " + name);

            var race = (WarcraftRace)Activator.CreateInstance(_races[name]);
            race.Register();

            return race;
        }

        public WarcraftRace[] GetAllRaces()
        {
            return _raceObjects.Values.ToArray();
        }

        public WarcraftRace GetRace(string name)
        {
            return _raceObjects.ContainsKey(name) ? _raceObjects[name] : null;
        }
    }
}
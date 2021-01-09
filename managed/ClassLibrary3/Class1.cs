using System;
using CSGONET.API.Core;
using CSGONET.API.Modules.Events;

namespace ClassLibrary3
{
    public class Class1 : BasePlugin
    {
        public override string ModuleName => "F";
        public override string ModuleVersion => "F";

        public override void Load(bool hotReload)
        {
            base.Load(hotReload);
            RegisterEventHandler("player_connect", Handler);
        }

        private void Handler(GameEvent obj)
        {
            Console.WriteLine("Connect class lib 3");
        }
    }
}

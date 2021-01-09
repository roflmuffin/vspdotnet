using System;
using CSGONET.API.Core;

namespace CSGONET.API.Modules.Commands
{
    public class CommandInfo
    {
        public delegate void CommandCallback(int client, CommandInfo commandInfo);

        public IntPtr Handle { get; private set; }
        internal CommandInfo(IntPtr pointer)
        {
            Handle = pointer;
        }

        public int ArgCount() => NativeAPI.CommandGetArgCount(Handle);

        public string ArgString() => NativeAPI.CommandGetArgString(Handle);

        public string GetCommandString() => NativeAPI.CommandGetCommandString(Handle);

        public string ArgByIndex(int index) => NativeAPI.CommandGetArgByIndex(Handle, index);
    }
}
using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;
using CSGONET.API.Core;
using CSGONET.API.Modules.Entities;
using CSGONET.API.Modules.Errors;
using CSGONET.API.Modules.Sound;

namespace CSGONET.API
{
    public class Server
    {
        public static float TickInterval => NativeAPI.GetTickInterval();

        public static void ExecuteCommand(string command) => NativeAPI.IssueServerCommand(command);

        public static string MapName => NativeAPI.GetMapName();
        //public static string GameFolderName => NativePINVOKE.GetGameFolderName();
        public static void PrintToConsole(string message) => NativeAPI.PrintToConsole(message);

        public static void PrintToLog(string message) => NativePINVOKE.PrintToLog(message);

        public static double TickedTime => NativeAPI.GetTickedTime();
        public static float CurrentTime => NativeAPI.GetCurrentTime();
        public static int TickCount => NativeAPI.GetTickCount();
        public static float GameFrameTime => NativeAPI.GetGameframeTime();
        public static double EngineTime => NativeAPI.GetEngineTime();
        //public static string GetLatestError() => NativePINVOKE.GetLatestError();
        public static void PrecacheModel(string name) => NativeAPI.PrecacheModel(name);
        public static void PrecacheSound(string name) => Sound.PrecacheSound(name);

        private static List<Action> nextFrameTasks = new List<Action>();

        public static void NextFrame(Action task)
        {
            nextFrameTasks.Add(task);
            var ptr = Marshal.GetFunctionPointerForDelegate(task);
            NativeAPI.QueueTaskForNextFrame(ptr);
        }
    }
}

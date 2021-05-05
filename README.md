# VSP.NET

VSP.NET is a server side modding framework for Counter-Strike: Global Offensive. This project attempts to implement a .NET Core scripting layer on top of Valve Server Plugins (VSP), allowing developers to create plugins that interact with the game server in a modern language (C#) to facilitate the creation of maintainable and testable code.

VSP.NET is in its infancy and has been a dream project of mine after maintaining multiple CS:GO servers and being forced to write plugins in the archaic, C-like language of SourcePawn for SourceMod.

While VSP.net lacks significant features when compared to these big players, I hope that it can one day be the go to mod for CS:GO (and other source engine) plugin scripting.

**NOTE**: This plugin only supports Windows at this point in time due to CS:GO server being 32-bit and .NET Core only supporting x64 on Linux. If you want Linux support you'll have to bug the .NET team about it.

## What works?

These features are the core of the platform and work pretty well/have a low risk of causing issues.

- [x] Console Variables, Console Commands, Server Commands (e.g. sv_customvar)
- [x] Game Event Handlers & Custom Events (e.g. player_death)
- [x] Game Tick Based Timers (e.g. repeating map timers)
- [x] Listeners (e.g. client connected, disconnected, map start etc.)
- [x] Server Information (current map, game time, tick rate, model precaching)
- [x] Radio Menus (create menus and respond to selections)

## What kind of works?

These features have a rudimentary implementation but have not been thoroughly tested.

- [x] Entity Manipulation
  - Basic manipulation of networked entity props/sendinfo e.g. position, team, ground entity, position, velocity etc.
  - Currently missing entity input/output functionality, for things like func_door "Open" inputs etc.
- [x] Engine Raycasts
  - Can do basic raycasts with predicate filter to match entities
- [x] Poor Memory Functionality
  - It is possible to hook and call virtual functions by supplying the int offset & parameters of the method.
  - Might cause crashes if you use the wrong parameters :(
  - These are things that are traditionally provided by SDK_Hooks or SDK_Tools in SourceMod
- [x] Multi Threading & Game Frames
  - Game Event Listeners & Command Handlers happen synchronously in the game frame
  - If you spawn a new thread/task in .NET you will need to queue your game actions for the next in-game frame or some things might crash.

## Examples

You can view the example Warcraft plugin located in the managed folder under "ClassLibrary2" (until it gets renamed). This plugin shows how you can hook events, create commands & console variables, use third party libraries (SQLite) and do basic entity manipulation.

## Credits

A lot of code has been borrowed from SourceMod as well as Source.Python, two pioneering source engine plugin frameworks which this project lends a lot of its credit to.

I've also used the scripting context & native system that is implemented in FiveM for GTA5.

## How to Build

Building requires CMake for Windows.

Clone the repository

```bash
git clone https://github.com/roflmuffin/VSP.NET
```

Init and update submodules

```bash
git submodule init
git submodule update
```

Make build folder

```bash
mkdir build
cd build
```

Generate CMake Build Files

```bash
cmake -A Win32 ..
```

Build

```bash
cmake --build . --config Debug
```

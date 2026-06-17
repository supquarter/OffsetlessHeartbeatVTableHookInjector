# Offsetless PlayerListManager VTable Hook Injector

Offsetless, driverless Roblox injector using PlayerListManager vtable hooking with a WPF UI featuring Monaco editor and Luau script execution.

## Features

- **Offsetless**: scans memory for PlayerListManager by class name (`"Players"`), no hardcoded addresses
- **Driverless**: uses NT syscall trampolines for RPM/WPM, no kernel driver
- **Manual mapping**: module DLL loaded via manual map into `mshtml.dll` memory
- **Script execution**: execute Luau source or bytecode inside Roblox
- **Print capture**: hooks `luaB_print` to capture script output
- **IPC**: shared memory + event-based communication between UI and module
- **Monaco editor**: WPF + WebView2 with Luau syntax highlighting and autocomplete

## Components

| Component | Language | Description |
|-----------|----------|-------------|
| `Injector/` | C++ | Finds PlayerListManager, hooks vtable, manual-maps module |
| `Module/` | C++ | Injected DLL — IPC server, Luau VM access, script executor |
| `UI/` | C# WPF | Monaco editor, inject/execute controls, output console |
| `Shared/` | C++ | IPC protocol — shared memory layout and status enums |

## How It Works

1. **UI** launches `injector.exe`, which finds `RobloxPlayerBeta.exe`
2. **Injector** scans all committed RW/RWX memory for Roblox instances with class name `"Players"` (PlayerListManager)
3. Hooks PlayerListManager vtable → redirects `step()` to shellcode in `devenum.dll`
4. Manual-maps `module.dll` into `mshtml.dll` — imports, relocations, TLS, exception tables processed
5. **Module** creates shared memory `Global\RBX_SIPC_Mem` + events for IPC
6. Module finds Luau `global_State`, hooks `luaB_print` for output capture
7. UI sends scripts via shared memory → module executes via `loadsafe` + `luau_execute` → output returned

## Build

### Requirements

- Visual Studio 2022 (v143 toolset)
- .NET 10 SDK
- Windows 10/11 x64

### Build Steps

```powershell
# Injector (output: heartbeatinj/build/injector.exe)
msbuild heartbeatinj/injector/injector.vcxproj /p:Configuration=Release /p:Platform=x64

# Module DLL (output: heartbeatinj/build/module.dll)
msbuild heartbeatinj/module/mdopduel/mdopduel.vcxproj /p:Configuration=Release /p:Platform=x64

# UI (output: UI/bin/Release/net10.0-windows/)
dotnet build heartbeatinj/UI/InjectorUI.csproj -c Release
```

## Usage

1. Place `injector.exe` and `module.dll` in the same folder
2. Launch Roblox
3. Run `InjectorUI.exe`
4. Click **Inject** → wait for green status
5. Write or paste Luau script in the editor
6. Click **Execute** → output appears in the console

## IPC Protocol

Shared memory: `Global\RBX_SIPC_Mem` (64KB)
Events: `Global\RBX_SIPC_Cmd` (command ready), `Global\RBX_SIPC_Res` (response ready)

```
[0x00] Status     (Idle=0, CmdPending=1, Executing=2, Done=3, Error=4)
[0x04] ScriptType (Source=0, Bytecode=1)
[0x08] ScriptSize
[0x0C] OutputSize
[0x10] Script data + Output data
```

## Credits

- Original Heartbeat injector by [x4v](https://github.com/x4v) / nuvq
- Offsetless + driverless modifications by atreluted
- PlayerListManager migration, module executor, Monaco UI by supquarter
- Offsets dumper by Versual / VannaRy

## Disclaimer

For educational purposes only. Use at your own risk.

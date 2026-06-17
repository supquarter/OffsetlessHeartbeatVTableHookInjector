#include "pch.h"
#include <thread>
#include <string>
#include "IPC/SharedMemory.hpp"
#include "Luau/State.hpp"
#include "Luau/Executor.hpp"
#include "Luau/Offsets.hpp"
#include "Luau/Structs.hpp"
#include "Hooks/PrintHook.hpp"

static bool g_Running = true;

void IpcWorkerThread() {
    while (g_Running) {
        IPC::ScriptRequest req = IPC::WaitForScript(2000);
        if (req.Script.empty()) continue;

        CosmicState* L = Luau::FindLuaState();
        if (!L) {
            L = (CosmicState*)Luau::GetLuaStateViaFunction();
        }

        if (!L) {
            IPC::SendError("Failed to find Lua state");
            continue;
        }

        int result = -1;
        if (req.Type == ScriptType::Source) {
            result = Luau::ExecuteSource(L, req.Script.c_str(), "=UI_Script");
        } else if (req.Type == ScriptType::Bytecode) {
            result = Luau::ExecuteBytecode(L, (const uint8_t*)req.Script.c_str(), req.Script.size(), "=UI_Bytecode");
        }

        std::string output = Luau::GetOutput();
        if (result != 0 && output.empty()) {
            IPC::SendError("Script execution returned code " + std::to_string(result));
        } else {
            IPC::SendOutput(output);
        }
    }
}

void InitializeModule() {
    bool ipcReady = IPC::Create();
    bool luauReady = Luau::Initialize();
    bool printHooked = Hooks::Install();

    if (ipcReady && luauReady) {
        std::thread ipcThread(IpcWorkerThread);
        ipcThread.detach();
    }
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, [](LPVOID) -> DWORD {
            Sleep(100);
            InitializeModule();
            return 0;
        }, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        g_Running = false;
        Hooks::Remove();
        IPC::Destroy();
        break;
    }
    return TRUE;
}

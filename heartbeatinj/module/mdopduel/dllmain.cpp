#include "pch.h"
#include "IPC/SharedMemory.hpp"
#include "Luau/State.hpp"
#include "Luau/Executor.hpp"
#include "Luau/Offsets.hpp"
#include "Luau/Structs.hpp"
#include "Hooks/PrintHook.hpp"

static volatile LONG g_Running = 1;

DWORD WINAPI IpcWorkerThread(LPVOID) {
    char scriptBuf[IPC_MAX_SCRIPT + 1];
    uint32_t scriptSize;
    uint32_t scriptType;

    while (g_Running) {
        if (!IPC::WaitForScript(scriptBuf, &scriptSize, &scriptType, 2000))
            continue;

        CosmicState* L = Luau::FindLuaState();
        if (!L) L = (CosmicState*)Luau::GetLuaStateViaFunction();

        if (!L) { IPC::SendError("No Lua state"); continue; }

        int result = -1;
        if (scriptType == (uint32_t)ScriptType::Source)
            result = Luau::ExecuteSource(L, scriptBuf, "=UI_Script");
        else
            result = Luau::ExecuteBytecode(L, (const uint8_t*)scriptBuf, scriptSize, "=UI_Bytecode");

        const char* output = Luau::GetOutput();
        uint32_t outLen = Luau::g_OutputLen;

        if (result != 0 && outLen == 0) {
            char errBuf[64];
            int len = sprintf_s(errBuf, sizeof(errBuf), "Script failed: %d", result);
            IPC::SendOutput(errBuf, len > 0 ? (uint32_t)len : 0);
        } else {
            IPC::SendOutput(output, outLen);
        }
    }
    return 0;
}

void InitializeModule() {
    if (!IPC::Create()) return;

    MessageBoxA(NULL, "IPC created, initializing Luau...", "Module", MB_OK);

    if (!Luau::Initialize()) {
        IPC::SendError("Luau init failed - offsets may be wrong for this Roblox version");
        return;
    }

    MessageBoxA(NULL, "Luau initialized, installing hooks...", "Module", MB_OK);

    Hooks::Install();

    MessageBoxA(NULL, "Module ready - waiting for scripts", "Module", MB_OK);

    CreateThread(NULL, 0, IpcWorkerThread, NULL, 0, NULL);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    (void)lpReserved;
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
        InterlockedExchange(&g_Running, 0);
        Hooks::Remove();
        IPC::Destroy();
        break;
    }
    return TRUE;
}

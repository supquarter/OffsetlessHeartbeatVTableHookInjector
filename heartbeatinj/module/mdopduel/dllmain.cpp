#include "pch.h"
#include "IPC/SharedMemory.hpp"
#include "Luau/State.hpp"
#include "Luau/Executor.hpp"
#include "Luau/Offsets.hpp"
#include "Luau/Structs.hpp"
#include "Hooks/PrintHook.hpp"

static volatile LONG g_Running = 1;

static void LogToFile(const char* msg) {
    HANDLE h = CreateFileA("C:\\Users\\Public\\module_log.txt", FILE_APPEND_DATA, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (h != INVALID_HANDLE_VALUE) {
        DWORD written;
        WriteFile(h, msg, (DWORD)strlen(msg), &written, NULL);
        WriteFile(h, "\r\n", 2, &written, NULL);
        CloseHandle(h);
    }
}

DWORD WINAPI IpcWorkerThread(LPVOID) {
    LogToFile("IPC worker thread started");
    char scriptBuf[IPC_MAX_SCRIPT + 1];
    uint32_t scriptSize;
    uint32_t scriptType;

    while (g_Running) {
        if (!IPC::WaitForScript(scriptBuf, &scriptSize, &scriptType, 2000))
            continue;

        LogToFile("Received script from UI");

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
    LogToFile("InitializeModule called");

    if (!IPC::Create()) {
        LogToFile("IPC::Create FAILED");
        return;
    }
    LogToFile("IPC created successfully");

    if (!Luau::Initialize()) {
        LogToFile("Luau::Initialize FAILED");
        IPC::SendError("Luau init failed");
        return;
    }
    LogToFile("Luau initialized");

    Hooks::Install();
    LogToFile("Print hook installed");

    CreateThread(NULL, 0, IpcWorkerThread, NULL, 0, NULL);
    LogToFile("Module ready");
}

static DWORD WINAPI InitThread(LPVOID) {
    Sleep(100);
    InitializeModule();
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    (void)hModule; (void)lpReserved;
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        LogToFile("DllMain DLL_PROCESS_ATTACH");
        CreateThread(NULL, 0, InitThread, NULL, 0, NULL);
        break;
    case DLL_PROCESS_DETACH:
        LogToFile("DllMain DLL_PROCESS_DETACH");
        InterlockedExchange(&g_Running, 0);
        Hooks::Remove();
        IPC::Destroy();
        break;
    }
    return TRUE;
}

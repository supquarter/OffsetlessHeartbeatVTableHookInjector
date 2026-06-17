#pragma once
#include <Windows.h>
#include <cstdint>
#include <string>
#include "IpcProtocol.h"

namespace IPC {

static HANDLE g_hMapFile = nullptr;
static IpcLayout* g_pIpc = nullptr;
static HANDLE g_hEventCmd = nullptr;
static HANDLE g_hEventRes = nullptr;

static bool Create() {
    g_hMapFile = CreateFileMappingW(
        INVALID_HANDLE_VALUE,
        NULL,
        PAGE_READWRITE,
        0,
        IPC_BUFFER_SIZE,
        IPC_SHARED_MEM_NAME
    );
    if (!g_hMapFile) {
        DWORD err = GetLastError();
        if (err == ERROR_ALREADY_EXISTS) {
            g_hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, IPC_SHARED_MEM_NAME);
        }
    }
    if (!g_hMapFile) return false;

    g_pIpc = (IpcLayout*)MapViewOfFile(g_hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, IPC_BUFFER_SIZE);
    if (!g_pIpc) return false;

    memset(g_pIpc, 0, IPC_BUFFER_SIZE);
    g_pIpc->Fields.Status = (uint32_t)IpcStatus::Idle;

    g_hEventCmd = CreateEventW(NULL, FALSE, FALSE, IPC_EVENT_CMD_NAME);
    if (!g_hEventCmd) {
        g_hEventCmd = OpenEventW(EVENT_ALL_ACCESS, FALSE, IPC_EVENT_CMD_NAME);
    }

    g_hEventRes = CreateEventW(NULL, FALSE, FALSE, IPC_EVENT_RES_NAME);
    if (!g_hEventRes) {
        g_hEventRes = OpenEventW(EVENT_ALL_ACCESS, FALSE, IPC_EVENT_RES_NAME);
    }

    return g_hMapFile && g_pIpc && g_hEventCmd && g_hEventRes;
}

static void Destroy() {
    if (g_pIpc) { UnmapViewOfFile(g_pIpc); g_pIpc = nullptr; }
    if (g_hMapFile) { CloseHandle(g_hMapFile); g_hMapFile = nullptr; }
    if (g_hEventCmd) { CloseHandle(g_hEventCmd); g_hEventCmd = nullptr; }
    if (g_hEventRes) { CloseHandle(g_hEventRes); g_hEventRes = nullptr; }
}

struct ScriptRequest {
    std::string Script;
    ScriptType Type;
};

static ScriptRequest WaitForScript(DWORD timeoutMs = INFINITE) {
    ScriptRequest req;

    while (true) {
        DWORD waitResult = WaitForSingleObject(g_hEventCmd, timeoutMs);
        if (waitResult != WAIT_OBJECT_0) {
            req.Script = "";
            return req;
        }

        if (g_pIpc->Fields.Status == (uint32_t)IpcStatus::CmdPending) {
            uint32_t size = g_pIpc->Fields.ScriptSize;
            if (size > IPC_MAX_SCRIPT) size = IPC_MAX_SCRIPT;

            req.Script.assign((const char*)g_pIpc->GetScript(), size);
            req.Type = (ScriptType)g_pIpc->Fields.ScriptType;

            g_pIpc->Fields.Status = (uint32_t)IpcStatus::Executing;
            return req;
        }
    }
}

static void SendOutput(const std::string& output) {
    uint32_t outSize = (uint32_t)output.size();
    if (outSize > IPC_MAX_OUTPUT) outSize = IPC_MAX_OUTPUT;

    g_pIpc->WriteOutput((const uint8_t*)output.c_str(), outSize);
    g_pIpc->Fields.Status = (uint32_t)IpcStatus::Done;
    SetEvent(g_hEventRes);
}

static void SendError(const std::string& error) {
    std::string errMsg = "[Error] " + error;
    uint32_t outSize = (uint32_t)errMsg.size();
    if (outSize > IPC_MAX_OUTPUT) outSize = IPC_MAX_OUTPUT;

    g_pIpc->WriteOutput((const uint8_t*)errMsg.c_str(), outSize);
    g_pIpc->Fields.Status = (uint32_t)IpcStatus::Error;
    SetEvent(g_hEventRes);
}

}

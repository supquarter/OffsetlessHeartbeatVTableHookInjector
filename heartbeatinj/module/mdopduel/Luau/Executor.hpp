#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include "State.hpp"
#include "Structs.hpp"
#include "Offsets.hpp"
#include "IpcProtocol.h"

namespace Luau {

static char g_OutputBuffer[IPC_MAX_OUTPUT];
static uint32_t g_OutputLen = 0;

static int CompileAndLoad_L(CosmicState* L, const char* source, const char* chunkname) {
    using loadsafe_fn = int(*)(CosmicState*, const char*, const char*);
    auto loadsafe = (loadsafe_fn)Functions::loadsafe;
    if (!loadsafe) return -1;
    return loadsafe(L, source, chunkname ? chunkname : "=script");
}

static int CompileAndLoad(CosmicState* L, const char* source, const char* chunkname) {
    __try { return CompileAndLoad_L(L, source, chunkname); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int ExecuteDirect_L(CosmicState* L) {
    using luau_execute_fn = int(*)(CosmicState*);
    auto luau_execute = (luau_execute_fn)Functions::luau_execute;
    if (!luau_execute) return -1;
    return luau_execute(L);
}

static int ExecuteDirect(CosmicState* L) {
    __try { return ExecuteDirect_L(L); }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int ExecuteSource(CosmicState* L, const char* source, const char* chunkname) {
    g_OutputLen = 0;
    g_OutputBuffer[0] = 0;

    int loadResult = CompileAndLoad(L, source, chunkname);
    if (loadResult != 0) {
        sprintf_s(g_OutputBuffer, sizeof(g_OutputBuffer), "[Load error: %d]", loadResult);
        g_OutputLen = (uint32_t)strlen(g_OutputBuffer);
        return loadResult;
    }

    int execResult = ExecuteDirect(L);
    if (execResult != 0 && g_OutputLen == 0) {
        sprintf_s(g_OutputBuffer, sizeof(g_OutputBuffer), "[Exec error: %d]", execResult);
        g_OutputLen = (uint32_t)strlen(g_OutputBuffer);
    }
    return execResult;
}

static void PushOutput(const char* str, size_t len) {
    if (g_OutputLen + len + 2 > sizeof(g_OutputBuffer)) return;
    memcpy(g_OutputBuffer + g_OutputLen, str, len);
    g_OutputLen += (uint32_t)len;
    g_OutputBuffer[g_OutputLen++] = '\n';
    g_OutputBuffer[g_OutputLen] = 0;
}

static void PushOutputInt(int val) {
    char buf[32];
    int n = sprintf_s(buf, sizeof(buf), "%d", val);
    if (n > 0) PushOutput(buf, n);
}

static const char* GetOutput() { return g_OutputBuffer; }

static int TryLoadBytecode(CosmicState* L, const uint8_t* bytecode, size_t size, const char* chunkname) {
    __try {
        using luaVMLoad_fn = int(*)(CosmicState*, const char*, const char*, size_t, int);
        auto luaVMLoad = (luaVMLoad_fn)Functions::LuaVMLoad;
        if (!luaVMLoad) return -2;
        return luaVMLoad(L, chunkname, (const char*)bytecode, size, 0);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) { return -1; }
}

static int ExecuteBytecode(CosmicState* L, const uint8_t* bytecode, size_t size, const char* chunkname) {
    g_OutputLen = 0;
    g_OutputBuffer[0] = 0;

    int loadResult = TryLoadBytecode(L, bytecode, size, chunkname);
    if (loadResult == -2) {
        sprintf_s(g_OutputBuffer, sizeof(g_OutputBuffer), "[Error: LuaVMLoad not available]");
        g_OutputLen = (uint32_t)strlen(g_OutputBuffer);
        return -1;
    }
    if (loadResult != 0) {
        sprintf_s(g_OutputBuffer, sizeof(g_OutputBuffer), "[BC Load error: %d]", loadResult);
        g_OutputLen = (uint32_t)strlen(g_OutputBuffer);
        return loadResult;
    }

    int execResult = ExecuteDirect(L);
    if (execResult != 0 && g_OutputLen == 0) {
        sprintf_s(g_OutputBuffer, sizeof(g_OutputBuffer), "[BC Exec error: %d]", execResult);
        g_OutputLen = (uint32_t)strlen(g_OutputBuffer);
    }
    return execResult;
}

}

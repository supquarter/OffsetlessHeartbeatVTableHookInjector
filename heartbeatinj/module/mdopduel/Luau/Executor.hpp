#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <string>
#include "State.hpp"
#include "Structs.hpp"
#include "Offsets.hpp"

namespace Luau {

static std::string g_OutputBuffer;

static int CompileAndLoad_L(CosmicState* L, const char* source, const char* chunkname) {
    using loadsafe_fn = int(*)(CosmicState*, const char*, const char*);
    auto loadsafe = (loadsafe_fn)Functions::loadsafe;
    if (!loadsafe) return -1;
    return loadsafe(L, source, chunkname ? chunkname : "=script");
}

static int CompileAndLoad(CosmicState* L, const char* source, const char* chunkname) {
    __try {
        return CompileAndLoad_L(L, source, chunkname);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return -1;
    }
}

static int ExecuteDirect_L(CosmicState* L) {
    using luau_execute_fn = int(*)(CosmicState*);
    auto luau_execute = (luau_execute_fn)Functions::luau_execute;
    if (!luau_execute) return -1;
    return luau_execute(L);
}

static int ExecuteDirect(CosmicState* L) {
    __try {
        return ExecuteDirect_L(L);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return -1;
    }
}

static int ExecuteSource(CosmicState* L, const char* source, const char* chunkname) {
    g_OutputBuffer.clear();

    int loadResult = CompileAndLoad(L, source, chunkname);
    if (loadResult != 0) {
        g_OutputBuffer = "[Load error: ";
        g_OutputBuffer += std::to_string(loadResult);
        g_OutputBuffer += "]";
        return loadResult;
    }

    int execResult = ExecuteDirect(L);
    if (execResult != 0 && g_OutputBuffer.empty()) {
        g_OutputBuffer = "[Exec error: ";
        g_OutputBuffer += std::to_string(execResult);
        g_OutputBuffer += "]";
    }
    return execResult;
}

static void PushOutput(const char* str, size_t len) {
    g_OutputBuffer.append(str, len);
    g_OutputBuffer += '\n';
}

static std::string GetOutput() {
    std::string out = g_OutputBuffer;
    g_OutputBuffer.clear();
    return out;
}

static int ExecuteBytecode_L(CosmicState* L, const uint8_t* bytecode, size_t size, const char* chunkname) {
    using luaVMLoad_fn = int(*)(CosmicState*, const char*, const char*, size_t, int);
    auto luaVMLoad = (luaVMLoad_fn)Functions::LuaVMLoad;
    if (!luaVMLoad) return -2;
    return luaVMLoad(L, chunkname, (const char*)bytecode, size, 0);
}

static int TryLoadBytecode(CosmicState* L, const uint8_t* bytecode, size_t size, const char* chunkname) {
    __try {
        return ExecuteBytecode_L(L, bytecode, size, chunkname);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return -1;
    }
}

static int ExecuteBytecode(CosmicState* L, const uint8_t* bytecode, size_t size, const char* chunkname) {
    g_OutputBuffer.clear();

    int loadResult = TryLoadBytecode(L, bytecode, size, chunkname);

    if (loadResult == -2) {
        g_OutputBuffer = "[Error: LuaVMLoad not available]";
        return -1;
    }
    if (loadResult != 0) {
        g_OutputBuffer = "[BC Load error: ";
        g_OutputBuffer += std::to_string(loadResult);
        g_OutputBuffer += "]";
        return loadResult;
    }

    int execResult = ExecuteDirect(L);
    if (execResult != 0 && g_OutputBuffer.empty()) {
        g_OutputBuffer = "[BC Exec error: ";
        g_OutputBuffer += std::to_string(execResult);
        g_OutputBuffer += "]";
    }
    return execResult;
}

}

#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include "Offsets.hpp"
#include "Structs.hpp"

namespace Luau {

static uintptr_t g_RobloxBase = 0;
static global_State* g_GState = nullptr;
static uint64_t g_PtrEncKey[4] = { 0 };

static uintptr_t DecodePointer(uintptr_t encoded) {
    if (!g_GState) return encoded;
    uintptr_t keyAddr = (uintptr_t)g_GState + 0x538;
    uint64_t keys[4];
    memcpy(keys, (void*)keyAddr, sizeof(keys));
    for (int i = 0; i < 4; i++) {
        if (keys[i]) encoded ^= keys[i];
    }
    return encoded;
}

template<typename T>
static T DecodePtr(T encoded) {
    return (T)DecodePointer((uintptr_t)encoded);
}

static global_State* FindGlobalState() {
    if (g_GState) return g_GState;

    g_RobloxBase = (uintptr_t)GetModuleHandle(NULL);
    if (!g_RobloxBase) return nullptr;

    using GetGlobalStateFn = global_State * (*)();
    auto getGlobalState = (GetGlobalStateFn)Functions::GetGlobalState;
    if (!getGlobalState) return nullptr;

    __try {
        g_GState = getGlobalState();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return nullptr;
    }

    if (g_GState) {
        memcpy(g_PtrEncKey, (void*)((uintptr_t)g_GState + 0x538), sizeof(g_PtrEncKey));
    }

    return g_GState;
}

static CosmicState* FindLuaState() {
    if (!FindGlobalState()) return nullptr;

    __try {
        auto mt = g_GState->mainthread;
        if (mt && mt->tt > 0 && mt->tt < 10) {
            return mt;
        }
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {}

    return nullptr;
}

static uintptr_t GetLuaStateViaFunction() {
    using GetLuaStateFn = uintptr_t(*)();
    auto fn = (GetLuaStateFn)Functions::GetLuaState;
    if (!fn) return 0;

    __try {
        return fn();
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }
}

static bool Initialize() {
    if (!FindGlobalState()) {
        auto state = GetLuaStateViaFunction();
        if (state) {
            auto L = (CosmicState*)state;
            if (L->global) {
                g_GState = L->global;
                memcpy(g_PtrEncKey, (void*)((uintptr_t)g_GState + 0x538), sizeof(g_PtrEncKey));
            }
        }
    }
    return g_GState != nullptr;
}

}

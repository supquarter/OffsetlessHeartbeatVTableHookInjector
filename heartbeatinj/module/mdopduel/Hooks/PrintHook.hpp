#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include "../Luau/Offsets.hpp"
#include "../Luau/Executor.hpp"

namespace Hooks {

static uintptr_t g_OriginalPrint = 0;
static uint8_t g_OriginalPrintBytes[16] = { 0 };
static bool g_PrintHooked = false;

static int Hk_Print_L(CosmicState* L) {
    int nargs = (int)(L->top - L->base);
    if (nargs < 1) return 0;

    for (int i = 0; i < nargs; i++) {
        if (i > 0) Luau::PushOutput("\t", 1);

        TValue* val = L->base + i;
        int tag = val->tag;

        if (tag == 0) {
            TString* strObj = (TString*)(val->value.gc);
            if (strObj && strObj->len > 0 && strObj->len < 4096) {
                Luau::PushOutput(strObj->data, strObj->len);
            }
        } else {
            Luau::PushOutput("[", 1);
            Luau::PushOutputInt(tag);
            Luau::PushOutput("]", 1);
        }
    }

    if (g_OriginalPrint) {
        using PrintFn = int(*)(CosmicState*);
        ((PrintFn)g_OriginalPrint)(L);
    }
    return 0;
}

static int Hk_Print(CosmicState* L) {
    __try { return Hk_Print_L(L); }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        Luau::PushOutput("[hook error]", 12);
    }
    return 0;
}

static bool Install() {
    uintptr_t printAddr = Functions::luaB_print;
    if (!printAddr) return false;

    DWORD oldProtect;
    if (!VirtualProtect((LPVOID)printAddr, 16, PAGE_EXECUTE_READWRITE, &oldProtect))
        return false;

    g_OriginalPrint = printAddr;
    memcpy(g_OriginalPrintBytes, (void*)printAddr, 16);

    uintptr_t hookAddr = (uintptr_t)&Hk_Print;
    int32_t relOffset = (int32_t)(hookAddr - (printAddr + 5));

    uint8_t jmpStub[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
    memcpy(jmpStub + 1, &relOffset, 4);
    memcpy((void*)printAddr, jmpStub, sizeof(jmpStub));

    DWORD restore;
    VirtualProtect((LPVOID)printAddr, 16, oldProtect, &restore);

    g_PrintHooked = true;
    return true;
}

static void Remove() {
    if (!g_PrintHooked || !g_OriginalPrint) return;
    DWORD oldProtect;
    VirtualProtect((LPVOID)g_OriginalPrint, 16, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)g_OriginalPrint, g_OriginalPrintBytes, 16);
    DWORD restore;
    VirtualProtect((LPVOID)g_OriginalPrint, 16, oldProtect, &restore);
    g_PrintHooked = false;
}

}

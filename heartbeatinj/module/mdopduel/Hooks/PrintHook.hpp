#pragma once
#include <Windows.h>
#include <cstdint>
#include <cstring>
#include <string>
#include "../Luau/Offsets.hpp"
#include "../Luau/Executor.hpp"

namespace Hooks {

static uintptr_t g_OriginalPrint = 0;
static uint8_t g_OriginalPrintBytes[16] = { 0 };
static bool g_PrintHooked = false;

static int Hk_Print_L(CosmicState* L) {
    int nargs = 0;
    StkId base = L->base;
    StkId top = L->top;
    nargs = (int)(top - base);

    std::string output;
    for (int i = 1; i < nargs; i++) {
        if (i > 1) output += "\t";

        TValue* val = L->base + (i - 1);
        int tag = val->tag;

        if (tag == 0) {
            auto strObj = val->value.gc ? (TString*)(val->value.gc) : nullptr;
            if (strObj && strObj->len > 0 && strObj->len < 4096) {
                output.append(strObj->data, strObj->len);
            }
        } else {
            char buf[64];
            sprintf_s(buf, "[type=%d]", tag);
            output += buf;
        }
    }

    Luau::PushOutput(output.c_str(), output.size());

    if (g_OriginalPrint) {
        using PrintFn = int(*)(CosmicState*);
        ((PrintFn)g_OriginalPrint)(L);
    }

    return 0;
}

static int Hk_Print(CosmicState* L) {
    int result = 0;
    __try {
        result = Hk_Print_L(L);
    }
    __except (EXCEPTION_EXECUTE_HANDLER) {
        Luau::PushOutput("[Error: exception in print hook]", 33);
    }
    return result;
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

    uint8_t jmpStub[] = {
        0xE9, 0x00, 0x00, 0x00, 0x00
    };
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

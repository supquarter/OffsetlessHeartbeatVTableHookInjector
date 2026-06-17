#pragma once
#include <Windows.h>
#include <cstdint>

#define REBASE(x) (x + (uintptr_t)GetModuleHandle(NULL))

namespace Offsets {
    namespace Instance {
        constexpr uintptr_t ClassDescriptor = 0x18;
        constexpr uintptr_t Children = 0x78;
        constexpr uintptr_t Parent = 0x70;
        constexpr uintptr_t Name = 0xB0;
        constexpr uintptr_t ScriptContext = 0x3F0;
        constexpr uintptr_t globalStatesMap = 0x150;
        constexpr uintptr_t loadedModules = 0x480;
    }

    namespace ClassDescriptor {
        constexpr uintptr_t Name = 0x08;
    }

    namespace CosmicState {
        constexpr uintptr_t global = 0x28;
        constexpr uintptr_t top = 0x38;
        constexpr uintptr_t base = 0x30;
        constexpr uintptr_t ci = 0x48;
        constexpr uintptr_t stack = 0x40;
        constexpr uintptr_t gt = 0x60;
        constexpr uintptr_t userdata = 0x68;
        constexpr uintptr_t stacksize = 0x78;
    }

    namespace RobloxExtraSpace {
        constexpr uintptr_t shared = 0x18;
        constexpr uintptr_t next = 0x00;
        constexpr uintptr_t prev = 0x10;
        constexpr uintptr_t identity = 0x30;
        constexpr uintptr_t capabilities = 0x58;
    }

    namespace global_State {
        constexpr uintptr_t cb = 0x4E0;
        constexpr uintptr_t ptrenckey = 0x538;
        constexpr uintptr_t mainthread = 0x70;
        constexpr uintptr_t strt = 0x50;
    }
}

namespace Functions {
    const uintptr_t GetLuaState           = REBASE(0x1CB0530);
    const uintptr_t GetGlobalState        = REBASE(0x1CB00D0);
    const uintptr_t LuaVMLoad             = REBASE(0x1CC8D60);
    const uintptr_t loadsafe              = REBASE(0x453F030);
    const uintptr_t luau_execute          = REBASE(0x454ACD0);
    const uintptr_t task_spawn            = REBASE(0x1DE4170);
    const uintptr_t task_defer            = REBASE(0x1DE3270);
    const uintptr_t task_wait             = REBASE(0x1DE44E0);
    const uintptr_t luaB_print            = REBASE(0x8EFC70);
    const uintptr_t lua_pushfstringL      = REBASE(0x4534100);
    const uintptr_t lua_pushcclosurek     = REBASE(0x4533FD0);
    const uintptr_t lua_pushvfstring      = REBASE(0x4534810);
    const uintptr_t lua_newstate          = REBASE(0x453A580);
    const uintptr_t lua_encodepointer     = REBASE(0x4532DF0);
    const uintptr_t luaF_newLclosure      = REBASE(0x45633C0);
    const uintptr_t luaF_newproto         = REBASE(0x4563470);
    const uintptr_t luaF_newCclosure      = REBASE(0x4563330);
    const uintptr_t luaH_new              = REBASE(0x455BF10);
    const uintptr_t luaM_newgco_          = REBASE(0x4559D70);
    const uintptr_t luaD_callint          = REBASE(0x4536E40);
    const uintptr_t luaD_pcall            = REBASE(0x4537430);
    const uintptr_t luaD_rawrunprotected  = REBASE(0x45376C0);
    const uintptr_t luaD_throw            = REBASE(0x4537A00);
    const uintptr_t luaL_errorL           = REBASE(0x4538E00);
    const uintptr_t luaL_argerrorL        = REBASE(0x4538A30);
    const uintptr_t luaV_gettable         = REBASE(0x4565050);
    const uintptr_t luaV_settable         = REBASE(0x4565690);
    const uintptr_t lua_resetthread       = REBASE(0x453B3F0);
    const uintptr_t lua_rawcheckstack     = REBASE(0x4534870);
    const uintptr_t lua_yield             = REBASE(0x454E0C0);
    const uintptr_t lua_namecallatom      = REBASE(0x4533A40);
    const uintptr_t luaT_objtypename      = REBASE(0x455A4A0);
    const uintptr_t f_luaopen             = REBASE(0x453A000);
    const uintptr_t close_state           = REBASE(0x4539F40);
    const uintptr_t PropagateMark         = REBASE(0x453DDB0);
    const uintptr_t Atomic                = REBASE(0x453CA60);
    const uintptr_t Print                 = REBASE(0x1E08380);
    const uintptr_t PushInstance          = REBASE(0x1CA3840);
    const uintptr_t GetPropertyData       = REBASE(0x1D4A080);
    const uintptr_t FireTouchInterest     = REBASE(0x29BDAC0);
    const uintptr_t ScriptContextResume   = REBASE(0x1D79260);
    const uintptr_t GetCapabilities       = REBASE(0x45AB650);
    const uintptr_t GetAppDataInfo        = REBASE(0x35E7BC0);
    const uintptr_t AttachRobloxExtraSpace= REBASE(0x1CDECA0);
    const uintptr_t Pushcclosure          = REBASE(0x4533FD0);
}

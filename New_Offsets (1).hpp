//Dumped at      : 2026-06-10 02:28:45
//Dump duration  : 5.66 seconds

//Roblox version : version-76173e47a79145c7
//Roblox         : 0.725.0.7251138
//This Dumper made by Versual
//(these are not checked)
//VannaRy Dumper

#pragma once
#include <cstdint>

#define REBASE(x) (x)
#define Hyperion_Rbase(x) (x)

namespace Offsets {
    namespace Internal_Offsets {
        const uintptr_t currfuncname = REBASE(0x4538520);
        const uintptr_t lua_newstate = REBASE(0x453A580);
        const uintptr_t pseudo2addr = REBASE(0x45368A0);
        const uintptr_t IdentityToCaps = REBASE(0x45AB650);
        const uintptr_t atomic = REBASE(0x453CA60);
        const uintptr_t ScriptContextResume = REBASE(0x1D79260);
        const uintptr_t AppdataInfo = REBASE(0x35E7BC0);
        const uintptr_t markroot = REBASE(0x453DB30);
        const uintptr_t pseudoaddr = REBASE(0x45368A0);
        const uintptr_t Print = REBASE(0x1E08380);
        const uintptr_t std_runtime_error = REBASE(0x49B8A10);
        const uintptr_t GetGlobalState = REBASE(0x1CB00D0);
        const uintptr_t GetLuaStateForInstance = REBASE(0x1C821B0);
        const uintptr_t GetCapabilites = REBASE(0x3011540);
        const uintptr_t GetLuaState = REBASE(0x1DAC870);
        const uintptr_t GetIdentityStruct = REBASE(0x45AB200);
        const uintptr_t GetFastFlag = REBASE(0x6858B30);
        const uintptr_t GetValues = REBASE(0x1CAA710);
        const uintptr_t GetPropertyData = REBASE(0x1D4A080);
        const uintptr_t Pushcclosure = REBASE(0x4533FD0);
        const uintptr_t Pushuserdata = REBASE(0x4533D00);
        const uintptr_t RawScheduler = REBASE(0x1893E4F);
        const uintptr_t TaskSchedulerTargetFps = REBASE(0x47E04A0);
        const uintptr_t TaskSynchronize = REBASE(0x1DE42F0);
        const uintptr_t TaskDefer = REBASE(0x1DE3270);
        const uintptr_t PushInstance = REBASE(0x1CA3840);
        const uintptr_t PushInstance_2 = REBASE(0x1CA3880);
        const uintptr_t KTable = REBASE(0x7FCCAC0);
        const uintptr_t FireTouchInterest = REBASE(0x29D9ACB);
        const uintptr_t IdentityToCaps = REBASE(0x45AB650);
        const uintptr_t callerrfunc = REBASE(0x45369F0);
        const uintptr_t auxopen = REBASE(0x3C38790);
        const uintptr_t freeudata = REBASE(0x455B770);
        const uintptr_t coyield = REBASE(0x454E0C0);
        const uintptr_t newclasspage = REBASE(0x455A150);
        const uintptr_t newgcoblock = REBASE(0x4559D70);
        const uintptr_t newpage = REBASE(0x455A250);
        const uintptr_t gcstep = REBASE(0x453D110);
        const uintptr_t f_luaopen = REBASE(0x453A000);
        const uintptr_t traversetable = REBASE(0x453E9C0);
        const uintptr_t loadsafe = REBASE(0x453F030);
        const uintptr_t close_state = REBASE(0x4539F40);
        const uintptr_t currentline = REBASE(0x1E2A803);
        const uintptr_t pusherror = REBASE(0x4537980);
        const uintptr_t tag_error = REBASE(0x4539F10);
        const uintptr_t propagetemark = REBASE(0x453DDB0);
        const uintptr_t compress = REBASE(0x4A45F50);
        const uintptr_t cocreate = REBASE(0x454E020);
        const uintptr_t coclose = REBASE(0x454E140);
        const uintptr_t lengthFieldBasedFrameDecoder = REBASE(0x6AD07C8);
        const uintptr_t AttachRobloxExtraSpace = REBASE(0x1CDECA0);
        const uintptr_t task_spawn = REBASE(0x1DE4170);
        const uintptr_t task_cancel = REBASE(0x1DE2F90);
        const uintptr_t task_delay = REBASE(0x1DE36A0);
        const uintptr_t task_defer = REBASE(0x1DE3270);
        const uintptr_t task_desynchronize = REBASE(0x1DE38E0);
        const uintptr_t task_wait = REBASE(0x1DE44E0);
    }

    namespace Internal_Xref_Offsets {
    }

    namespace Dll_Offsets {
        const uintptr_t ControlFlowGuard = 0xF9BCE0;
        const uintptr_t BitMap = 0x3C548;
        const uintptr_t ByteShift = 0xF;
        const uintptr_t PageShift = 0xC;
        const uintptr_t BitMask = 0x7;
        const uintptr_t PageSize = 0x1000;
        const uintptr_t PageMask = 0xFFF;
    }

    namespace If_needed {
        constexpr uintptr_t ConnectionDisconnect = 0x1CB7DB0;
        constexpr uintptr_t IsLegalSendEvent = 0xA18810;
    }

    namespace Lua_Offsets {
        const uintptr_t Luau_Execute = REBASE(0x454ACD0);
        const uintptr_t LuaA_toobject = REBASE(0x4532880);
        const uintptr_t LuaD_Throw = REBASE(0x4537A00);
        const uintptr_t luaB_print = REBASE(0x8EFC70);
        const uintptr_t luaM_free = REBASE(0x4559AC0);
        const uintptr_t luaM_freegco = REBASE(0x4559B40);
        const uintptr_t luaM_visitgco = REBASE(0x4559FF0);
        const uintptr_t luaM_toobig = REBASE(0x453BFE0);
        const uintptr_t lua_newstate_path = REBASE(0x453A580);
        const uintptr_t luaF_newUpval = REBASE(0x4563010);
        const uintptr_t luaF_newtable = REBASE(0x455B610);
        // const uintptr_t luaF_freeUpval = 0x0;
        // const uintptr_t luaF_freethread = 0x0;
        const uintptr_t luaM_freearray = REBASE(0x4559AC0);
        const uintptr_t luaS_newlstr = REBASE(0x456A0D0);
        const uintptr_t luaF_freeproto = REBASE(0x453A240);
        const uintptr_t luaG_runerrorL = REBASE(0x1E08380);
        const uintptr_t luaF_newCclosure = REBASE(0x4563330);
        const uintptr_t luaF_newproto = REBASE(0x4563470);
        const uintptr_t luaO_str2d = REBASE(0x4569890);
        const uintptr_t luaD_rawunprotected = REBASE(0x45376C0);
        const uintptr_t luaL_errorL = REBASE(0x4538E00);
        const uintptr_t luaopen_os = REBASE(0x45509A0);
        const uintptr_t LuaF_Newproto = REBASE(0x4563470);
        const uintptr_t Luau_Load = REBASE(0xF4B245);
        const uintptr_t LuaVM_Load = REBASE(0xB54210);
        const uintptr_t LuaVM_Load_2 = REBASE(0x1CC8D60);
        const uintptr_t Lua_Load = REBASE(0x453ED80);
        const uintptr_t luaB_assert = REBASE(0x454D640);
        const uintptr_t luaC_Step = REBASE(0x453D620);
        const uintptr_t luaT_objtypename = REBASE(0x455A4A0);
        const uintptr_t luaO_chunkid = REBASE(0x45693C0);
        const uintptr_t luaopen_base = REBASE(0x454DE30);
        const uintptr_t luaopen_math = REBASE(0x45571F0);
        const uintptr_t luaH_clone = REBASE(0x455B610);
        const uintptr_t luaV_gettable = REBASE(0x4565050);
        const uintptr_t luaL_register = REBASE(0x45394D0);
        const uintptr_t luaL_findtable = REBASE(0x4538E50);
        const uintptr_t luaL_checklstring = REBASE(0x45391B0);
        const uintptr_t luaL_argerrorL = REBASE(0x4538A30);
        const uintptr_t luaL_typerrorL = REBASE(0x4539D20);
        const uintptr_t luaH_new = REBASE(0x455BF10);
        const uintptr_t lua_pushfstringL = REBASE(0x4534100);
        const uintptr_t lua_pushvfstring = REBASE(0x4534810);
        const uintptr_t luaF_newLclosure = REBASE(0x45633C0);
        const uintptr_t luaB_error = REBASE(0x454D100);
        const uintptr_t luaB_gcinfo = REBASE(0x454D530);
        const uintptr_t luaB_getfenv = REBASE(0x454D290);
        const uintptr_t luaB_getmetatable = REBASE(0x454D170);
        const uintptr_t luaB_net = REBASE(0x454D5E0);
        const uintptr_t luaB_newproy = REBASE(0x454D790);
        const uintptr_t luaB_pcallrun = REBASE(0x454DBE0);
        const uintptr_t luaB_rawequal = REBASE(0x454D3D0);
        const uintptr_t luaB_rawget = REBASE(0x454D420);
        const uintptr_t luaB_rawset = REBASE(0x454D470);
        const uintptr_t luaB_rawlen = REBASE(0x454D4D0);
        const uintptr_t luaB_select = REBASE(0x454D6B0);
        const uintptr_t luaB_setfenv = REBASE(0x454D2F0);
        const uintptr_t luaB_setmetatable = REBASE(0x454D1D0);
        const uintptr_t luaB_tonumber = REBASE(0x45593C0);
        const uintptr_t luaB_tostring = REBASE(0x454D760);
        const uintptr_t luaB_type = REBASE(0x454D560);
        const uintptr_t luaB_typeof = REBASE(0x454D5A0);
        const uintptr_t luaB_pcally = REBASE(0x454DAF0);
        const uintptr_t luaB_pcally_alt = REBASE(0x1D3D6B0);
        const uintptr_t luaB_pcallrun = REBASE(0x454DAB0);
        const uintptr_t lua_rawcheckstack = REBASE(0x4534870);
        const uintptr_t lua_resetthread = REBASE(0x1C98860);
        const uintptr_t lua_yield = REBASE(0x454E0C0);
        const uintptr_t luaV_settable = REBASE(0x4565690);
        const uintptr_t luaV_lessthan = REBASE(0x4565900);
        const uintptr_t luaV_equal = REBASE(0x4564E90);
        const uintptr_t lua_encodepointer = REBASE(0x4532DF0);
        const uintptr_t luaS_free = REBASE(0x456A020);
        const uintptr_t lua_namecallatom = REBASE(0x4533A40);
        const uintptr_t lua_newstate = REBASE(0x453A580);
        const uintptr_t lua_eception = REBASE(0x45369D0);
        const uintptr_t luaT_init = REBASE(0x455A3C0);
    }

    namespace Raknet_Offsets {
        // const uintptr_t RakNet_Send = 0x0;
        // const uintptr_t RakNet_ProcessNetworkPacket = 0x0;
        const uintptr_t RakNet_ReportNetworkError = REBASE(0x6375B0);
    }

    namespace System {
        const uintptr_t Cache = 0xE8;
        const uintptr_t LRUCache = 0x20;
        const uintptr_t MeshData = 0x40;
        const uintptr_t AssetID = 0x10;
    }
}

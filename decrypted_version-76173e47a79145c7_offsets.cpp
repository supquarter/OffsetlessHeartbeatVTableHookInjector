// Roblox Version: decrypted_version-76173e47a79145c7
#pragma once

#include <Windows.h>

#include <cstdint>

#define REBASE(x) (x + (uintptr_t)GetModuleHandle(NULL))

namespace Offsets {
    const uintptr_t Children = 0x78;
    const uintptr_t ClassDescriptor = 0x18;
    const uintptr_t DescriptorName = 0x8;
    const uintptr_t FakeDataModelToReal = 0x1D8;
    const uintptr_t JobName = 0x18;
    const uintptr_t Name = 0xB0;
    const uintptr_t Overlap = 0x1F0;
    const uintptr_t Parent = 0x70;
    const uintptr_t Primitive = 0x148;
    const uintptr_t RakPeer = 0x1D0;
    const uintptr_t RenderJobToView = 0xA0;
    const uintptr_t RenderViewToDevice = 0x48;
    const uintptr_t RequireFlag = 0x851;
    const uintptr_t ScriptContext = 0x3F0;
    const uintptr_t VisualEngineToDevice = 0x98;
    const uintptr_t actorPool = 0x170;
    const uintptr_t decryptKey = 0x198;
    const uintptr_t globalStatesMap = 0x150;
    const uintptr_t loadedModules = 0x480;
    const uintptr_t scriptContextResumeFacet = 0x7F8;
}

namespace Structs {
    const uintptr_t AppDataInfo = REBASE(0x7F31380);
    const uintptr_t FakeDataModel = REBASE(0x7A39AD8);
    const uintptr_t IdentityPtr = REBASE(0x7FBD4A0);
    const uintptr_t KTable = REBASE(0x7FCCAC0);
    const uintptr_t OpcodeLookupTable = REBASE(0x6056C90);
    const uintptr_t RawScheduler = REBASE(0x7FCB088);
    const uintptr_t luaH_dummynode = REBASE(0x67AE2E8);
    const uintptr_t luaO_nilobject = REBASE(0x67AE440);
}

namespace Functions {
    const uintptr_t DisconnectConnection = REBASE(0x2FA7DC0);
    const uintptr_t FireLeftMouseClick = REBASE(0x2566BF0);
    const uintptr_t FireMouseHoverEnter = REBASE(0x2566E00);
    const uintptr_t FireMouseHoverLeave = REBASE(0x2566FF0);
    const uintptr_t FireProximityPrompt = REBASE(0x74C500);
    const uintptr_t FireRightMouseClick = REBASE(0x2567260);
    const uintptr_t FireTouchInterest = REBASE(0x29BDAC0);
    const uintptr_t GetAppDataInfo = REBASE(0x35E7BC0);
    const uintptr_t GetCapabilities = REBASE(0x45AB650);
    const uintptr_t GetIdentityStruct = REBASE(0x82D0);
    const uintptr_t GetLuaState = REBASE(0x1CB0530);
    const uintptr_t GetValues = REBASE(0x1C91ED0);
    const uintptr_t LuaVMLoad = REBASE(0x1CC8D60);
    const uintptr_t Print = REBASE(0x1E08380);
    const uintptr_t PushInstance = REBASE(0x1CA3840);
    const uintptr_t PushInstance2 = REBASE(0x1CA3880);
    const uintptr_t ScriptContextResume = REBASE(0x1D79260);
    const uintptr_t TaskCancel = REBASE(0x1DE2F90);
    const uintptr_t TaskDefer = REBASE(0x1DE3270);
    const uintptr_t TaskDelay = REBASE(0x1DE36A0);
    const uintptr_t TaskDesynchronize = REBASE(0x1DE38E0);
    const uintptr_t TaskSpawn = REBASE(0x1DE4170);
    const uintptr_t TaskSynchronize = REBASE(0x1DE42F0);
    const uintptr_t TaskWait = REBASE(0x1DE44E0);
    const uintptr_t atomic = REBASE(0x453CA60);
    const uintptr_t callerrfunc = REBASE(0x45369F0);
    const uintptr_t close_state = REBASE(0x4539F40);
    const uintptr_t coclose = REBASE(0x454E140);
    const uintptr_t cocreate = REBASE(0x454E020);
    const uintptr_t coisyieldable = REBASE(0x454E110);
    const uintptr_t coresumey = REBASE(0x454E700);
    const uintptr_t corunning = REBASE(0x454E0E0);
    const uintptr_t costatus = REBASE(0x454DFB0);
    const uintptr_t cowrap = REBASE(0x454E060);
    const uintptr_t coyield = REBASE(0x454E0C0);
    const uintptr_t currentline = REBASE(0x453B4E0);
    const uintptr_t currfuncname = REBASE(0x4538520);
    const uintptr_t dumpobj = REBASE(0x454B0F0);
    const uintptr_t dumpthread = REBASE(0x454B9C0);
    const uintptr_t enumgco = REBASE(0x454C1D0);
    const uintptr_t enumobj = REBASE(0x454C1F0);
    const uintptr_t enumtable = REBASE(0x454C7D0);
    const uintptr_t f_luaopen = REBASE(0x453A000);
    const uintptr_t gcstep = REBASE(0x453D110);
    const uintptr_t isLegalSendEvent = REBASE(0xA18810);
    const uintptr_t loadsafe = REBASE(0x453F030);
    const uintptr_t luaA_toobject = REBASE(0x4532880);
    const uintptr_t luaB_pcallrun = REBASE(0x454DAB0);
    const uintptr_t luaB_pcally = REBASE(0x454DAF0);
    const uintptr_t luaB_xpcally = REBASE(0x454DD10);
    const uintptr_t luaC_step = REBASE(0x453D620);
    const uintptr_t luaD_callint = REBASE(0x4536E40);
    const uintptr_t luaD_pcall = REBASE(0x4537430);
    const uintptr_t luaD_rawrunprotected = REBASE(0x45376C0);
    const uintptr_t luaD_throw = REBASE(0x4537A00);
    const uintptr_t luaF_close = REBASE(0x4562EE0);
    const uintptr_t luaF_freeproto = REBASE(0x4563110);
    const uintptr_t luaF_newLclosure = REBASE(0x45633C0);
    const uintptr_t luaF_newproto = REBASE(0x4563470);
    const uintptr_t luaG_runerrorL = REBASE(0x453C010);
    const uintptr_t luaH_clone = REBASE(0x455B610);
    const uintptr_t luaH_getstr = REBASE(0x455BEA0);
    const uintptr_t luaH_new = REBASE(0x455BF10);
    const uintptr_t luaL_argerrorL = REBASE(0x4538A30);
    const uintptr_t luaL_callmeta = REBASE(0x4DBE7C0);
    const uintptr_t luaL_errorL = REBASE(0x3D521E0);
    const uintptr_t luaL_getmetafield = REBASE(0x4538F80);
    const uintptr_t luaL_register = REBASE(0x45394D0);
    const uintptr_t luaL_tolstring = REBASE(0x4539610);
    const uintptr_t luaL_typerrorL = REBASE(0x4539D20);
    const uintptr_t luaL_where = REBASE(0x3D52420);
    const uintptr_t luaM_free_ = REBASE(0x4559AC0);
    const uintptr_t luaM_freegco_ = REBASE(0x4559B40);
    const uintptr_t luaM_new_ = REBASE(0x4559CB0);
    const uintptr_t luaM_newgco_ = REBASE(0x4559D70);
    const uintptr_t luaM_visitgco = REBASE(0x4559FF0);
    const uintptr_t luaO_chunkid = REBASE(0x45693C0);
    const uintptr_t luaO_pushfstring = REBASE(0x4569580);
    const uintptr_t luaO_pushvfstring = REBASE(0x650D10);
    const uintptr_t luaS_free = REBASE(0x456A020);
    const uintptr_t luaT_init = REBASE(0x455A3C0);
    const uintptr_t luaT_objtypename = REBASE(0x455A4A0);
    const uintptr_t luaV_gettable = REBASE(0x4565050);
    const uintptr_t lua_clock = REBASE(0x51B6538);
    const uintptr_t lua_encodepointer = REBASE(0x4532DF0);
    const uintptr_t lua_getthreaddata = REBASE(0x1210930);
    const uintptr_t lua_namecallatom = REBASE(0x4533A40);
    const uintptr_t lua_newstate = REBASE(0x453A580);
    const uintptr_t lua_newthread = REBASE(0x4533B70);
    const uintptr_t lua_pushcclosurek = REBASE(0x4533FD0);
    const uintptr_t lua_pushfstringL = REBASE(0x4534100);
    const uintptr_t lua_pushvfstring = REBASE(0x499ED30);
    const uintptr_t lua_rawcheckstack = REBASE(0x4534870);
    const uintptr_t lua_resetthread = REBASE(0x453B3F0);
    const uintptr_t lua_typename = REBASE(0x45365B0);
    const uintptr_t luaopen_base = REBASE(0x454DE30);
    const uintptr_t luaopen_bit32 = REBASE(0x4554740);
    const uintptr_t luaopen_buffer = REBASE(0x45557E0);
    const uintptr_t luaopen_coroutine = REBASE(0x454E7B0);
    const uintptr_t luaopen_debug = REBASE(0x4557B50);
    const uintptr_t luaopen_math = REBASE(0x45571F0);
    const uintptr_t luaopen_os = REBASE(0x45509A0);
    const uintptr_t luaopen_string = REBASE(0x4552F80);
    const uintptr_t luaopen_table = REBASE(0x454F770);
    const uintptr_t luaopen_utf8 = REBASE(0x4556100);
    const uintptr_t luaopen_vector = REBASE(0x4558500);
    const uintptr_t luau_execute = REBASE(0x454ACD0);
    const uintptr_t luau_precall = REBASE(0x454ADF0);
    const uintptr_t markmt = REBASE(0x453D9F0);
    const uintptr_t markroot = REBASE(0x453DB30);
    const uintptr_t newblock = REBASE(0x455A0C0);
    const uintptr_t newgcoblock = REBASE(0x455A1C0);
    const uintptr_t newpage = REBASE(0x455A250);
    const uintptr_t propagatemark = REBASE(0x453DDB0);
    const uintptr_t pseudo2addr = REBASE(0x45368A0);
    const uintptr_t pusherror = REBASE(0x453C9B0);
    const uintptr_t resume_handle = REBASE(0x45382A0);
    const uintptr_t tag_error = REBASE(0x4539F10);
    const uintptr_t traversetable = REBASE(0x453E9C0);
    const uintptr_t vsnprintf = REBASE(0x75C4C0);
}
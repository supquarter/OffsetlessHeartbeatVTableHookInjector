#include <iostream>
#include <vector>
#include <filesystem>
#include "Memory/Memory.hpp"
#include "Mapper/Mapper.hpp"
#include "Defs.hpp"

uintptr_t Hook(uintptr_t a1, uintptr_t a2, uintptr_t a3) {
    auto s = (Shared*)0x100000000;
    if (s->Status == State::Load) {
        s->Status = State::Wait;
        char m[] = { 'm', 's', 'h', 't', 'm', 'l', '.', 'd', 'l', 'l', '\0' };
        s->LdrEx(m, NULL, DONT_RESOLVE_DLL_REFERENCES);
    }
    if (s->Status == State::Inject) {
        auto d = s->dllSt;
        auto n = (PIMAGE_NT_HEADERS)((BYTE*)d + ((PIMAGE_DOS_HEADER)d)->e_lfanew);
        auto& e = n->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXCEPTION];
        if (e.Size)
            s->AddTab((PRUNTIME_FUNCTION)((BYTE*)d + e.VirtualAddress), e.Size / sizeof(RUNTIME_FUNCTION), (DWORD64)d);

        auto i = (PIMAGE_IMPORT_DESCRIPTOR)(d + s->ImpVA);
        auto ie = (PIMAGE_IMPORT_DESCRIPTOR)((uint8_t*)i + s->ImpSz);
        while (i < ie && i->Name) {
            HMODULE l = s->Ldr((char*)(d + i->Name));
            if (!l) {
                ++i;
                continue;
            }
            uintptr_t* t = (uintptr_t*)(d + (i->OriginalFirstThunk ? i->OriginalFirstThunk : i->FirstThunk));
            FARPROC* f = (FARPROC*)(d + i->FirstThunk);
            for (; *t; ++t, ++f) {
                if (IMAGE_SNAP_BY_ORDINAL(*t))
                    *f = s->Proc(l, MAKEINTRESOURCEA(IMAGE_ORDINAL(*t)));
                else
                    *f = s->Proc(l, ((IMAGE_IMPORT_BY_NAME*)(d + *t))->Name);
            }
            ++i;
        }

        if (s->TlsVA && s->TlsSz) {
            auto t = (IMAGE_TLS_DIRECTORY64*)(d + s->TlsVA);
            ULONGLONG rv = t->AddressOfCallBacks;
            if (rv) {
                uintptr_t c = (uintptr_t)rv;
                if (c < d || c >= s->dllEd)
                    c = d + (uintptr_t)rv;
                PIMAGE_TLS_CALLBACK* cl = (PIMAGE_TLS_CALLBACK*)c;
                for (size_t i = 0;; ++i) {
                    if (!cl[i]) break;
                    cl[i]((PVOID)d, DLL_PROCESS_ATTACH, nullptr);
                }
            }
        }
        ((BOOL(__stdcall*)(HMODULE, DWORD, LPVOID))(s->dllEp))((HMODULE)d, DLL_PROCESS_ATTACH, nullptr);
        s->Status = State::Done;
    }
    return s->OrgHook(a1, a2, a3);
}

bool IsReadablePtr(HANDLE h, uintptr_t addr) {
    if (addr < 0x10000 || addr >= 0x7FFFFFFFFFFF) return false;
    MEMORY_BASIC_INFORMATION mbi;
    if (!VirtualQueryEx(h, (LPCVOID)addr, &mbi, sizeof(mbi))) return false;
    return mbi.State == MEM_COMMIT && !(mbi.Protect & PAGE_GUARD) &&
        (mbi.Protect & (PAGE_READONLY | PAGE_READWRITE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE));
}

uintptr_t FindPlayerListManager(HANDLE process_handle, uintptr_t rbBase, uintptr_t rbSize) {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);

    uintptr_t start_address = (uintptr_t)sys_info.lpMinimumApplicationAddress;
    uintptr_t end_address = (uintptr_t)sys_info.lpMaximumApplicationAddress;

    MEMORY_BASIC_INFORMATION mbi;

    printf("[*] Scanning memory for PlayerListManager (class name 'Players')...\n");
    int regionsChecked = 0;

    while (start_address < end_address) {
        if (VirtualQueryEx(process_handle, (LPCVOID)start_address, &mbi, sizeof(mbi))) {
            if ((mbi.State == MEM_COMMIT) && (mbi.Protect & (PAGE_READWRITE | PAGE_EXECUTE_READWRITE)) && !(mbi.Protect & PAGE_GUARD)) {
                size_t region_size = mbi.RegionSize;
                std::vector<BYTE> buffer(region_size);

                if (g_Syscall.SyscallRead(process_handle, start_address, buffer.data(), region_size)) {
                    for (size_t i = 0; i < region_size - 0x80; i += 8) {
                        uintptr_t inst = *reinterpret_cast<uintptr_t*>(&buffer[i]);

                        if (inst < 0x10000 || inst >= 0x7FFFFFFFFFFF) continue;

                        uintptr_t classDesc = 0;
                        if (!g_Syscall.SyscallRead(process_handle, inst + 0x18, &classDesc, sizeof(classDesc)))
                            continue;

                        if (classDesc < 0x10000 || classDesc >= 0x7FFFFFFFFFFF) continue;

                        char className[32] = { 0 };
                        if (!g_Syscall.SyscallRead(process_handle, classDesc + 0x08, className, sizeof(className)))
                            continue;

                        if (strcmp(className, "Players") != 0) continue;

                        uintptr_t parent = 0;
                        g_Syscall.SyscallRead(process_handle, inst + 0x70, &parent, sizeof(parent));
                        if (parent >= 0x10000 && parent < 0x7FFFFFFFFFFF) {
                            uintptr_t parentCD = 0;
                            if (g_Syscall.SyscallRead(process_handle, parent + 0x18, &parentCD, sizeof(parentCD))) {
                                char parentName[32] = { 0 };
                                if (g_Syscall.SyscallRead(process_handle, parentCD + 0x08, parentName, sizeof(parentName))) {
                                    if (strcmp(parentName, "DataModel") == 0) {
                                        printf("[+] Found PlayerListManager at 0x%p (parent: DataModel)\n", (void*)inst);
                                        return inst;
                                    }
                                }
                            }
                        }

                        printf("[*] Found 'Players' instance at 0x%p (parent not DataModel, checking anyway)\n", (void*)inst);
                        return inst;
                    }
                }
                regionsChecked++;
                if (regionsChecked % 50 == 0) {
                    printf("[*] Scanned %d regions...\n", regionsChecked);
                }
            }
            start_address += mbi.RegionSize;
        } else {
            start_address += 0x1000;
        }
    }

    printf("[-] Could not find PlayerListManager after scanning %d regions.\n", regionsChecked);
    return 0;
}

unsigned long g_pid = 0;
unsigned long g_old = 0;
void* g_process = nullptr;

int main() {
    g_DllPath = (std::filesystem::current_path() / "module.dll").string();
    if (!std::filesystem::exists(g_DllPath)) {
        printf("[-] module.dll not found at %s\n", g_DllPath.c_str());
        return 1;
    }

    g_pid = GetPid("RobloxPlayerBeta.exe");
    if (!g_pid) {
        printf("[-] RobloxPlayerBeta.exe not found.\n");
        return 1;
    }

    if (!ensure_proc_handle()) {
        printf("[-] No valid process handle, exiting.\n");
        return 1;
    }

    printf("[+] Roblox PID: %u\n", g_pid);

    std::vector<MODULEENTRY32> mods;
    GetMods(g_pid, { "devenum.dll", "RobloxPlayerBeta.exe", "KERNELBASE.dll", "KERNEL32.dll", "ntdll.dll" }, mods);

    if (mods.size() < 5 || !mods[0].modBaseAddr || !mods[1].modBaseAddr) {
        printf("[-] Failed to enumerate modules.\n");
        return 1;
    }

    uintptr_t deBase = (uintptr_t)mods[0].modBaseAddr;
    uintptr_t rbBase = (uintptr_t)mods[1].modBaseAddr;
    uintptr_t rbSize = mods[1].modBaseSize;
    uintptr_t kbBase = (uintptr_t)mods[2].modBaseAddr;
    uintptr_t k3Base = (uintptr_t)mods[3].modBaseAddr;
    uintptr_t ntBase = (uintptr_t)mods[4].modBaseAddr;

    printf("[+] RobloxPlayerBeta.exe: 0x%p (size: 0x%X)\n", (void*)rbBase, rbSize);
    printf("[+] devenum.dll: 0x%p\n", (void*)deBase);

    uintptr_t pcmgrInst = FindPlayerListManager(g_process, rbBase, rbSize);

    if (!pcmgrInst) {
        printf("[-] Could not find PlayerListManager.\n");
        return 1;
    }

    uintptr_t oVab = Read<uintptr_t>(pcmgrInst);
    printf("[+] PlayerListManager VTable: 0x%p\n", (void*)oVab);

    uintptr_t oStep = Read<uintptr_t>(oVab + 8);
    printf("[+] Original step function: 0x%p\n", (void*)oStep);

    uintptr_t nVab = Alloc(0x300, PAGE_READWRITE);
    if (!nVab) {
        printf("[-] Failed to allocate fake vtable.\n");
        return 1;
    }

    for (uintptr_t i = 0; i < 0x300; i += 8) {
        Write<uintptr_t>(nVab + i, Read<uintptr_t>(oVab + i));
    }

    g_Shared = Alloc(sizeof(Shared), PAGE_READWRITE);
    if (!g_Shared) {
        printf("[-] Failed to allocate Shared struct.\n");
        return 1;
    }

    Prot(deBase, 0x1000, PAGE_EXECUTE_READWRITE);
    std::vector<BYTE> z(0x1000, 0);
    Write(deBase, z.data(), z.size());

    std::vector<BYTE> sc = ExtSc((uintptr_t)Hook);
    RepSc(sc, 0x100000000ULL, g_Shared);
    Write(deBase, sc.data(), sc.size());

    Write<uintptr_t>(nVab + 8, deBase);

    Shared loc = {};
    loc.OrgHook = (fHbk)oStep;
    loc.LdrEx = (fLdrEx)GetProc(kbBase, "LoadLibraryExA");
    loc.Ldr = (fLdr)GetProc(k3Base, "LoadLibraryA");
    loc.Proc = (fProc)GetProc(k3Base, "GetProcAddress");
    loc.AddTab = (fTab)GetProc(ntBase, "RtlAddFunctionTable");
    loc.Status = State::Load;
    loc.PcmgrInst = pcmgrInst;
    Write(g_Shared, &loc, sizeof(Shared));

    Write<uintptr_t>(pcmgrInst, nVab);
    printf("[+] PlayerListManager vtable redirected. Waiting for hook to fire...\n");

    MODULEENTRY32 msme = {};
    while (!msme.modBaseAddr) {
        msme = GetMod(g_pid, "mshtml.dll");
        Sleep(250);
    }
    uintptr_t msBase = (uintptr_t)msme.modBaseAddr;
    Prot(msBase, msme.modBaseSize, PAGE_EXECUTE_READWRITE);
    z = std::vector<BYTE>(msme.modBaseSize, 0);
    Write(msBase, z.data(), z.size());
    printf("[+] mshtml.dll at 0x%p, size 0x%X\n", (void*)msBase, msme.modBaseSize);

    g_DllBase = msBase;
    g_DllSz = DllSz(g_DllPath);
    SH(dllSt, g_DllBase);
    SH(dllEd, g_DllBase + g_DllSz);
    mapr::Map(g_DllPath);
    mapr::Inj();

    Write<uintptr_t>(pcmgrInst, oVab);
    printf("[+] Original vtable restored. Injection complete.\n");

    return 0;
}

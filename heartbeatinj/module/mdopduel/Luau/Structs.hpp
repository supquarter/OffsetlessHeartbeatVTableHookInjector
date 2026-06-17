#pragma once
#include <cstdint>

struct CosmicState;
struct lua_Debug;
struct lua_Page;

#define CommonHeader uint8_t memcat; uint8_t tt; uint8_t marked

#define CLOSURE_CONT_ENC(type)      type
#define CLOSURE_DEBUGNAME_ENC(type) type
#define LSTATE_STACKSIZE_ENC(type)  type
#define PROTO_ABSLINEINFO_ENC(type) type
#define PROTO_DEBUGINSN_ENC(type)   type
#define PROTO_DEBUGNAME_ENC(type)   type
#define PROTO_LINEINFO_ENC(type)    type
#define PROTO_LOCVARS_ENC(type)     type
#define PROTO_SOURCE_ENC(type)      type
#define PROTO_TYPEINFO_ENC(type)    type
#define PROTO_UPVALUES_ENC(type)    type
#define PROTO_USERDATA_ENC(type)    type
#define TSTRING_HASH_ENC(type)      type
#define UDATA_META_ENC(type)        type

using StkId = struct TValue*;
using Instruction = unsigned int;
using lua_CFunction = int(*)(struct CosmicState*);
using lua_Continuation = int(*)(struct CosmicState*, int, long long);

struct GCObject {
    CommonHeader;
    GCObject* gclist;
};

struct TValue {
    union {
        void* gc;
        void* p;
        int b;
        double n;
        long long f;
        uint64_t u64;
    } value;
    int32_t extra;
    int32_t tag;
};

struct UpVal {
    CommonHeader;
    uint8_t open;
    uint8_t marked2;
    TValue* v;
    union {
        TValue value;
        struct { UpVal* prev; UpVal* next; };
    };
};

struct TString {
    CommonHeader;
    int16_t flag;
    int16_t atom;
    TString* next;
    unsigned int hash;
    unsigned int len;
    char data[1];
};

struct LocVar { TString* varname; int startpc; int endpc; };

struct Proto {
    CommonHeader;
    uint8_t maxstacksize;
    uint8_t numparams;
    uint8_t flags;
    uint8_t is_vararg;
    uint8_t nups;
    const Instruction* codeentry;
    struct Proto** p;
    TString* debugname;
    uint8_t* debuginsn;
    TString** upvalues;
    int* abslineinfo;
    void* execdata;
    uintptr_t exectarget;
    void* userdata;
    uint8_t* lineinfo;
    struct LocVar* locvars;
    uint8_t* typeinfo;
    TString* source;
    TValue* k;
    Instruction* code;
    GCObject* gclist;
    int sizek;
    int sizeupvalues;
    int sizep;
    int linedefined;
    int bytecodeid;
    int sizetypeinfo;
    int linegaplog2;
    int sizelineinfo;
    int sizecode;
    int sizelocvars;
    void* feedbackvec;
    uint32_t feedbackvecsize;
    uint32_t funid;
};

struct Closure {
    CommonHeader;
    uint8_t isC;
    uint8_t stacksize;
    uint8_t nupvalues;
    uint8_t preload;
    uint64_t usage;
    GCObject* gclist;
    struct LuaTable* env;
    union {
        struct {
            const char* debugname;
            lua_Continuation cont;
            lua_CFunction f;
            TValue upvals[1];
        } c;
        struct {
            Proto* p;
            TValue uprefs[1];
        } l;
    };
};

struct LuaNode { TValue val; TValue key; LuaNode* next; };

struct LuaTable {
    CommonHeader;
    uint8_t lsizenode;
    uint8_t safeenv;
    uint8_t nodemask8;
    uint8_t tmcache;
    uint8_t readonly;
    int sizearray;
    union { int lastfree; int aboundary; };
    struct LuaTable* metatable;
    TValue* array;
    GCObject* gclist;
    LuaNode* node;
};

struct CallInfo {
    StkId base;
    StkId func;
    StkId top;
    union { const Instruction* savedpc; int errfunc; };
    int nresults;
    unsigned int flags;
};

struct lua_Callbacks {
    void* userdata;
    void (*interrupt)(CosmicState* L, int gc);
    void (*debugbreak)(CosmicState* L, lua_Debug* ar);
    void (*onallocate)(CosmicState* L, size_t osize, size_t nsize);
    void (*userthread)(CosmicState* LP, CosmicState* L);
    void (*debugstep)(CosmicState* L, lua_Debug* ar);
    void (*debugprotectederror)(CosmicState* L);
    void (*debuginterrupt)(CosmicState* L, lua_Debug* ar);
    void (*panic)(CosmicState* L, int errcode);
    int16_t (*useratom)(CosmicState* L, const char* s, size_t l);
};

struct lua_Debug {
    int event;
    const char* name;
    const char* namewhat;
    const char* what;
    const char* source;
    int currentline;
    int linedefined;
    int lastlinedefined;
    char nups;
    char nparams;
    char isvararg;
    char istailcall;
    unsigned short short_src[60];
    CallInfo* ci;
    lua_Page* page;
};

struct lua_Page {
    lua_Page* listprev;
    lua_Page* listnext;
    lua_Page* prev;
    lua_Page* next;
    int pageSize;
    int blockSize;
    void* freeList;
    int freeNext;
    int busyBlocks;
    char __padding1[8];
    char data[1];
};

struct stringtable {
    TString** hash;
    uint32_t nuse;
    int size;
};

struct global_State {
    size_t GCthreshold;
    size_t totalbytes;
    GCObject* weak;
    GCObject* grayagain;
    GCObject* gray;
    uint8_t currentwhite;
    uint8_t gcstate;
    void* frealloc;
    void* ud;
    int gcstepsize;
    int gcstepmul;
    int gcgoal;
    stringtable strt;
    lua_Page* sweepgcopage;
    lua_Page* allpages;
    CosmicState* mainthread;
    lua_Page* allgcopages;
    lua_Page* freepages[40];
    UpVal uvhead;
    lua_Page* freegcopages[40];
    TString* tmname[21];
    TString* ttname[14];
    LuaTable* mt[14];
    TValue pseudotemp;
    TValue registry;
    int registryfree;
    void* errorjmp;
    lua_Callbacks cb;
    uint64_t rngstate;
    uint64_t ptrenckey[4];
    char ecbdata_pad[0x268];
    char udatagc_pad[0x400];
    LuaTable* udatamt[128];
    uint32_t lastprotoid;
};

struct CosmicState {
    CommonHeader;
    uint8_t status;
    uint8_t activememcat;
    bool isactive;
    bool singlestep;
    unsigned short nCcalls;
    unsigned short baseCcalls;
    int cachedslot;
    CallInfo* end_ci;
    CallInfo* base_ci;
    GCObject* gclist;
    global_State* global;
    StkId base;
    StkId top;
    StkId stack;
    CallInfo* ci;
    StkId stack_last;
    TString* namecall;
    LuaTable* gt;
    void* userdata;
    UpVal* openupval;
    int stacksize;
    int size_ci;
};

struct RobloxExtraSpace {
    RobloxExtraSpace* next;
    uintptr_t _container;
    RobloxExtraSpace* prev;
    char shared_pad[0x20];
    char identity_pad[0x28];
    uint64_t capabilities;
    char actor_pad[0x40];
};

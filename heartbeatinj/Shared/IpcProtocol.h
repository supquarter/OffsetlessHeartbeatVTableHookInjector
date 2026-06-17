#pragma once
#include <cstdint>
#include <cstring>

#define IPC_SHARED_MEM_NAME L"Global\\RBX_SIPC_Mem"
#define IPC_EVENT_CMD_NAME  L"Global\\RBX_SIPC_Cmd"
#define IPC_EVENT_RES_NAME  L"Global\\RBX_SIPC_Res"
#define IPC_BUFFER_SIZE     65536
#define IPC_DATA_OFFSET     16
#define IPC_MAX_SCRIPT      (IPC_BUFFER_SIZE - IPC_DATA_OFFSET - 4096)
#define IPC_MAX_OUTPUT      4096

enum class IpcStatus : uint32_t {
    Idle = 0,
    CmdPending = 1,
    Executing = 2,
    Done = 3,
    Error = 4
};

enum class ScriptType : uint32_t {
    Source = 0,
    Bytecode = 1
};

struct IpcBuffer {
    volatile uint32_t Status;
    volatile uint32_t ScriptType;
    uint32_t ScriptSize;
    uint32_t OutputSize;
    uint8_t Data[IPC_BUFFER_SIZE - IPC_DATA_OFFSET];
};

struct IpcLayout {
    union {
        IpcBuffer Fields;
        uint8_t Raw[IPC_BUFFER_SIZE];
    };

    void WriteScript(const uint8_t* script, uint32_t size, ScriptType type) {
        Fields.ScriptType = (uint32_t)type;
        Fields.ScriptSize = size;
        Fields.OutputSize = 0;
        if (size > IPC_MAX_SCRIPT) size = IPC_MAX_SCRIPT;
        memcpy(Fields.Data, script, size);
    }

    void WriteOutput(const uint8_t* output, uint32_t size) {
        if (size > IPC_MAX_OUTPUT) size = IPC_MAX_OUTPUT;
        Fields.OutputSize = size;
        if (size > 0 && output)
            memcpy(Fields.Data + IPC_MAX_SCRIPT, output, size);
    }

    const uint8_t* GetScript() const { return Fields.Data; }
    const uint8_t* GetOutput() const { return Fields.Data + IPC_MAX_SCRIPT; }
};

using System;
using System.IO;
using System.IO.MemoryMappedFiles;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;

namespace InjectorUI
{
    public class SharedMemoryClient : IDisposable
    {
        private const string MemName = "Global\\RBX_SIPC_Mem";
        private const string EventCmdName = "Global\\RBX_SIPC_Cmd";
        private const string EventResName = "Global\\RBX_SIPC_Res";
        private const int BufferSize = 65536;
        private const int DataOffset = 16;
        private const int MaxOutput = 4096;

        private MemoryMappedFile? _mmf;
        private MemoryMappedViewAccessor? _accessor;
        private EventWaitHandle? _eventCmd;
        private EventWaitHandle? _eventRes;

        public enum ScriptType : uint { Source = 0, Bytecode = 1 }

        public enum IpcStatus : uint { Idle = 0, CmdPending = 1, Executing = 2, Done = 3, Error = 4 }

        public static SharedMemoryClient? Connect()
        {
            try
            {
                var client = new SharedMemoryClient();
                client._mmf = MemoryMappedFile.OpenExisting(MemName, MemoryMappedFileRights.ReadWrite);
                client._accessor = client._mmf.CreateViewAccessor(0, BufferSize, MemoryMappedFileAccess.ReadWrite);
                client._eventCmd = EventWaitHandle.OpenExisting(EventCmdName);
                client._eventRes = EventWaitHandle.OpenExisting(EventResName);

                uint status = 0;
                client._accessor.Read(0, out status);
                if (status <= (uint)IpcStatus.Error)
                {
                    return client;
                }

                client.Dispose();
                return null;
            }
            catch
            {
                return null;
            }
        }

        public string Execute(ScriptType type, byte[] scriptData)
        {
            if (_accessor == null || _eventCmd == null || _eventRes == null)
                throw new InvalidOperationException("Not connected to IPC.");

            _accessor.Write(0, (uint)IpcStatus.CmdPending);
            _accessor.Write(4, (uint)type);
            _accessor.Write(8, (uint)scriptData.Length);
            _accessor.Write(12, (uint)0);

            int maxScript = BufferSize - DataOffset - MaxOutput;
            int writeSize = Math.Min(scriptData.Length, maxScript);
            _accessor.WriteArray(DataOffset, scriptData, 0, writeSize);

            _eventCmd.Set();

            bool signaled = _eventRes.WaitOne(30000);
            if (!signaled)
                throw new TimeoutException("Script execution timed out.");

            uint status = 0;
            _accessor.Read(0, out status);

            uint outputSize = 0;
            _accessor.Read(12, out outputSize);
            outputSize = Math.Min(outputSize, (uint)MaxOutput);

            byte[] outputBytes = new byte[outputSize];
            _accessor.ReadArray(DataOffset + maxScript, outputBytes, 0, (int)outputSize);

            _accessor.Write(0, (uint)IpcStatus.Idle);

            string result = Encoding.UTF8.GetString(outputBytes).TrimEnd('\0');

            if (status == (uint)IpcStatus.Error)
            {
                return result;
            }

            return string.IsNullOrWhiteSpace(result) ? "[No output]" : result;
        }

        public void Dispose()
        {
            _accessor?.Dispose();
            _mmf?.Dispose();
            _eventCmd?.Dispose();
            _eventRes?.Dispose();
        }
    }
}

using System;
using System.Diagnostics;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace InjectorUI
{
    public class InjectorProcess : IDisposable
    {
        private Process? _process;
        private readonly string _exePath;
        private readonly StringBuilder _outputBuffer = new();

        public event Action<string>? OnOutput;
        public event Action<int>? OnExit;

        public InjectorProcess(string exePath)
        {
            _exePath = exePath;
        }

        public void Start(string? workingDir = null)
        {
            _process = new Process
            {
                StartInfo = new ProcessStartInfo
                {
                    FileName = _exePath,
                    WorkingDirectory = workingDir ?? AppDomain.CurrentDomain.BaseDirectory,
                    UseShellExecute = false,
                    RedirectStandardOutput = true,
                    RedirectStandardError = true,
                    CreateNoWindow = true,
                    StandardOutputEncoding = Encoding.UTF8
                },
                EnableRaisingEvents = true
            };

            _process.OutputDataReceived += (s, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    _outputBuffer.AppendLine(e.Data);
                    OnOutput?.Invoke(e.Data);
                }
            };

            _process.ErrorDataReceived += (s, e) =>
            {
                if (!string.IsNullOrEmpty(e.Data))
                {
                    OnOutput?.Invoke("[ERR] " + e.Data);
                }
            };

            _process.Exited += (s, e) =>
            {
                OnExit?.Invoke(_process?.ExitCode ?? -1);
            };

            _process.Start();
            _process.BeginOutputReadLine();
            _process.BeginErrorReadLine();
        }

        public void WaitForExit(int timeoutMs = -1)
        {
            _process?.WaitForExit(timeoutMs);
        }

        public string GetAllOutput() => _outputBuffer.ToString();

        public void Dispose()
        {
            if (_process != null && !_process.HasExited)
            {
                try { _process.Kill(); } catch { }
            }
            _process?.Dispose();
            _process = null;
        }
    }
}

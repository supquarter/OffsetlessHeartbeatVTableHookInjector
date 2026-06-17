using System;
using System.Diagnostics;
using System.Text;

namespace InjectorUI
{
    public class InjectorProcess : IDisposable
    {
        private Process? _process;
        private readonly string _exePath;

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
                    UseShellExecute = true,
                    WindowStyle = ProcessWindowStyle.Normal
                },
                EnableRaisingEvents = true
            };

            _process.Exited += (s, e) =>
            {
                OnExit?.Invoke(_process?.ExitCode ?? -1);
            };

            _process.Start();
        }

        public void WaitForExit(int timeoutMs = -1)
        {
            _process?.WaitForExit(timeoutMs);
        }

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

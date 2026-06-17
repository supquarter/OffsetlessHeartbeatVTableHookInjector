using System;
using System.Diagnostics;
using System.IO;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Media;
using System.Windows.Threading;
using Microsoft.Web.WebView2.Wpf;

namespace InjectorUI
{
    public partial class MainWindow : Window
    {
        private InjectorProcess? _injector;
        private SharedMemoryClient? _ipcClient;
        private bool _isInjected;
        private CancellationTokenSource? _monitorCts;

        public MainWindow()
        {
            InitializeComponent();
            Loaded += OnLoaded;
            Closing += OnClosing;
        }

        private async void OnLoaded(object sender, RoutedEventArgs e)
        {
            await InitializeWebView();
        }

        private void OnClosing(object? sender, System.ComponentModel.CancelEventArgs e)
        {
            _monitorCts?.Cancel();
            _ipcClient?.Dispose();
            _injector?.Dispose();
        }

        private async Task InitializeWebView()
        {
            var env = await Microsoft.Web.WebView2.Core.CoreWebView2Environment
                .CreateAsync(null, Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "WebView2Cache"));
            await WebView.EnsureCoreWebView2Async(env);

            string editorPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "www", "editor.html");
            if (File.Exists(editorPath))
            {
                WebView.Source = new Uri(editorPath);
            }
            else
            {
                WebView.CoreWebView2.NavigateToString(GetEmbeddedEditorHtml());
            }

            WebView.CoreWebView2.WebMessageReceived += OnWebMessageReceived;
        }

        private void OnWebMessageReceived(object? sender, Microsoft.Web.WebView2.Core.CoreWebView2WebMessageReceivedEventArgs e)
        {
            string message = e.TryGetWebMessageAsString();
        }

        private async void BtnInject_Click(object sender, RoutedEventArgs e)
        {
            BtnInject.IsEnabled = false;
            SetStatus("Launching injector...", Colors.Gold);

            string injectorPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "injector.exe");
            if (!File.Exists(injectorPath))
            {
                injectorPath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..", "..", "..", "..", "build", "injector.exe");
                injectorPath = Path.GetFullPath(injectorPath);
            }

            if (!File.Exists(injectorPath))
            {
                SetStatus("injector.exe not found", Colors.Red);
                Log("[-] injector.exe not found at: " + injectorPath);
                BtnInject.IsEnabled = true;
                return;
            }

            Log("[*] Running injector: " + injectorPath);

            try
            {
                _injector = new InjectorProcess(injectorPath);
                _injector.OnExit += (code) => Dispatcher.Invoke(() => OnInjectorExited(code));

                string? modulePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "module.dll");
                if (!File.Exists(modulePath))
                {
                    modulePath = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "..", "..", "..", "..", "build", "module.dll");
                    modulePath = Path.GetFullPath(modulePath);
                }

                if (modulePath != null && File.Exists(modulePath))
                {
                    string? dir = Path.GetDirectoryName(modulePath);
                    if (dir != null)
                        _injector.Start(dir);
                    else
                        _injector.Start();
                }
                else
                {
                    _injector.Start();
                }

                await Task.Run(() => _injector.WaitForExit(30000));
            }
            catch (Exception ex)
            {
                Log("[-] Failed to launch injector: " + ex.Message);
                SetStatus("Injector failed", Colors.Red);
                BtnInject.IsEnabled = true;
            }
        }

        private void OnInjectorExited(int exitCode)
        {
            if (exitCode == 0)
            {
                SetStatus("Connecting to module...", Colors.Gold);
                Log("[+] Injector exited successfully. Connecting IPC...");

                _monitorCts = new CancellationTokenSource();
                Task.Run(() => MonitorIpcConnection(_monitorCts.Token));
            }
            else
            {
                SetStatus("Injection failed", Colors.Red);
                Log("[-] Injector exited with code: " + exitCode);
                BtnInject.IsEnabled = true;
            }
        }

        private async Task MonitorIpcConnection(CancellationToken ct)
        {
            for (int i = 0; i < 60; i++)
            {
                if (ct.IsCancellationRequested) return;

                try
                {
                    _ipcClient = SharedMemoryClient.Connect();
                    if (_ipcClient != null)
                    {
                        await Dispatcher.InvokeAsync(() =>
                        {
                            _isInjected = true;
                            SetStatus("Connected", Colors.LimeGreen);
                            BtnExecute.IsEnabled = true;
                            Log("[+] Connected to module via shared memory.");
                        });
                        return;
                    }
                }
                catch { }

                try { await Task.Delay(500, ct); } catch { return; }
            }

            await Dispatcher.InvokeAsync(() =>
            {
                SetStatus("No connection", Colors.Orange);
                Log("[-] Timed out waiting for module IPC.");
                BtnInject.IsEnabled = true;
            });
        }

        private async void BtnExecute_Click(object sender, RoutedEventArgs e)
        {
            if (!_isInjected || _ipcClient == null)
            {
                Log("[-] Not connected to module.");
                return;
            }

            BtnExecute.IsEnabled = false;
            SetStatus("Executing...", Colors.Gold);

            string script = await GetEditorContent();
            if (string.IsNullOrWhiteSpace(script))
            {
                Log("[-] No script to execute.");
                SetStatus("Connected", Colors.LimeGreen);
                BtnExecute.IsEnabled = true;
                return;
            }

            bool isBytecode = ChkBytecode.IsChecked == true;
            string scriptType = isBytecode ? "Bytecode" : "Source";

            Log($"[*] Executing {scriptType} script ({script.Length} chars)...");

            try
            {
                string result = await Task.Run(() =>
                {
                    if (isBytecode)
                    {
                        var bytes = Convert.FromBase64String(script);
                        return _ipcClient.Execute(SharedMemoryClient.ScriptType.Bytecode, bytes);
                    }
                    else
                    {
                        var data = Encoding.UTF8.GetBytes(script);
                        return _ipcClient.Execute(SharedMemoryClient.ScriptType.Source, data);
                    }
                });

                TxtOutput.Text = result;
                Log($"[+] Execution complete. Output: {result.Length} chars.");
                SetStatus("Connected", Colors.LimeGreen);
            }
            catch (Exception ex)
            {
                TxtOutput.Text = $"[Error] {ex.Message}";
                Log($"[-] Execution error: {ex.Message}");
                SetStatus("Error", Colors.Red);
            }

            BtnExecute.IsEnabled = true;
        }

        private void BtnClear_Click(object sender, RoutedEventArgs e)
        {
            TxtOutput.Text = "";
            Log("[*] Output cleared.");
        }

        private async Task<string> GetEditorContent()
        {
            try
            {
                string script = await WebView.CoreWebView2.ExecuteScriptAsync("getEditorContent()");
                if (script.StartsWith("\"") && script.EndsWith("\""))
                {
                    script = script.Substring(1, script.Length - 2);
                    script = script.Replace("\\\"", "\"")
                                   .Replace("\\n", "\n")
                                   .Replace("\\r", "\r")
                                   .Replace("\\t", "\t");
                }
                return script;
            }
            catch
            {
                return "";
            }
        }

        private void SetStatus(string text, Color color)
        {
            TxtStatus.Text = text;
            StatusDot.Fill = new SolidColorBrush(color);
        }

        private void Log(string message)
        {
            string timestamp = DateTime.Now.ToString("HH:mm:ss");
            TxtLog.Text = $"[{timestamp}] {message}";
        }

        private string GetEmbeddedEditorHtml()
        {
            return @"<!DOCTYPE html>
<html>
<head>
<meta charset='utf-8'>
<style>
* { margin: 0; padding: 0; box-sizing: border-box; }
html, body { height: 100%; overflow: hidden; background: #1E1E1E; }
#container { width: 100%; height: 100%; }
</style>
</head>
<body>
<div id='container'></div>
<script>
var require = { paths: { vs: 'https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.52.2/min/vs' } };
</script>
<script src='https://cdnjs.cloudflare.com/ajax/libs/monaco-editor/0.52.2/min/vs/loader.js'></script>
<script>
require(['vs/editor/editor.main'], function() {
    monaco.editor.defineTheme('darkTheme', {
        base: 'vs-dark',
        inherit: true,
        rules: [
            { token: 'comment', foreground: '6A9955' },
            { token: 'keyword', foreground: '569CD6' },
            { token: 'string', foreground: 'CE9178' },
            { token: 'number', foreground: 'B5CEA8' },
            { token: 'function', foreground: 'DCDCAA' },
            { token: 'type', foreground: '4EC9B0' }
        ],
        colors: {
            'editor.background': '#1E1E1E',
            'editor.foreground': '#D4D4D4',
            'editor.lineHighlightBackground': '#2A2D2E',
            'editor.selectionBackground': '#264F78',
            'editorCursor.foreground': '#AEAFAD'
        }
    });
    window.editor = monaco.editor.create(document.getElementById('container'), {
        value: '-- Enter Luau script here\\nprint(""Hello from UI!"")\\n',
        language: 'lua',
        theme: 'darkTheme',
        fontSize: 14,
        fontFamily: ""Consolas, 'Courier New', monospace"",
        minimap: { enabled: true },
        automaticLayout: true,
        scrollBeyondLastLine: false,
        lineNumbers: 'on',
        renderWhitespace: 'selection',
        tabSize: 4,
        insertSpaces: true
    });
    window.getEditorContent = function() {
        return editor.getValue();
    };
});
</script>
</body>
</html>";
        }
    }
}

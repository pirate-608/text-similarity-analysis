# 使用 ngrok 暴露 Web 服务

本指南介绍如何使用 ngrok 将本地运行的文本相似度分析 Web 服务暴露给公网访问。

## 1. 安装 ngrok

如果尚未安装 ngrok，可以通过以下方式安装：

### Windows (使用 Winget)
在终端中运行：
```powershell
winget install ngrok.ngrok
```

### 手动下载
访问 [ngrok 官网](https://ngrok.com/download) 下载适用于您操作系统的版本。

## 2. 配置 ngrok

1. 注册 ngrok 账号并获取 Authtoken。
2. 配置 Authtoken：
   ```bash
   ngrok config add-authtoken <YOUR_AUTHTOKEN>
   ```

## 3. 启动服务

1. 首先启动本地 Web 服务（确保在项目根目录下）：
   ```bash
   python web/app.py
   ```
   服务将运行在 `http://127.0.0.1:5000`。

2. 打开一个新的终端窗口，运行 ngrok 转发端口 5000：
   ```bash
   ngrok http 5000
   ```

## 4. 常见问题与故障排查

### 找不到 ngrok 命令
如果您在安装后运行 `ngrok` 提示 "无法识别" 或 "not recognized"：

1. **尝试使用完整路径运行**：
   Winget 安装的 ngrok 可能位于深层目录中。您可以在 PowerShell 中尝试运行：
   ```powershell
   # 查找并运行 ngrok
   $ngrokPath = Get-ChildItem -Path $env:LOCALAPPDATA -Filter ngrok.exe -Recurse -ErrorAction SilentlyContinue | Select-Object -ExpandProperty FullName -First 1
   & $ngrokPath http 5000
   ```
   
   **针对您的环境，可以直接运行：**
   ```powershell
   & "C:\Users\黄悠然\AppData\Local\Microsoft\WinGet\Packages\Ngrok.Ngrok_Microsoft.Winget.Source_8wekyb3d8bbwe\ngrok.exe" http 5000
   ```

2. **手动添加到 PATH**：
   如果找到路径（如 `C:\Users\...\WinGet\Packages\...\ngrok.exe`），可以将该目录添加到系统环境变量 PATH 中，或者直接使用完整路径。

### 版本过旧导致认证失败
如果遇到 `authentication failed: Your ngrok-agent version ... is too old` 错误：
请运行以下命令进行更新：
```powershell
# 使用完整路径运行 update
& "C:\Users\黄悠然\AppData\Local\Microsoft\WinGet\Packages\Ngrok.Ngrok_Microsoft.Winget.Source_8wekyb3d8bbwe\ngrok.exe" update
```

## 5. 访问

ngrok 启动后会显示一个公网 URL（例如 `https://xxxx-xx-xx-xx-xx.ngrok-free.app`）。
您可以使用该 URL 在任何设备上访问您的本地服务。

## 注意事项

- 免费版 ngrok 的 URL 每次重启都会变化。
- 请确保不要在公共网络中暴露敏感数据。

# 全平台编译指南

## 编译前准备

### 必需工具
1. **Python 3.x**: [下载 Python](https://www.python.org/downloads/)
2. **SCons**: 安装命令 `pip install scons==4.4.0`
3. **C++ 编译器**:
   - Windows: Visual Studio Build Tools 或 MinGW
   - Linux: GCC/Clang (`sudo apt install build-essential`)
   - macOS: Xcode Command Line Tools

### 平台特定依赖

#### Android 平台
需要安装 Android SDK 和 NDK:
```bash
export ANDROID_HOME=/path/to/android/sdk
export ANDROID_NDK_ROOT=$ANDROID_HOME/ndk/23.2.8568313
```

#### Web 平台
需要安装 Emscripten:
```bash
# 安装 Emscripten
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

#### macOS/iOS 平台
需要在 macOS 系统上编译,需要 Xcode。

**重要限制**:
- macOS 编译**不能在 Windows/Linux 上进行**,必须在 macOS 系统上完成
- Apple 的许可证限制不允许在其他平台交叉编译 macOS/iOS 应用
- 需要安装 Xcode Command Line Tools: `xcode-select --install`

**支持的架构**:
- `universal` - 通用二进制(同时支持 Intel 和 Apple Silicon Mac)
- `x86_64` - Intel Mac
- `arm64` - Apple Silicon Mac (M1/M2/M3)

## 编译方法

### 方法一:使用 GitHub Actions 自动编译

这是最简单的方法,不需要本地环境:

1. Push 代码到 GitHub 仓库
2. GitHub Actions 会自动触发 `.github/workflows/builds.yml`
3. 编译完成后在 Actions 页面下载编译产物

**优势**:
- 自动编译所有平台
- 无需配置复杂的环境
- 编译产物直接打包下载

**支持的平台**:
- Windows (x86_32, x86_64)
- Linux (x86_64)
- Android (arm32, arm64, x86_32, x86_64)
- **macOS (universal)** - 已启用,自动编译通用二进制
- iOS (需要在配置中启用)
- Web (需要在配置中启用)

**macOS 自动编译说明**:
- GitHub Actions 使用 `macos-latest` 运行环境
- 自动编译 `universal` 架构(同时支持 Intel 和 Apple Silicon)
- 包含 Debug 和 Release 版本
- 包含单精度和双精度版本

### 方法二:本地批量编译

#### Windows 系统使用 PowerShell:
```powershell
.\build_all.ps1
```

#### Linux/macOS 系统使用 Bash:
```bash
chmod +x build_all.sh
./build_all.sh
```

### 方法三:单独编译某个平台

#### Windows x86_64 Debug:
```bash
scons platform=windows arch=x86_64 target=template_debug precision=single
```

#### Linux x86_64 Release:
```bash
scons platform=linux arch=x86_64 target=template_release precision=single
```

#### Android arm64 Debug:
```bash
scons platform=android arch=arm64 target=template_debug precision=single
```

#### macOS Universal Debug:
```bash
scons platform=macos arch=universal target=template_debug precision=single
```

#### macOS Universal Release:
```bash
scons platform=macos arch=universal target=template_release precision=single
```

#### macOS ARM64 Debug (Apple Silicon):
```bash
scons platform=macos arch=arm64 target=template_debug precision=single
```

#### macOS x86_64 Debug (Intel):
```bash
scons platform=macos arch=x86_64 target=template_debug precision=single
```

#### Web Debug:
```bash
scons platform=web arch=wasm32 target=template_debug precision=single
```

## 编译参数说明

### platform (平台)
- `windows` - Windows 系统
- `linux` - Linux 系统
- `android` - Android 系统
- `macos` - macOS 系统
- `ios` - iOS 系统
- `web` - Web 浏览器

### arch (架构)
- `x86_32` - 32位 x86
- `x86_64` - 64位 x86
- `arm32` - 32位 ARM
- `arm64` - 64位 ARM
- `universal` - macOS 通用二进制
- `wasm32` - WebAssembly

### target (目标)
- `template_debug` - Debug 版本(用于开发调试)
- `template_release` - Release 版本(用于最终发布)

### precision (精度)
- `single` - 单精度浮点数(默认)
- `double` - 双精度浮点数(用于高精度计算)

## 编译产物

编译完成后,产物会在以下目录:
- `bin/<platform>/` - 编译后的库文件
- `godot/addons/godot_gameplay_attributes/bin/<platform>/` - 复制的库文件

## 常见问题

### 1. Windows 编译失败
确保安装了 Visual Studio Build Tools 或 MinGW:
```powershell
# 使用 Visual Studio
# 安装 "Desktop development with C++" 工具集

# 使用 MinGW (需要 12.2.0 或更高版本)
# https://github.com/egor-tensin/setup-mingw
```

### 2. Android 编译失败
检查 Android SDK/NDK 环境变量:
```bash
echo $ANDROID_HOME
echo $ANDROID_NDK_ROOT
```

### 3. Linux 编译失败
安装必需的依赖:
```bash
sudo apt update
sudo apt install build-essential pkg-config
```

### 4. 编译时间过长
使用 `production=yes` 参数优化编译速度:
```bash
scons platform=linux arch=x86_64 target=template_debug precision=single production=yes
```

### 5. 如何启用更多平台
编辑 `.github/workflows/builds.yml`,取消注释相应平台的配置。

### 6. macOS 编译问题
macOS 编译只能在 macOS 系统上进行,不能在 Windows/Linux 上交叉编译。

**解决方案**:
- 使用 GitHub Actions 的 macOS 运行环境自动编译
- 如果有 macOS 系统,可以在本地编译
- 确保 Xcode Command Line Tools 已安装: `xcode-select --install`

### 7. macOS 编译产物位置
macOS 编译产物是 Framework 格式:
```
bin/macos/godot_gameplay_attributes.macos.framework/
```

### 8. iOS 编译
iOS 编译需要在 macOS 系统上,并需要 iOS SDK。编辑 `.github/workflows/builds.yml` 启用 iOS 编译配置。

## 完整示例

### 编译所有常见平台(单精度):
```bash
# Windows
scons platform=windows arch=x86_64 target=template_debug precision=single
scons platform=windows arch=x86_64 target=template_release precision=single

# Linux
scons platform=linux arch=x86_64 target=template_debug precision=single
scons platform=linux arch=x86_64 target=template_release precision=single

# macOS (需要在 macOS 系统上编译)
scons platform=macos arch=universal target=template_debug precision=single
scons platform=macos arch=universal target=template_release precision=single

# Android
scons platform=android arch=arm64 target=template_debug precision=single
scons platform=android arch=arm64 target=template_release precision=single
```

### 编译双精度版本(高精度计算):
```bash
scons platform=windows arch=x86_64 target=template_debug precision=double
scons platform=linux arch=x86_64 target=template_debug precision=double
scons platform=macos arch=universal target=template_debug precision=double
```

## 建议

- **开发阶段**: 使用 `template_debug` 版本,方便调试
- **发布阶段**: 使用 `template_release` 版本,性能更好
- **推荐方式**: 使用 GitHub Actions 自动编译,无需配置复杂环境
- **本地编译**: 根据需要编译特定平台,避免全平台编译耗时过长
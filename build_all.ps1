# 全平台编译脚本
# 需要安装 SCons: pip install scons==4.4.0

Write-Host "开始全平台编译..." -ForegroundColor Green

# 定义编译配置
$builds = @(
    # Windows 平台
    @{platform="windows"; arch="x86_64"; target="template_debug"; precision="single"},
    @{platform="windows"; arch="x86_64"; target="template_release"; precision="single"},
    @{platform="windows"; arch="x86_32"; target="template_debug"; precision="single"},
    @{platform="windows"; arch="x86_32"; target="template_release"; precision="single"},
    
    # Linux 平台 (需要在 Linux 环境中编译)
    @{platform="linux"; arch="x86_64"; target="template_debug"; precision="single"},
    @{platform="linux"; arch="x86_64"; target="template_release"; precision="single"},
    
    # macOS 平台 (需要在 macOS 环境中编译)
    # 注意: macOS 编译只能在 macOS 系统上运行
    @{platform="macos"; arch="universal"; target="template_debug"; precision="single"},
    @{platform="macos"; arch="universal"; target="template_release"; precision="single"},
    
    # Android 平台 (需要 Android SDK/NDK)
    @{platform="android"; arch="arm64"; target="template_debug"; precision="single"},
    @{platform="android"; arch="arm32"; target="template_debug"; precision="single"},
    @{platform="android"; arch="x86_64"; target="template_debug"; precision="single"},
    @{platform="android"; arch="x86_32"; target="template_debug"; precision="single"},
    
    # 双精度版本
    @{platform="windows"; arch="x86_64"; target="template_debug"; precision="double"},
    @{platform="windows"; arch="x86_64"; target="template_release"; precision="double"},
    @{platform="macos"; arch="universal"; target="template_debug"; precision="double"},
)

$totalBuilds = $builds.Count
$currentBuild = 0

foreach ($build in $builds) {
    $currentBuild++
    $platform = $build.platform
    $arch = $build.arch
    $target = $build.target
    $precision = $build.precision
    
    Write-Host "`n[$currentBuild/$totalBuilds] 编译 $platform $arch $target $precision..." -ForegroundColor Cyan
    
    # 执行编译命令
    $command = "scons platform=$platform arch=$arch target=$target precision=$precision production=yes"
    
    try {
        Invoke-Expression $command
        Write-Host "✓ $platform $arch $target $precision 编译成功" -ForegroundColor Green
    }
    catch {
        Write-Host "✗ $platform $arch $target $precision 编译失败: $_" -ForegroundColor Red
    }
}

Write-Host "`n全平台编译完成!" -ForegroundColor Green
Write-Host "编译产物位于 bin/ 目录下" -ForegroundColor Yellow
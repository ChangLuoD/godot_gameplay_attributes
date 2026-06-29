#!/bin/bash
# 全平台编译脚本
# 需要安装 SCons: pip install scons==4.4.0

echo "开始全平台编译..."

# 定义编译配置
builds=(
    # Linux 平台
    "linux x86_64 template_debug single"
    "linux x86_64 template_release single"
    
    # Windows 平台 (交叉编译可能需要 MinGW)
    "windows x86_64 template_debug single"
    "windows x86_64 template_release single"
    "windows x86_32 template_debug single"
    
    # macOS 平台 (需要在 macOS 环境中编译)
    # 注意: macOS 编译只能在 macOS 系统上运行
    "macos universal template_debug single"
    "macos universal template_release single"
    
    # Android 平台 (需要 Android SDK/NDK)
    "android arm64 template_debug single"
    "android arm32 template_debug single"
    "android x86_64 template_debug single"
    "android x86_32 template_debug single"
    
    # Web 平台 (需要 Emscripten)
    "web wasm32 template_debug single"
    
    # 双精度版本
    "linux x86_64 template_debug double"
    "windows x86_64 template_debug double"
    "macos universal template_debug double"
)

total_builds=${#builds[@]}
current_build=0

for build in "${builds[@]}"; do
    current_build=$((current_build + 1))
    read -r platform arch target precision <<< "$build"
    
    echo ""
    echo "[$current_build/$total_builds] 编译 $platform $arch $target $precision..."
    
    # 执行编译命令
    if scons platform=$platform arch=$arch target=$target precision=$precision production=yes; then
        echo "✓ $platform $arch $target $precision 编译成功"
    else
        echo "✗ $platform $arch $target $precision 编译失败"
    fi
done

echo ""
echo "全平台编译完成!"
echo "编译产物位于 bin/ 目录下"
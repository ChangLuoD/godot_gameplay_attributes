# GitHub Actions 使用指南

## 什么是 GitHub Actions?

GitHub Actions 是 GitHub 提供的自动化持续集成/持续部署 (CI/CD) 服务。它可以在代码推送、Pull Request 等事件触发时自动执行编译、测试、部署等任务。

这个项目的 `builds.yml` 文件定义了自动编译所有平台的流程。

## 如何使用 GitHub Actions

### 第一步:将项目推送到 GitHub

1. **创建 GitHub 仓库**
   - 登录 GitHub: https://github.com
   - 点击右上角 "+" → "New repository"
   - 创建一个新仓库,例如: `godot_gameplay_attributes`

2. **推送代码到 GitHub**

   在本地项目目录执行以下命令:

   ```bash
   # 初始化 Git (如果还没有)
   git init

   # 添加远程仓库
   git remote add origin https://github.com/你的用户名/godot_gameplay_attributes.git

   # 添加所有文件
   git add .

   # 提交
   git commit -m "Initial commit: 添加 GitHub Actions 编译配置"

   # 推送到 GitHub
   git push -u origin main
   ```

### 第二步:触发自动编译

当你推送代码到 GitHub 后,Actions 会自动开始编译:

#### 自动触发条件:
- 每次 `push` 到任何分支
- Pull Request 创建时
- 手动触发(可选)

#### 编译过程:
1. GitHub 自动检测 `.github/workflows/builds.yml` 文件
2. 为每个平台配置创建一个编译任务
3. 使用相应的运行环境(Windows/Linux/macOS)
4. 执行编译命令
5. 上传编译产物

### 第三步:查看编译状态

#### 在 GitHub 网站上查看:

1. **进入 Actions 页面**
   - 打开你的 GitHub 仓库
   - 点击顶部菜单的 "Actions" 标签

2. **查看编译进度**
   - 可以看到当前正在运行的编译任务
   - 绿色圆点表示正在运行
   - 绿色对勾表示成功完成
   - 红色叉号表示失败

3. **查看具体任务**
   - 点击某个编译任务查看详细日志
   - 可以看到编译步骤和输出信息
   - 如果编译失败,可以看到错误信息

### 第四步:下载编译产物

#### 编译完成后下载:

1. **进入 Actions 页面**
   - 点击已完成的编译任务

2. **下载编译产物**
   - 在页面底部的 "Artifacts" 区域
   - 点击 `godot_gameplay_attributes` 下载所有平台的编译文件
   - 或者点击单独的文件,例如 `godot_gameplay_attributes-windows-x86_64-single-template_debug`

3. **解压使用**
   - 下载的是 ZIP 压缩包
   - 解压后得到编译好的库文件
   - 将文件放到你的 Godot 项目中

## GitHub Actions 配置详解

### 编译配置矩阵

这个项目配置了以下平台的自动编译:

#### Windows 平台:
- Windows x86_32 Debug/Release (单精度)
- Windows x86_64 Debug/Release (单精度)
- Windows x86_32 Debug (双精度)
- Windows x86_64 Debug/Release (双精度)

#### Linux 平台:
- Linux x86_64 Debug/Release (单精度)
- Linux x86_64 Debug/Release (双精度)

#### macOS 平台:
- macOS Universal Debug/Release (单精度)
- macOS Universal Debug/Release (双精度)

#### Android 平台:
- Android arm32/arm64/x86_32/x86_64 Debug/Release (单精度和双精度)

### 编译流程

每个编译任务执行以下步骤:

1. **环境准备**
   - 安装 Python 3.x
   - 安装 SCons 构建工具
   - 安装平台特定的依赖(Android SDK, MinGW 等)

2. **代码下载**
   - 从 GitHub 下载项目源代码
   - 包括 godot-cpp 子模块

3. **编译执行**
   - 编译 godot-cpp
   - 编译 GDExtension 库
   - 生成平台特定的库文件

4. **产物上传**
   - 将编译好的库文件打包
   - 上传到 GitHub Artifacts

## 高级功能

### 手动触发编译

可以手动触发编译,无需推送代码:

1. 进入 Actions 页面
2. 选择 "Build GDExtension" workflow
3. 点击 "Run workflow" 按钮
4. 选择要编译的分支
5. 点击绿色的 "Run workflow" 按钮

### 查看编译日志

如果编译失败,可以查看详细日志找出问题:

1. 点击失败的编译任务
2. 展开每个步骤查看输出
3. 查看错误信息定位问题
4. 修复代码后重新推送

### 优化编译时间

GitHub Actions 会缓存编译结果,加速后续编译:

- 使用 SCons 缓存机制
- 缓存 godot-cpp 编译结果
- 重复编译会更快完成

## 常见问题

### 1. 编译失败怎么办?

**检查错误日志**:
- 进入 Actions 页面
- 点击失败的编译任务
- 查看详细的错误信息
- 根据错误信息修复代码

**常见错误原因**:
- 代码语法错误
- 缺少依赖库
- 编译器版本不兼容

### 2. 如何只编译特定平台?

编辑 `.github/workflows/builds.yml` 文件:
- 注释掉不需要的平台配置
- 只保留需要的平台

### 3. 编译产物在哪里?

编译完成后:
- 在 Actions 页面下载
- 文件名格式: `godot_gameplay_attributes-平台-架构-精度-类型`
- 解压后放到 Godot 项目中

### 4. 如何添加新平台?

编辑 `.github/workflows/builds.yml`:
- 取消注释被注释的平台配置
- 例如取消 iOS、Web 平台的注释

### 5. 编译时间多长?

典型编译时间:
- 第一次编译: 5-15 分钟(每个平台)
- 后续编译: 2-5 分钟(利用缓存)
- 所有平台并行编译

### 6. 是否消耗 GitHub 资源?

GitHub Actions 对公开仓库免费:
- 每月 2000 分钟免费额度
- 私有仓库有使用限制
- 这个项目的编译不会超出免费额度

## 优势

### 使用 GitHub Actions 的优势:

1. **无需配置环境**
   - 不需要本地安装编译工具
   - 不需要配置 Android SDK/NDK
   - 不需要 macOS 系统(使用 GitHub 的 macOS 环境)

2. **自动编译所有平台**
   - 自动编译 Windows/Linux/macOS/Android
   - 自动编译 Debug 和 Release 版本
   - 自动编译单精度和双精度版本

3. **持续集成**
   - 每次代码变更自动编译
   - 快速发现编译错误
   - 确保代码质量

4. **方便分发**
   - 编译产物自动打包
   - 可以直接下载使用
   - 无需手动编译

5. **免费使用**
   - 公开仓库完全免费
   - 无限制的编译次数

## 最佳实践

### 推送代码的最佳时机:

- 完成重要功能开发后
- 修复 Bug 后
- 添加新类或功能后
- 更新文档后

### 检查编译状态:

- 推送代码后立即检查 Actions 页面
- 确保所有平台编译成功
- 下载编译产物测试功能

### 发布版本:

- 创建 Git tag 标记版本
- Actions 会编译该版本
- 下载编译产物发布给用户

## 总结

GitHub Actions 是最简单的编译方法:

1. **推送代码到 GitHub** - 自动触发编译
2. **等待编译完成** - 查看编译进度和日志
3. **下载编译产物** - 获取所有平台的库文件

**推荐使用 GitHub Actions**,无需配置复杂的本地环境,自动编译所有平台!
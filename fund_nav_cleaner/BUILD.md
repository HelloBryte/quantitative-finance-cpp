# Build Instructions / 构建说明

This document provides detailed instructions for building the Fund NAV Data Cleaner project.

本文档提供基金净值数据清洗工具项目的详细构建说明。

## Prerequisites / 前置要求

### Required / 必需
- **CMake**: Version 3.14 or higher
- **C++ Compiler**: Supporting C++17 standard
  - GCC 7.0+
  - Clang 5.0+
  - MSVC 2017+
  - Apple Clang (Xcode 10+)

### Optional / 可选
- **Make** or **Ninja**: Build tools
- **Git**: For cloning the repository

## Build Steps / 构建步骤

### 1. Navigate to Project Directory / 进入项目目录

```bash
cd /Users/mac/PycharmProjects/finance/fund_nav_cleaner
```

### 2. Create Build Directory / 创建构建目录

```bash
mkdir build
cd build
```

### 3. Configure with CMake / 使用CMake配置

**Debug Build** / 调试构建:
```bash
cmake -DCMAKE_BUILD_TYPE=Debug ..
```

**Release Build** / 发布构建:
```bash
cmake -DCMAKE_BUILD_TYPE=Release ..
```

**Without Tests** / 不构建测试:
```bash
cmake -DBUILD_TESTS=OFF ..
```

### 4. Build the Project / 构建项目

**Using Make**:
```bash
make -j$(nproc)
```

**Using CMake (platform-independent)**:
```bash
cmake --build . --config Release -j 4
```

**Using Ninja** (if available):
```bash
cmake -G Ninja ..
ninja
```

## Build Targets / 构建目标

The project generates the following executables:

项目生成以下可执行文件：

### 1. Static Library / 静态库
```
build/libnav_cleaner_lib.a
```
Core functionality as a reusable library.

核心功能的可重用库。

### 2. Demo Executable / 演示程序
```
build/demo
```
Demonstrates the cleaning tool with sample data.

使用示例数据演示清洗工具。

**Run the demo** / 运行演示:
```bash
./demo
```

### 3. Test Suite / 测试套件
```
build/run_tests
```
Google Test-based unit test suite.

基于Google Test的单元测试套件。

**Run all tests** / 运行所有测试:
```bash
./run_tests
```

**Run specific test** / 运行特定测试:
```bash
./run_tests --gtest_filter=NavValidatorTest.DetectOutliers
```

## Platform-Specific Instructions / 特定平台说明

### macOS

**Install dependencies with Homebrew**:
```bash
brew install cmake
```

**Build command**:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(sysctl -n hw.ncpu)
```

### Linux (Ubuntu/Debian)

**Install dependencies**:
```bash
sudo apt-get update
sudo apt-get install build-essential cmake
```

**Build command**:
```bash
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
```

### Windows (Visual Studio)

**Using Visual Studio 2019+**:
```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A x64 ..
cmake --build . --config Release
```

**Run from build directory**:
```cmd
Release\demo.exe
Release\run_tests.exe
```

## Build Options / 构建选项

### CMake Options / CMake选项

| Option | Default | Description |
|--------|---------|-------------|
| `BUILD_TESTS` | `ON` | Build unit tests / 构建单元测试 |
| `CMAKE_BUILD_TYPE` | `Debug` | Build type (Debug/Release) / 构建类型 |

**Example** / 示例:
```bash
cmake -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release ..
```

## Compiler Flags / 编译器标志

### Debug Build / 调试构建
- `-g`: Debug symbols / 调试符号
- `-O0`: No optimization / 无优化
- `-Wall -Wextra -Wpedantic`: All warnings / 所有警告

### Release Build / 发布构建
- `-O3`: Maximum optimization / 最大优化
- `-DNDEBUG`: Disable assertions / 禁用断言
- `-Wall -Wextra -Wpedantic`: All warnings / 所有警告

## Verification / 验证

After building, verify the installation:

构建后，验证安装：

```bash
# Check if executables exist
ls -lh demo run_tests

# Run quick test
./demo

# Run all unit tests
./run_tests
```

**Expected output** / 预期输出:
```
[==========] Running X tests from Y test suites.
[==========] X tests from Y test suites ran.
[  PASSED  ] X tests.
```

## Troubleshooting / 故障排除

### Issue: CMake version too old / CMake版本过旧

**Error**:
```
CMake Error: CMake 3.14 or higher is required.
```

**Solution** / 解决方案:
```bash
# macOS
brew upgrade cmake

# Ubuntu
sudo snap install cmake --classic
```

### Issue: Compiler not found / 找不到编译器

**Error**:
```
No CMAKE_CXX_COMPILER could be found.
```

**Solution** / 解决方案:
```bash
# macOS
xcode-select --install

# Ubuntu
sudo apt-get install g++
```

### Issue: Google Test download fails / Google Test下载失败

**Error**:
```
Could not resolve host: github.com
```

**Solution** / 解决方案:
- Check internet connection / 检查网络连接
- Or build without tests: `cmake -DBUILD_TESTS=OFF ..`

### Issue: Permission denied / 权限被拒绝

**Error**:
```
Permission denied
```

**Solution** / 解决方案:
```bash
chmod +x demo run_tests
```

## Clean Build / 清理构建

To start fresh:

重新开始构建：

```bash
# Remove build directory
cd ..
rm -rf build

# Rebuild
mkdir build && cd build
cmake ..
make
```

## IDE Integration / IDE集成

### Visual Studio Code

1. Install **CMake Tools** extension
2. Open project folder
3. Select kit (compiler)
4. Build with `Cmd+Shift+B` (macOS) or `Ctrl+Shift+B` (Linux/Windows)

### CLion

1. Open project folder
2. CLion automatically detects CMakeLists.txt
3. Build with `Cmd+F9` (macOS) or `Ctrl+F9` (Linux/Windows)

### Xcode

```bash
mkdir xcode-build && cd xcode-build
cmake -G Xcode ..
open FundNavCleaner.xcodeproj
```

## Performance Testing / 性能测试

To test with large datasets:

测试大型数据集：

```bash
# Build in Release mode
cmake -DCMAKE_BUILD_TYPE=Release ..
make

# Run demo (includes performance benchmarks)
./demo
```

## Next Steps / 后续步骤

After successful build:

成功构建后：

1. Run `./demo` to see the tool in action / 运行演示查看工具效果
2. Run `./run_tests` to verify all tests pass / 运行测试验证通过
3. Check generated CSV files in the build directory / 检查生成的CSV文件
4. Read [README.md](README.md) for usage examples / 阅读使用示例

## Support / 支持

If you encounter issues not covered here, please:

如果遇到此处未涵盖的问题，请：

1. Check CMake output for detailed error messages
2. Ensure all prerequisites are met
3. Try a clean build
4. Open an issue with build logs

检查CMake输出的详细错误信息，确保满足所有前置要求，尝试清理构建，或提交包含构建日志的issue。

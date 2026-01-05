# Quantitative Finance Algorithms in C++17 | C++17 量化金融算法

[English](#english) | [中文](#chinese)

---

<a name="english"></a>
## 🇬🇧 English

### Overview

This repository contains production-grade implementations of fundamental quantitative finance algorithms in modern C++17. The project demonstrates deep understanding of financial mathematics, numerical methods, and software engineering best practices.

**Two Core Modules:**
1. **Implied Volatility Solver** - Black-Scholes option pricing with Newton-Raphson and Brent's method
2. **Zero-Coupon Yield Curve Bootstrapping** - Interest rate curve construction from bond prices

### 🎯 Key Features

- **Built from First Principles**: All numerical methods implemented from scratch (no black-box libraries)
- **Production Quality**: Comprehensive error handling, numerical stability, edge case coverage
- **Fully Tested**: Google Test framework with 40+ unit tests
- **Modern C++17**: Clean architecture, RAII, smart pointers, STL algorithms
- **Well Documented**: Detailed mathematical derivations and implementation notes

### 📚 Projects

#### 1. Implied Volatility Solver

Solves for implied volatility from option market prices using:
- **Black-Scholes Pricing Model**: European call/put options
- **Greeks Calculation**: Delta, Gamma, Vega, Theta, Rho
- **Newton-Raphson Method**: Fast convergence (3-5 iterations)
- **Brent's Method Fallback**: Robust root-finding for edge cases
- **Advanced Features**: Volatility smile, put-call parity checks

**Use Cases:**
- Options trading and risk management
- Volatility surface construction
- Derivatives pricing and hedging

#### 2. Zero-Coupon Yield Curve Bootstrapping

Constructs zero-coupon interest rate curves from bond prices:
- **Bootstrapping Algorithm**: Recursive discount factor extraction
- **Multiple Compounding Conventions**: Continuous, annual, semi-annual, quarterly
- **Interpolation Methods**: Linear, log-linear, flat-forward
- **Cubic Spline Smoothing**: Natural spline with Thomas algorithm (O(n))
- **Forward Curve Calculation**: Discrete and instantaneous forward rates
- **Arbitrage Detection**: Validates curve for negative forward rates

**Use Cases:**
- Fixed income portfolio valuation
- Interest rate derivatives pricing
- Risk management (DV01, duration, convexity)
- Regulatory reporting (Basel III, Solvency II)

### 🛠️ Technical Stack

- **Language**: C++17
- **Build System**: CMake 3.14+
- **Testing**: Google Test
- **Compiler**: GCC 7+, Clang 5+, MSVC 2017+
- **Standards**: ISO C++17, IEEE 754 floating-point

### 📦 Project Structure

```
finance/
├── implied_volatility_solver/
│   ├── include/          # Header files
│   ├── src/              # Implementation
│   ├── tests/            # Unit tests
│   ├── README.md         # Detailed documentation
│   └── BUILD.md          # Build instructions
└── yield_curve_bootstrapping/
    ├── include/          # Header files
    ├── src/              # Implementation
    ├── tests/            # Unit tests
    ├── README.md         # Detailed documentation
    └── BUILD.md          # Build instructions
```

### 🚀 Quick Start

#### Prerequisites
```bash
# macOS
brew install cmake

# Ubuntu/Debian
sudo apt-get install cmake build-essential
```

#### Build and Run

**Implied Volatility Solver:**
```bash
cd implied_volatility_solver
mkdir build && cd build
cmake ..
make
./demo        # Run demonstrations
./run_tests   # Run test suite
```

**Yield Curve Bootstrapping:**
```bash
cd yield_curve_bootstrapping
mkdir build && cd build
cmake ..
make
./demo        # Run demonstrations
./run_tests   # Run test suite
```

### 📊 Performance

- **Implied Vol Solver**: 5-20 microseconds per option (Release build)
- **Curve Bootstrapping**: Microseconds per bond
- **Cubic Spline**: O(n) using Thomas algorithm
- **Memory Efficient**: Minimal allocations, cache-friendly

### 🎓 Educational Value

This project demonstrates:
- **Financial Mathematics**: Black-Scholes PDE, Greeks, yield curve theory
- **Numerical Methods**: Root-finding, interpolation, spline fitting
- **C++ Engineering**: RAII, templates, STL, modern best practices
- **Software Design**: Separation of concerns, testability, maintainability
- **Numerical Stability**: Handling edge cases, precision issues

### 📖 Documentation

Each project contains:
- **README.md**: Financial context, mathematical foundations, architecture
- **BUILD.md**: Compilation instructions, troubleshooting
- **Inline Comments**: Implementation details and formulas
- **Demo Programs**: Real-world usage examples

### 🧪 Testing

- **Unit Tests**: 40+ tests covering core functionality
- **Edge Cases**: Deep ITM/OTM, near expiry, extreme volatilities
- **Numerical Accuracy**: Validated against known solutions
- **Robustness**: Error handling, boundary conditions

### 🤝 Contributing

This is a portfolio/educational project. Feel free to:
- Report issues or bugs
- Suggest improvements
- Use as reference for learning

### 📄 License

MIT License - See individual project directories for details.

### 👤 Author

**Bryte** - Building quantitative finance tools with modern C++

- GitHub: [@HelloBryte](https://github.com/HelloBryte)
- Portfolio: [bento.me/brytebento](https://bento.me/brytebento)

### 🌟 Acknowledgments

Inspired by real-world experience in securities trading and quantitative development.

---

<a name="chinese"></a>
## 🇨🇳 中文

### 项目概述

本仓库包含使用现代 C++17 实现的生产级量化金融算法。项目展示了对金融数学、数值方法和软件工程最佳实践的深入理解。

**两个核心模块：**
1. **隐含波动率求解器** - Black-Scholes 期权定价与牛顿法、Brent 方法
2. **零息收益率曲线自举** - 从债券价格构建利率曲线

### 🎯 核心特性

- **从第一性原理构建**：所有数值方法从零实现（无黑盒库依赖）
- **生产级质量**：完善的错误处理、数值稳定性、边界情况覆盖
- **全面测试**：使用 Google Test 框架，40+ 单元测试
- **现代 C++17**：清晰架构、RAII、智能指针、STL 算法
- **文档完善**：详细的数学推导和实现说明

### 📚 项目介绍

#### 1. 隐含波动率求解器

从期权市场价格反推隐含波动率，使用：
- **Black-Scholes 定价模型**：欧式看涨/看跌期权
- **希腊字母计算**：Delta、Gamma、Vega、Theta、Rho
- **牛顿-拉弗森法**：快速收敛（3-5 次迭代）
- **Brent 方法回退**：边界情况的稳健求根
- **高级功能**：波动率微笑、看涨看跌平价检验

**应用场景：**
- 期权交易与风险管理
- 波动率曲面构建
- 衍生品定价与对冲

#### 2. 零息收益率曲线自举

从债券价格构建零息利率曲线：
- **自举算法**：递归提取贴现因子
- **多种复利约定**：连续复利、年复利、半年复利、季度复利
- **插值方法**：线性、对数线性、平坦远期
- **三次样条平滑**：自然样条与 Thomas 算法（O(n)）
- **远期曲线计算**：离散和瞬时远期利率
- **套利检测**：验证曲线是否存在负远期利率

**应用场景：**
- 固定收益组合估值
- 利率衍生品定价
- 风险管理（DV01、久期、凸性）
- 监管报告（巴塞尔协议 III、偿付能力 II）

### 🛠️ 技术栈

- **语言**：C++17
- **构建系统**：CMake 3.14+
- **测试框架**：Google Test
- **编译器**：GCC 7+、Clang 5+、MSVC 2017+
- **标准**：ISO C++17、IEEE 754 浮点运算

### 📦 项目结构

```
finance/
├── implied_volatility_solver/      # 隐含波动率求解器
│   ├── include/                    # 头文件
│   ├── src/                        # 实现文件
│   ├── tests/                      # 单元测试
│   ├── README.md                   # 详细文档
│   └── BUILD.md                    # 构建说明
└── yield_curve_bootstrapping/      # 收益率曲线自举
    ├── include/                    # 头文件
    ├── src/                        # 实现文件
    ├── tests/                      # 单元测试
    ├── README.md                   # 详细文档
    └── BUILD.md                    # 构建说明
```

### 🚀 快速开始

#### 环境要求
```bash
# macOS
brew install cmake

# Ubuntu/Debian
sudo apt-get install cmake build-essential
```

#### 构建与运行

**隐含波动率求解器：**
```bash
cd implied_volatility_solver
mkdir build && cd build
cmake ..
make
./demo        # 运行演示程序
./run_tests   # 运行测试套件
```

**收益率曲线自举：**
```bash
cd yield_curve_bootstrapping
mkdir build && cd build
cmake ..
make
./demo        # 运行演示程序
./run_tests   # 运行测试套件
```

### 📊 性能指标

- **隐含波动率求解**：每个期权 5-20 微秒（Release 构建）
- **曲线自举**：每个债券微秒级
- **三次样条**：使用 Thomas 算法，O(n) 复杂度
- **内存高效**：最小化内存分配，缓存友好

### 🎓 教育价值

本项目展示：
- **金融数学**：Black-Scholes 偏微分方程、希腊字母、收益率曲线理论
- **数值方法**：求根算法、插值、样条拟合
- **C++ 工程**：RAII、模板、STL、现代最佳实践
- **软件设计**：关注点分离、可测试性、可维护性
- **数值稳定性**：边界情况处理、精度问题

### 📖 文档说明

每个项目包含：
- **README.md**：金融背景、数学基础、架构设计
- **BUILD.md**：编译说明、故障排除
- **代码注释**：实现细节和公式
- **演示程序**：实际使用示例

### 🧪 测试覆盖

- **单元测试**：40+ 测试覆盖核心功能
- **边界情况**：深度实值/虚值、临近到期、极端波动率
- **数值精度**：与已知解对比验证
- **鲁棒性**：错误处理、边界条件

### 🤝 贡献

这是一个作品集/教育项目。欢迎：
- 报告问题或 bug
- 提出改进建议
- 作为学习参考

### 📄 许可证

MIT 许可证 - 详见各项目目录

### 👤 作者

**Bryte** - 使用现代 C++ 构建量化金融工具

- GitHub: [@HelloBryte](https://github.com/HelloBryte)
- 作品集: [bento.me/brytebento](https://bento.me/brytebento)

### 🌟 致谢

灵感来源于证券交易和量化开发的实际工作经验。

---

## 📈 Why This Project Matters | 为什么这个项目重要

### For Employers | 对雇主
- Demonstrates **quantitative finance expertise** and **C++ mastery**
- Shows ability to implement **complex algorithms from first principles**
- Proves **production-ready code quality** with comprehensive testing

### For Learners | 对学习者
- **Educational resource** for quantitative finance and C++ programming
- **Real-world implementations** of academic concepts
- **Best practices** in numerical computing and software engineering

### For Traders | 对交易员
- **Practical tools** for options trading and fixed income analysis
- **Transparent implementations** - understand every calculation
- **Extensible codebase** - customize for specific needs

---

**⭐ If you find this project useful, please consider giving it a star!**

**⭐ 如果你觉得这个项目有用，请考虑给它一个星标！**

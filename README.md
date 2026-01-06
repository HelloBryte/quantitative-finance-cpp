# Quantitative Finance Algorithms in C++17 | C++17 量化金融算法

[English](#english) | [中文](#chinese)

---

<a name="english"></a>
## 🇬🇧 English

### Overview
Production-grade implementations of fundamental quantitative finance algorithms in modern C++17. Focuses on financial mathematics, numerical methods, and software engineering.

### 🎯 Key Features
- **First Principles**: Numerical methods implemented from scratch.
- **Production Quality**: Robust error handling and numerical stability.
- **Fully Tested**: Google Test framework (40+ unit tests).
- **Clean Architecture**: Modern C++17, RAII, and STL algorithms.

### 📚 Core Modules

#### 1. Implied Volatility Solver
- **Models**: Black-Scholes pricing for European options.
- **Methods**: Newton-Raphson (fast) and Brent's method (fallback).
- **Features**: Greeks (Δ, Γ, ν, θ, ρ), volatility smile, put-call parity.

#### 2. Zero-Coupon Yield Curve Bootstrapping
- **Algorithm**: Recursive discount factor extraction from bond prices.
- **Interpolation**: Linear, log-linear, flat-forward.
- **Smoothing**: Cubic spline with Thomas algorithm (O(n)).
- **Analysis**: Forward rates calculation and arbitrage detection.

### 🚀 Quick Start
```bash
# Example for Yield Curve Bootstrapping
cd yield_curve_bootstrapping && mkdir build && cd build
cmake .. && make
./demo        # Run demonstrations
./run_tests   # Run tests
```

### 👤 Author
**Bryte** - [@HelloBryte](https://github.com/HelloBryte) | [bento.me/brytebento](https://bento.me/brytebento)

---

<a name="chinese"></a>
## 🇨🇳 中文

### 项目概述
使用现代 C++17 实现的高性能量化金融基础算法库。涵盖金融数学推导、数值分析方法及软件工程实践。

### 🎯 核心特性
- **自主实现**：所有数值算法从零构建，不依赖黑盒库。
- **生产级质量**：完善的错误处理与数值稳定性设计。
- **全面测试**：Google Test 框架支撑，40+ 单元测试覆盖。
- **现代 C++**：遵循 C++17 标准，采用 RAII 和 STL 算法。

### 📚 核心模块

#### 1. 隐含波动率求解器
- **模型**：欧式期权 Black-Scholes 定价。
- **算法**：牛顿迭代法（高速）与 Brent 方法（鲁棒回退）。
- **功能**：希腊字母计算、波动率微笑分析、平价公式检验。

#### 2. 零息收益率曲线自举
- **算法**：从债券价格递归提取贴现因子。
- **插值**：线性、对数线性、平坦远期。
- **平滑**：三次样条拟合（Thomas 算法，O(n) 复杂度）。
- **分析**：远期利率计算及无套利检验。

### 🚀 快速开始
```bash
# 以收益率曲线项目为例
cd yield_curve_bootstrapping && mkdir build && cd build
cmake .. && make
./demo        # 运行演示
./run_tests   # 运行测试
```

### 👤 作者
**Bryte** - [@HelloBryte](https://github.com/HelloBryte) | [bento.me/brytebento](https://bento.me/brytebento)

---
**⭐ If you find this project useful, please consider giving it a star!**


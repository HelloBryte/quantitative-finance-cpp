# Build Instructions

## Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.14 or higher
- Internet connection (for downloading Google Test)

## Quick Start

### Build and Run Demo

```bash
cd implied_volatility_solver
mkdir build
cd build
cmake ..
make
./demo
```

### Build and Run Tests

```bash
cd implied_volatility_solver
mkdir build
cd build
cmake ..
make
./run_tests
```

## Detailed Build Instructions

### 1. Configure the Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
```

Build types:
- `Release`: Optimized for performance (-O3)
- `Debug`: Includes debug symbols (-g -O0)

### 2. Compile

```bash
make -j$(nproc)  # Linux/macOS
make -j%NUMBER_OF_PROCESSORS%  # Windows
```

### 3. Run the Demo

```bash
./demo
```

Expected output: Demonstrations of implied volatility calculations, method comparisons, volatility smile, put-call parity, Greeks, and edge cases.

### 4. Run Tests

```bash
./run_tests
```

Or with verbose output:
```bash
./run_tests --gtest_verbose
```

Run specific tests:
```bash
./run_tests --gtest_filter=BlackScholesTest.*
./run_tests --gtest_filter=ImpliedVolSolverTest.RecoverKnownVolatilityATM
```

## Build Options

### Disable Tests

```bash
cmake .. -DBUILD_TESTS=OFF
make
```

### Custom Compiler

```bash
cmake .. -DCMAKE_CXX_COMPILER=g++-11
cmake .. -DCMAKE_CXX_COMPILER=clang++
```

### Install

```bash
sudo make install
```

## Project Structure

```
implied_volatility_solver/
├── CMakeLists.txt          # Build configuration
├── README.md               # Project documentation
├── BUILD.md                # This file
├── include/                # Header files
│   ├── option_types.hpp
│   ├── normal_distribution.hpp
│   ├── black_scholes.hpp
│   └── implied_vol_solver.hpp
├── src/                    # Implementation files
│   ├── option_types.cpp
│   ├── normal_distribution.cpp
│   ├── black_scholes.cpp
│   ├── implied_vol_solver.cpp
│   └── main.cpp
└── tests/                  # Test files
    ├── test_normal_distribution.cpp
    ├── test_black_scholes.cpp
    ├── test_implied_vol_solver.cpp
    └── test_edge_cases.cpp
```

## Troubleshooting

### CMake Version Too Old

```bash
# Ubuntu/Debian
sudo apt-get install cmake

# macOS
brew install cmake
```

### Compiler Not Found

```bash
# Ubuntu/Debian
sudo apt-get install build-essential

# macOS (install Xcode Command Line Tools)
xcode-select --install
```

### Google Test Download Fails

If behind a firewall, manually download Google Test:
```bash
cd build/_deps
git clone https://github.com/google/googletest.git
cd googletest
git checkout release-1.12.1
```

## Performance Notes

- Release builds are ~10x faster than Debug builds
- Newton-Raphson typically converges in 3-5 iterations
- Brent's method is more robust but ~2-3x slower
- Typical solve time: 5-20 microseconds per option (Release build)

## Integration into Your Project

### As a Static Library

```cmake
add_subdirectory(implied_volatility_solver)
target_link_libraries(your_target implied_vol_lib)
```

### Header-Only Usage

Copy `include/` and `src/` to your project and compile together.

## Next Steps

1. Review `README.md` for mathematical background
2. Run `./demo` to see capabilities
3. Run `./run_tests` to verify correctness
4. Integrate into your trading/risk system
5. Extend with American options, dividends, or exotic payoffs

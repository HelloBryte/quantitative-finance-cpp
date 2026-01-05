# Build Instructions

## Prerequisites

- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)
- CMake 3.14 or higher
- Internet connection (for downloading Google Test)

## Quick Start

### Build and Run Demo

```bash
cd yield_curve_bootstrapping
mkdir build
cd build
cmake ..
make
./demo
```

### Build and Run Tests

```bash
cd yield_curve_bootstrapping
mkdir build
cd build
cmake ..
make
./run_tests
```

## Project Structure

```
yield_curve_bootstrapping/
├── CMakeLists.txt          # Build configuration
├── README.md               # Project documentation
├── BUILD.md                # This file
├── include/                # Header files
│   ├── bond_types.hpp
│   ├── discount_factor.hpp
│   ├── interpolation.hpp
│   ├── cubic_spline.hpp
│   ├── yield_curve.hpp
│   ├── bootstrapper.hpp
│   └── forward_curve.hpp
├── src/                    # Implementation files
│   ├── bond_types.cpp
│   ├── discount_factor.cpp
│   ├── interpolation.cpp
│   ├── cubic_spline.cpp
│   ├── yield_curve.cpp
│   ├── bootstrapper.cpp
│   ├── forward_curve.cpp
│   └── main.cpp
└── tests/                  # Test files
    ├── test_discount_factor.cpp
    ├── test_interpolation.cpp
    ├── test_cubic_spline.cpp
    └── test_bootstrapper.cpp
```

## Build Options

### Release Build (Optimized)

```bash
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

### Debug Build

```bash
cmake .. -DCMAKE_BUILD_TYPE=Debug
make
```

### Disable Tests

```bash
cmake .. -DBUILD_TESTS=OFF
make
```

## Demo Output

The demo program showcases:
1. Basic yield curve bootstrapping
2. Interpolation method comparison
3. Forward rate curve calculation
4. Cubic spline smoothing
5. Compounding convention comparison
6. Arbitrage detection

## Performance Notes

- Release builds are ~10x faster than Debug builds
- Bootstrapping typically takes microseconds per bond
- Cubic spline fitting is O(n) using Thomas algorithm
- Interpolation lookup is O(log n) using binary search

## Integration

### As a Static Library

```cmake
add_subdirectory(yield_curve_bootstrapping)
target_link_libraries(your_target yield_curve_lib)
```

### Header-Only Usage

Copy `include/` and `src/` to your project and compile together.

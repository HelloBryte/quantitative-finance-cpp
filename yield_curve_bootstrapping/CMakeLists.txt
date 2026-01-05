cmake_minimum_required(VERSION 3.14)
project(YieldCurveBootstrapping VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

include_directories(include)

set(SOURCES
    src/bond_types.cpp
    src/discount_factor.cpp
    src/interpolation.cpp
    src/cubic_spline.cpp
    src/yield_curve.cpp
    src/bootstrapper.cpp
    src/forward_curve.cpp
)

add_library(yield_curve_lib STATIC ${SOURCES})

add_executable(demo src/main.cpp)
target_link_libraries(demo yield_curve_lib)

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    FetchContent_MakeAvailable(googletest)
    
    add_executable(run_tests
        tests/test_discount_factor.cpp
        tests/test_interpolation.cpp
        tests/test_cubic_spline.cpp
        tests/test_bootstrapper.cpp
    )
    target_link_libraries(run_tests yield_curve_lib gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(run_tests)
endif()

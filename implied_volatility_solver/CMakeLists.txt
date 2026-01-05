cmake_minimum_required(VERSION 3.14)
project(ImpliedVolatilitySolver VERSION 1.0.0 LANGUAGES CXX)

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
    src/option_types.cpp
    src/normal_distribution.cpp
    src/black_scholes.cpp
    src/implied_vol_solver.cpp
)

add_library(implied_vol_lib STATIC ${SOURCES})

add_executable(demo src/main.cpp)
target_link_libraries(demo implied_vol_lib)

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
        tests/test_normal_distribution.cpp
        tests/test_black_scholes.cpp
        tests/test_implied_vol_solver.cpp
        tests/test_edge_cases.cpp
    )
    target_link_libraries(run_tests implied_vol_lib gtest_main)
    
    include(GoogleTest)
    gtest_discover_tests(run_tests)
endif()

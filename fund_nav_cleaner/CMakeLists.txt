cmake_minimum_required(VERSION 3.14)
project(FundNavCleaner VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set SDK path for macOS
if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE CMAKE_OSX_SYSROOT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_OSX_SYSROOT "${CMAKE_OSX_SYSROOT}" CACHE PATH "macOS SDK path" FORCE)
endif()

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/nav_types.cpp
    src/nav_validator.cpp
    src/interpolation.cpp
    src/knn_imputer.cpp
    src/nav_cleaner.cpp
    src/csv_handler.cpp
)

# Create static library
add_library(nav_cleaner_lib STATIC ${SOURCES})

# Demo executable
add_executable(demo src/main.cpp)
target_link_libraries(demo nav_cleaner_lib)

# Option to build tests
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    # Fetch GoogleTest
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG release-1.12.1
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    enable_testing()

    # Test files
    set(TEST_SOURCES
        tests/test_nav_validator.cpp
        tests/test_interpolation.cpp
        tests/test_knn_imputer.cpp
        tests/test_nav_cleaner.cpp
        tests/test_edge_cases.cpp
    )

    add_executable(run_tests ${TEST_SOURCES})
    target_link_libraries(run_tests nav_cleaner_lib gtest_main)

    include(GoogleTest)
    gtest_discover_tests(run_tests)
endif()

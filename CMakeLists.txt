cmake_minimum_required(VERSION 3.20)

project(JSONQL LANGUAGES CXX)

# Obtain gtest so we can test our files adequately
include(FetchContent)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.17.0.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB SRC_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB TEST_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cpp"
)

add_executable(JSONQL
    ${SRC_FILES}
    ${TEST_FILES}
)

add_executable(jsonquery
    main.cpp
    ${SRC_FILES}
)

add_executable(benchmark
    benchmark.cpp
    ${SRC_FILES}
)

target_include_directories(JSONQL PRIVATE include)
target_include_directories(jsonquery PRIVATE include)
target_include_directories(benchmark PRIVATE include)

# Absolute path to the sample JSON files so tests do not depend on the working
# directory they are run from.
target_compile_definitions(JSONQL PRIVATE
    JSON_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/jsonFiles")

target_compile_definitions(benchmark PRIVATE
    JSON_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/jsonFiles")

target_link_libraries(JSONQL PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(JSONQL)

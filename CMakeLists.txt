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
set(CMAKE_BUILD_TYPE Debug)

add_executable(JSONQLTests
    src/queryparser.cpp
    tests/parsertest.cpp
)

target_include_directories(JSONQLTests PRIVATE src)

target_link_libraries(JSONQLTests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(JSONQLTests)
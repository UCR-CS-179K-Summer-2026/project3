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
    src/jsonparser.cpp
    tests/parsertest.cpp
    tests/sprint1test.cpp
)

target_include_directories(JSONQLTests PRIVATE src)

# Absolute path to the sample JSON files so tests do not depend on the working
# directory they are run from.
target_compile_definitions(JSONQLTests PRIVATE
    JSON_DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/src/jsonFiles")

target_link_libraries(JSONQLTests PRIVATE GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(JSONQLTests)
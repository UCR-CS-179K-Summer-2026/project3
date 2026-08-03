#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "queryparser.h"
#include "jsonparser.h"

// This test opens employee.json from the jsonFiles folder.
// It checks that the stream is open and that json_parser::test() returns 0 for success.
TEST(JsonParser, OpensEmployeeFile) {
    std::ifstream json(JSON_DATA_DIR "/employee.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

// This test opens product.json from the jsonFiles folder.
// It checks that the stream is open and that json_parser::test() returns 0 for success.
TEST(JsonParser, OpensProductFile) {
    std::ifstream json(JSON_DATA_DIR "/product.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

// This test opens university.json from the jsonFiles folder.
// It checks that the stream is open and that json_parser::test() returns 0 for success.
TEST(JsonParser, OpensUniversityFile) {
    std::ifstream json(JSON_DATA_DIR "/university.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

// This test tries to open a file that does not exist.
// It checks that the stream remains closed and that json_parser::test() returns 1.
TEST(JsonParser, RejectsMissingFile) {
    std::ifstream json(JSON_DATA_DIR "/missing.json");

    EXPECT_FALSE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 1);
}

// This test creates an input stream using an empty filename.
// It checks that no file opens and that json_parser::test() returns 1.
TEST(JsonParser, RejectsEmptyFilename) {
    std::ifstream json("");

    EXPECT_FALSE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 1);
}

// This test calls json_parser::test() twice using the same open file stream.
// It checks that the function continues returning 0 while the stream remains open.
TEST(JsonParser, RepeatedCheckOfOpenFileSucceeds) {
    std::ifstream json(JSON_DATA_DIR "/employee.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
    EXPECT_EQ(json_parser::test(json), 0);
}

// Reads employee.json and checks that jsonToString() copies the file contents
// into a non-empty string containing expected employee data.
TEST(JsonParser, ConvertsEmployeeFileToString) {
    std::ifstream file(JSON_DATA_DIR "/employee.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    EXPECT_FALSE(jsonText.empty());
    EXPECT_NE(jsonText.find("\"employees\""), std::string::npos);
    EXPECT_NE(jsonText.find("\"Laura\""), std::string::npos);
}

// Follows employees -> 0 -> name and checks that the first employee is Laura.
// Strings are returned as raw JSON text, so the quotation marks are included.
TEST(JsonParser, FindsFirstEmployeeName) {
    std::ifstream file(JSON_DATA_DIR "/employee.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "employees",
        "0",
        "name"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"Laura\"");
}

// Follows employees -> 2 -> salary and checks that numeric values can be read
// from an object stored inside an array.
TEST(JsonParser, FindsEmployeeSalary) {
    std::ifstream file(JSON_DATA_DIR "/employee.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "employees",
        "2",
        "salary"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "95000");
}

// Follows employees -> 0 -> skills -> 1 and checks that the parser can move
// through an object, an array, and then another array.
TEST(JsonParser, FindsEmployeeSkill) {
    std::ifstream file(JSON_DATA_DIR "/employee.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "employees",
        "0",
        "skills",
        "1"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"Git\"");
}

// Follows products -> 0 -> price and checks that a decimal JSON number
// is returned correctly from product.json.
TEST(JsonParser, FindsProductPrice) {
    std::ifstream file(JSON_DATA_DIR "/product.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "products",
        "0",
        "price"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "89.99");
}

// Follows university -> location -> city and checks that the parser can
// retrieve a string from nested objects.
TEST(JsonParser, FindsUniversityCity) {
    std::ifstream file(JSON_DATA_DIR "/university.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "university",
        "location",
        "city"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"Riverside\"");
}

// Follows university -> departments -> 0 -> courses -> 1 and checks that
// the parser can traverse multiple nested objects and arrays.
TEST(JsonParser, FindsUniversityCourse) {
    std::ifstream file(JSON_DATA_DIR "/university.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "university",
        "departments",
        "0",
        "courses",
        "1"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"CS 179K\"");
}

// Requests a field that does not exist and checks that parsejson()
// returns an empty string_view instead of an incorrect value.
TEST(JsonParser, ReturnsEmptyForMissingKey) {
    std::ifstream file(JSON_DATA_DIR "/employee.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "employees",
        "0",
        "age"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_TRUE(result.empty());
}
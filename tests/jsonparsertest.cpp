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

// ...
TEST(JsonParser, TestEscapeChar) {
    std::ifstream file(JSON_DATA_DIR "/test.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "\\\"a"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "1");
}

TEST(JsonParser, UnicodeKey) {
    std::ifstream file(JSON_DATA_DIR "/oddKeys.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "a"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"Bob\"");
}

TEST(JsonParser, UnicodeKeyAndSubkey) {
    std::ifstream file(JSON_DATA_DIR "/oddKeys.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "b", 
        "c"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"Apple\"");
}


TEST(JsonParser, UnicodeKeyAndSubkeyAndValue) {
    std::ifstream file(JSON_DATA_DIR "/oddKeys.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "b", 
        "d"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"a\"");
}

// Looks up a key written directly as UTF-8 rather than as \uXXXX escapes.
// Every byte of a multi-byte character is above 0x7F, so the scanner cannot
// mistake one for a quote or a brace and the text survives unchanged.
TEST(JsonParser, FindsAccentedKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "naïve"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"café\"");
}

// Checks that a Cyrillic key and value pass through unchanged.
TEST(JsonParser, FindsCyrillicKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "привет"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"мир\"");
}

// Checks that a right-to-left script is carried through by byte order, which
// is the order it is stored in regardless of how it is displayed.
TEST(JsonParser, FindsArabicKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "مرحبا"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"العالم\"");
}

// Checks a Greek key whose value is made of mathematical symbols, so the
// value is non-ASCII as well as the key.
TEST(JsonParser, FindsGreekKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "αβγ"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"∞≠√\"");
}

// Checks a CJK key, whose characters are three UTF-8 bytes each.
TEST(JsonParser, FindsCjkKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "城市"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"東京\"");
}

// Emoji sit outside the Basic Multilingual Plane and take four UTF-8 bytes,
// which is the widest character the scanner has to carry through.
TEST(JsonParser, FindsEmojiKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "emoji😀"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"🌍\"");
}

// Checks a key that mixes a script with an emoji, so characters of two
// different byte widths sit inside one key.
TEST(JsonParser, FindsMixedWidthKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "ключ🔑"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"значение✨\"");
}

// Requests the ASCII spelling of an accented key and checks that it does not
// match, so the tests above are comparing whole keys and not prefixes.
TEST(JsonParser, RejectsUnaccentedKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "naive"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_TRUE(result.empty());
}

// A \uXXXX escape naming a character above 0x7F is written out as the two
// UTF-8 bytes that encode it.
TEST(JsonParser, DecodesTwoByteEscape) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "café"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"naïve\"");
}

// Characters higher up the range encode to three UTF-8 bytes.
TEST(JsonParser, DecodesThreeByteEscape) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "日本"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"東京\"");
}

// A character above U+FFFF is spelled as a surrogate pair: two escapes that
// together name one character and encode to four UTF-8 bytes.
TEST(JsonParser, DecodesSurrogatePair) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "🔑"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"😀\"");
}

// An escaped key and the same key written directly are the same key, so a
// query can be written either way.
TEST(JsonParser, EscapedKeyMatchesRawKey) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "é"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "1");
}

// A leading surrogate with no partner names no character, so it is left as
// literal text rather than turned into a stand-in character.
TEST(JsonParser, LeavesUnpairedLeadSurrogateAsText) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "lead"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, R"("\ud800")");
}

// A trailing surrogate that never followed a leading one is left alone for
// the same reason.
TEST(JsonParser, LeavesUnpairedTrailSurrogateAsText) {
    std::ifstream file(JSON_DATA_DIR "/unicode.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "trail"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, R"("\udc00")");
}

TEST(JsonParser, nullKey) {
    std::ifstream file(JSON_DATA_DIR "/oddKeys.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "\0a"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\"a\"");
}
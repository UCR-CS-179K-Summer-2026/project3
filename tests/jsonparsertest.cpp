#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "queryparser.h"
#include "jsonparser.h"

namespace {
constexpr auto EMPLOYEE_FILE =
    JSON_DATA_DIR "/basic/employee.json";

constexpr auto PRODUCT_FILE =
    JSON_DATA_DIR "/basic/product.json";

constexpr auto UNIVERSITY_FILE =
    JSON_DATA_DIR "/basic/university.json";

constexpr auto ESCAPED_QUOTE_FILE =
    JSON_DATA_DIR "/unicode/escapedquote.json";

constexpr auto ODD_KEYS_FILE =
    JSON_DATA_DIR "/unicode/oddKeys.json";

constexpr auto UNICODE_FILE =
    JSON_DATA_DIR "/unicode/unicode.json";

constexpr auto EMBEDDED_NULL_FILE =
    JSON_DATA_DIR "/edgecases/embeddednull.json";

constexpr auto LITERAL_TYPES_FILE =
    JSON_DATA_DIR "/edgecases/literaltypes.json";

constexpr auto EMPTY_STRUCTURES_FILE =
    JSON_DATA_DIR "/edgecases/emptystructures.json";

// Reads a fixture once and returns its full JSON text.
// ASSERT_TRUE stays in each test so a missing fixture gives a clear failure.
std::string readJsonFile(const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return {};
    }
    return json_parser::jsonToString(file);
}

// GoogleTest uses this field to give each parameterized case a readable name.
template <typename ParamType>
std::string caseName(const ::testing::TestParamInfo<ParamType>& info) {
    return info.param.name;
}

// ====================
// Basic file handling
// ====================

struct FileOpenCase {
    const char* name;
    const char* filename;
};

class JsonFileOpenTest : public ::testing::TestWithParam<FileOpenCase> {};

// These cases use the same behavior with different basic fixtures, so one
// parameterized test avoids repeating identical open/test assertions.
TEST_P(JsonFileOpenTest, OpensBasicFixture) {
    const auto& test = GetParam();
    std::ifstream json(test.filename);

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

INSTANTIATE_TEST_SUITE_P(
    BasicFiles,
    JsonFileOpenTest,
    ::testing::Values(
        // Verifies the employee fixture opens successfully.
        FileOpenCase{"OpensEmployeeFile", EMPLOYEE_FILE},
        // Verifies the product fixture opens successfully.
        FileOpenCase{"OpensProductFile", PRODUCT_FILE},
        // Verifies the university fixture opens successfully.
        FileOpenCase{"OpensUniversityFile", UNIVERSITY_FILE}
    ),
    caseName<FileOpenCase>
);

// This test tries to open a file that does not exist.
// It checks that the stream remains closed and json_parser::test() returns 1.
TEST(JsonParser, RejectsMissingFile) {
    std::ifstream json(JSON_DATA_DIR "/missing.json");

    EXPECT_FALSE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 1);
}

// This test creates an input stream using an empty filename.
// It checks that no file opens and json_parser::test() returns 1.
TEST(JsonParser, RejectsEmptyFilename) {
    std::ifstream json("");

    EXPECT_FALSE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 1);
}

// Calls json_parser::test() twice on the same open stream.
// The stream should remain valid and both calls should report success.
TEST(JsonParser, RepeatedCheckOfOpenFileSucceeds) {
    std::ifstream json(EMPLOYEE_FILE);

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
    EXPECT_EQ(json_parser::test(json), 0);
}

// Reads employee.json and checks that jsonToString() copies the file contents
// into a non-empty string containing expected employee data.
TEST(JsonParser, ConvertsEmployeeFileToString) {
    std::ifstream file(EMPLOYEE_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    EXPECT_FALSE(jsonText.empty());
    EXPECT_NE(jsonText.find("\"employees\""), std::string::npos);
    EXPECT_NE(jsonText.find("\"Laura\""), std::string::npos);
}

// ====================
// Basic path traversal
// ====================

struct ParseCase {
    const char* name;
    const char* filename;
    std::vector<std::string> path;
    std::string expected;
};

class JsonBasicParseTest : public ::testing::TestWithParam<ParseCase> {};

// Each case follows a different valid path through a basic fixture.
// The shared test checks that the fixture opens and parsejson() returns exactly
// the JSON text expected for that path.
TEST_P(JsonBasicParseTest, ReturnsExpectedValue) {
    const auto& test = GetParam();
    std::ifstream file(test.filename);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::string result = json_parser::parsejson(jsonText, test.path);

    EXPECT_EQ(result, test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    BasicTraversal,
    JsonBasicParseTest,
    ::testing::Values(
        // Follows employees -> 0 -> name and returns the first employee name.
        ParseCase{
            "FindsFirstEmployeeName",
            EMPLOYEE_FILE,
            {"employees", "0", "name"},
            "\"Laura\""
        },
        // Follows employees -> 2 -> salary and reads a number from an object in an array.
        ParseCase{
            "FindsEmployeeSalary",
            EMPLOYEE_FILE,
            {"employees", "2", "salary"},
            "95000"
        },
        // Follows employees -> 0 -> skills -> 1 through nested arrays/objects.
        ParseCase{
            "FindsEmployeeSkill",
            EMPLOYEE_FILE,
            {"employees", "0", "skills", "1"},
            "\"Git\""
        },
        // Follows products -> 0 -> price and preserves the decimal value.
        ParseCase{
            "FindsProductPrice",
            PRODUCT_FILE,
            {"products", "0", "price"},
            "89.99"
        },
        // Follows university -> location -> city through nested objects.
        ParseCase{
            "FindsUniversityCity",
            UNIVERSITY_FILE,
            {"university", "location", "city"},
            "\"Riverside\""
        },
        // Follows university -> departments -> 0 -> courses -> 1 through deep nesting.
        ParseCase{
            "FindsUniversityCourse",
            UNIVERSITY_FILE,
            {"university", "departments", "0", "courses", "1"},
            "\"CS 179K\""
        }
    ),
    caseName<ParseCase>
);

// Requests a field that does not exist.
// parsejson() should return an empty result instead of an incorrect value.
TEST(JsonParser, ReturnsEmptyForMissingKey) {
    std::ifstream file(EMPLOYEE_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "employees",
        "0",
        "age"
    };

    const std::string result = json_parser::parsejson(jsonText, path);
    EXPECT_TRUE(result.empty());
}

// ====================
// Unicode and escaped-key fixtures
// ====================

// Looks up a key containing an escaped quotation mark.
// The fixture was renamed from test.json so its purpose is clear.
TEST(JsonParser, TestEscapeChar) {
    std::ifstream file(ESCAPED_QUOTE_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {"\\\"a"};

    const std::string result = json_parser::parsejson(jsonText, path);
    EXPECT_EQ(result, "1");
}

struct OddKeyCase {
    const char* name;
    std::vector<std::string> path;
    std::string expected;
};

class JsonOddKeyTest : public ::testing::TestWithParam<OddKeyCase> {};

// These cases all use oddKeys.json and differ only in the path and expected
// result, so the table makes the key/subkey coverage easy to scan.
TEST_P(JsonOddKeyTest, ReturnsExpectedValue) {
    const auto& test = GetParam();
    std::ifstream file(ODD_KEYS_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::string result = json_parser::parsejson(jsonText, test.path);

    EXPECT_EQ(result, test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    OddKeys,
    JsonOddKeyTest,
    ::testing::Values(
        // Reads a top-level key from the odd-key fixture.
        OddKeyCase{"UnicodeKey", {"a"}, "\"Bob\""},
        // Reads a nested subkey b -> c.
        OddKeyCase{"UnicodeKeyAndSubkey", {"b", "c"}, "\"Apple\""},
        // Reads a second nested value b -> d.
        OddKeyCase{"UnicodeKeyAndSubkeyAndValue", {"b", "d"}, "\"a\""}
    ),
    caseName<OddKeyCase>
);

struct UnicodeCase {
    const char* name;
    std::vector<std::string> path;
    std::string expected;
    bool expectEmpty;
};

class JsonUnicodeTest : public ::testing::TestWithParam<UnicodeCase> {};

// All cases below exercise the same unicode.json fixture. Keeping the data in
// one table makes script, byte-width, escape, and surrogate coverage explicit.
TEST_P(JsonUnicodeTest, HandlesUnicodeCase) {
    const auto& test = GetParam();
    std::ifstream file(UNICODE_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::string result = json_parser::parsejson(jsonText, test.path);

    if (test.expectEmpty) {
        EXPECT_TRUE(result.empty());
    } else {
        EXPECT_EQ(result, test.expected);
    }
}

INSTANTIATE_TEST_SUITE_P(
    UnicodeCases,
    JsonUnicodeTest,
    ::testing::Values(
        // Direct UTF-8 accented key/value lookup.
        UnicodeCase{"FindsAccentedKey", {"naïve"}, "\"café\"", false},
        // Cyrillic key/value should pass through unchanged.
        UnicodeCase{"FindsCyrillicKey", {"привет"}, "\"мир\"", false},
        // Right-to-left Arabic text should preserve stored byte order.
        UnicodeCase{"FindsArabicKey", {"مرحبا"}, "\"العالم\"", false},
        // Greek key with non-ASCII mathematical-symbol value.
        UnicodeCase{"FindsGreekKey", {"αβγ"}, "\"∞≠√\"", false},
        // CJK characters exercise three-byte UTF-8 sequences.
        UnicodeCase{"FindsCjkKey", {"城市"}, "\"東京\"", false},
        // Emoji exercise four-byte UTF-8 sequences outside the BMP.
        UnicodeCase{"FindsEmojiKey", {"emoji😀"}, "\"🌍\"", false},
        // Mixed Cyrillic and emoji checks different byte widths in one key.
        UnicodeCase{"FindsMixedWidthKey", {"ключ🔑"}, "\"значение✨\"", false},
        // ASCII spelling must not match the accented key "naïve".
        UnicodeCase{"RejectsUnaccentedKey", {"naive"}, "", true},
        // A \uXXXX escape that becomes a two-byte UTF-8 character.
        UnicodeCase{"DecodesTwoByteEscape", {"café"}, "\"naïve\"", false},
        // A \uXXXX escape that becomes a three-byte UTF-8 character.
        UnicodeCase{"DecodesThreeByteEscape", {"日本"}, "\"東京\"", false},
        // A surrogate pair should decode to one four-byte UTF-8 character.
        UnicodeCase{"DecodesSurrogatePair", {"🔑"}, "\"😀\"", false},
        // Escaped and raw forms of the same key should match.
        UnicodeCase{"EscapedKeyMatchesRawKey", {"é"}, "1", false},
        // An unpaired leading surrogate is preserved as literal text.
        UnicodeCase{"LeavesUnpairedLeadSurrogateAsText", {"lead"}, R"("\ud800")", false},
        // An unpaired trailing surrogate is preserved as literal text.
        UnicodeCase{"LeavesUnpairedTrailSurrogateAsText", {"trail"}, R"("\udc00")", false}
    ),
    caseName<UnicodeCase>
);

// Keeps the original null-key case separate because its string literal has
// embedded-null behavior that is clearer to inspect outside a case table.
TEST(JsonParser, nullKey) {
    std::ifstream file(ODD_KEYS_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "\0a"
    };

    const std::string result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "\0a\"");
}

// ====================
// File-open helper behavior
// ====================

// Calls isFileOpen() on employee.json, which is already open.
// It should return 0, matching json_parser::test() for a valid stream.
TEST(JsonParser, IsFileOpenAgreesWithTestForOpenFile) {
    std::ifstream json(EMPLOYEE_FILE);

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::isFileOpen(json), 0);
}

// Calls isFileOpen() on a missing file.
// It should return 1, matching json_parser::test() for an invalid stream.
TEST(JsonParser, IsFileOpenAgreesWithTestForMissingFile) {
    std::ifstream json(JSON_DATA_DIR "/missing.json");

    EXPECT_FALSE(json.is_open());
    EXPECT_EQ(json_parser::isFileOpen(json), 1);
}

// ====================
// Edge-case fixtures
// ====================

// Reads a \u0000 escape from embeddednull.json.
// parsejson() decodes it to an actual embedded null byte in the returned string.
TEST(JsonParser, HandlesEscapedNullSequenceInValue) {
    std::ifstream file(EMBEDDED_NULL_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::string result =
        json_parser::parsejson(jsonText, {"null_escape"});

    // Explicit length keeps the embedded null instead of treating it as a terminator.
    const std::string expected("\"abc\0def\"", 9);
    EXPECT_EQ(result.size(), expected.size());
    EXPECT_EQ(result, expected);
}

// Reads a value containing an actual embedded null byte.
// The parser should return the text on both sides instead of truncating early.
TEST(JsonParser, DoesNotTruncateAtEmbeddedRawNullByte) {
    std::ifstream file(EMBEDDED_NULL_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::string result =
        json_parser::parsejson(jsonText, {"embedded_raw_null"});

    // Explicit length preserves the embedded null in the expected string.
    const std::string expected("\"abc\0def\"", 9);
    EXPECT_EQ(result.size(), expected.size());
    EXPECT_EQ(result, expected);
}

// This test looks up a key whose value is the JSON literal null.
// It checks that parsejson() returns the text "null" rather than an empty result.
TEST(JsonParser, ReturnsNullLiteralForNullValue) {
    std::ifstream file(LITERAL_TYPES_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    EXPECT_EQ(json_parser::parsejson(jsonText, {"null_literal"}), "null");
}

// Checks both a negative decimal and exponent notation in the same fixture.
// Both numbers should be returned exactly as they are written in the JSON.
TEST(JsonParser, HandlesNegativeAndExponentNumbers) {
    std::ifstream file(LITERAL_TYPES_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    EXPECT_EQ(json_parser::parsejson(jsonText, {"negative_num"}), "-273.15");
    EXPECT_EQ(json_parser::parsejson(jsonText, {"exponent_num"}), "6.022e23");
}

// This test looks up a JSON boolean literal.
// parsejson() should return the literal text "true".
TEST(JsonParser, HandlesBooleanLiteral) {
    std::ifstream file(LITERAL_TYPES_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    EXPECT_EQ(json_parser::parsejson(jsonText, {"bool_true"}), "true");
}

struct EmptyStructureCase {
    const char* name;
    std::vector<std::string> path;
};

class JsonEmptyStructureTest
    : public ::testing::TestWithParam<EmptyStructureCase> {};

// Both cases intentionally request a value that cannot exist in an empty
// structure. The parser should return an empty result rather than an invalid one.
TEST_P(JsonEmptyStructureTest, ReturnsEmptyForInvalidLookup) {
    const auto& test = GetParam();
    std::ifstream file(EMPTY_STRUCTURES_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    EXPECT_TRUE(json_parser::parsejson(jsonText, test.path).empty());
}

INSTANTIATE_TEST_SUITE_P(
    EmptyStructures,
    JsonEmptyStructureTest,
    ::testing::Values(
        // Requests index 0 from an empty array.
        EmptyStructureCase{"ReturnsEmptyForIndexIntoEmptyArray", {"empty_array", "0"}},
        // Requests a key from an empty object.
        EmptyStructureCase{"ReturnsEmptyForKeyIntoEmptyObject", {"empty_object", "anything"}}
    ),
    caseName<EmptyStructureCase>
);
}
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "queryparser.h"
#include "jsonparser.h"

#ifndef JSON_DATA_DIR
#define JSON_DATA_DIR "jsonFiles"
#endif

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

constexpr auto JSONL_TESTING_FILE =
    JSON_DATA_DIR "/jsonl/testing.jsonl";

constexpr auto JSONL_ACADEMIA_FILE =
    JSON_DATA_DIR "/jsonl/academia.jsonl";

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
// FIND queries
// ====================

struct FindCase {
    const char* name;
    const char* filename;
    const char* query;
    const char* expected;
};

class JsonFindTest : public ::testing::TestWithParam<FindCase> {};

// Each case parses a real FIND query and checks the reported answer.
// FIND reports whether the targets exist, so the result is "true" or "false"
// rather than the value stored there.
TEST_P(JsonFindTest, ReportsWhetherTargetsExist) {
    const auto& test = GetParam();
    std::ifstream file(test.filename);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    queryparser::parsequery(test.query);

    const std::string result =
        json_parser::parsejson(jsonText, queryparser::getparsedquery());

    EXPECT_EQ(result, test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    FindQueries,
    JsonFindTest,
    ::testing::Values(
        // Nesting descends, so this walks employees -> 0 -> name.
        FindCase{
            "FindsNestedPath",
            EMPLOYEE_FILE,
            R"(FIND {"employees" {"0" {"name"}}})",
            "true"
        },
        // The same names written side by side are three targets at the root
        // instead of a descent, and only "employees" is a root key.
        FindCase{
            "SiblingsAreNotAPath",
            EMPLOYEE_FILE,
            R"(FIND {"employees" "0" "name"})",
            "false"
        },
        // Both siblings are root keys, so the group holds.
        FindCase{
            "FindsSiblingsAtSameLevel",
            EMPLOYEE_FILE,
            R"(FIND {"employees" "company_info"})",
            "true"
        },
        // Every target has to exist, so one missing sibling fails the group.
        FindCase{
            "RejectsGroupWithMissingSibling",
            EMPLOYEE_FILE,
            R"(FIND {"employees" "budget"})",
            "false"
        },
        // A nested group descends from the sibling right before it.
        FindCase{
            "NestedGroupDescendsFromPrecedingSibling",
            EMPLOYEE_FILE,
            R"(FIND {"employees" "company_info" {"remote_work"}})",
            "true"
        },
        // Swapping those siblings points the group at "employees" instead, and an
        // array holds no key by that name.
        FindCase{
            "NestedGroupIgnoresEarlierSiblings",
            EMPLOYEE_FILE,
            R"(FIND {"company_info" "employees" {"remote_work"}})",
            "false"
        },
        // With no sibling to descend from, a leading group stays at this level.
        FindCase{
            "LeadingGroupStaysAtCurrentLevel",
            EMPLOYEE_FILE,
            R"(FIND {{"employees" "company_info"}})",
            "true"
        },
        // Siblings work the same way once a descent has happened.
        FindCase{
            "FindsSiblingsInsideDescent",
            EMPLOYEE_FILE,
            R"(FIND {"employees" {"0" {"name" "salary"}}})",
            "true"
        },
        // One missing target deep inside a descent still fails the whole query.
        FindCase{
            "RejectsMissingSiblingInsideDescent",
            EMPLOYEE_FILE,
            R"(FIND {"employees" {"0" {"name" "age"}}})",
            "false"
        },
        // A single top-level key.
        FindCase{
            "FindsTopLevelKey",
            EMPLOYEE_FILE,
            R"(FIND {"employees"})",
            "true"
        },
        // A key that is not in the object.
        FindCase{
            "RejectsMissingKey",
            EMPLOYEE_FILE,
            R"(FIND {"employees" {"0" {"age"}}})",
            "false"
        },
        // An index past the end of the array.
        FindCase{
            "RejectsIndexPastEndOfArray",
            EMPLOYEE_FILE,
            R"(FIND {"employees" {"10" {"name"}}})",
            "false"
        },
        // Index 0 of an empty array names no value.
        FindCase{
            "RejectsIndexIntoEmptyArray",
            EMPTY_STRUCTURES_FILE,
            R"(FIND {"empty_array" {"0"}})",
            "false"
        },
        // A key inside an empty object names no value.
        FindCase{
            "RejectsKeyIntoEmptyObject",
            EMPTY_STRUCTURES_FILE,
            R"(FIND {"empty_object" {"anything"}})",
            "false"
        },
        // "" is a legal key, so the empty quoted token must reach its value.
        FindCase{
            "FindsEmptyKey",
            EMPTY_STRUCTURES_FILE,
            R"(FIND {""})",
            "true"
        },
        // A value spelled with \uXXXX escapes is still found by its decoded key.
        FindCase{
            "FindsUnicodeKey",
            UNICODE_FILE,
            R"(FIND {"日本"})",
            "true"
        },
        // A key holding the literal null exists, even though it stores no data.
        FindCase{
            "FindsNullValuedKey",
            LITERAL_TYPES_FILE,
            R"(FIND {"null_literal"})",
            "true"
        }
    ),
    caseName<FindCase>
);

// A command the JSON parser does not run yet should say so rather than
// quietly reporting a wrong answer.
TEST(JsonParser, RejectsUnsupportedCommand) {
    std::ifstream file(EMPLOYEE_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    queryparser::parsequery(R"(ALLOF {"employees" {"name"}})");

    EXPECT_THROW(
        json_parser::parsejson(jsonText, queryparser::getparsedquery()),
        std::runtime_error);
}

// ====================
// DISPLAY queries
// ====================

struct DisplayCase {
    const char* name;
    const char* filename;
    const char* query;
    const char* expected;
};

class JsonDisplayTest : public ::testing::TestWithParam<DisplayCase> {};

// Each case parses a real DISPLAY query and checks the text it returns.
// A query that names nothing that exists returns an empty result.
TEST_P(JsonDisplayTest, ReturnsNamedValues) {
    const auto& test = GetParam();
    std::ifstream file(test.filename);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    queryparser::parsequery(test.query);

    const std::string result =
        json_parser::parsejson(jsonText, queryparser::getparsedquery());

    EXPECT_EQ(result, test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    DisplayQueries,
    JsonDisplayTest,
    ::testing::Values(
        // The keys along the way are waypoints, so only the leaf is shown.
        DisplayCase{
            "ShowsValueAtNestedPath",
            EMPLOYEE_FILE,
            R"(DISPLAY {"employees" {"0" {"name"}}})",
            "\"Laura\""
        },
        // The README's second DISPLAY example.
        DisplayCase{
            "ShowsDepartmentOfSecondEmployee",
            EMPLOYEE_FILE,
            R"(DISPLAY {"employees" {"1" {"department"}}})",
            "\"IT Support\""
        },
        // A descent through plain objects rather than an array.
        DisplayCase{
            "ShowsValueInNestedObject",
            EMPLOYEE_FILE,
            R"(DISPLAY {"company_info" {"company_name"}})",
            "\"UC Riverside\""
        },
        // A descent that ends in an array index.
        DisplayCase{
            "ShowsValueAtArrayIndex",
            EMPLOYEE_FILE,
            R"(DISPLAY {"employees" {"0" {"skills" {"1"}}}})",
            "\"Git\""
        },
        // Two leaves under the same waypoint come back as a list.
        DisplayCase{
            "ShowsSiblingLeavesAsList",
            EMPLOYEE_FILE,
            R"(DISPLAY {"employees" {"0" {"name" "salary"}}})",
            "[\"Laura\", 90000]"
        },
        // Siblings at the root need no descent at all.
        DisplayCase{
            "ShowsRootSiblingsAsList",
            LITERAL_TYPES_FILE,
            R"(DISPLAY {"bool_true" "null_literal"})",
            "[true, null]"
        },
        // A leading group is evaluated where it sits, so this reads the same way.
        DisplayCase{
            "ShowsLeadingGroupAtCurrentLevel",
            LITERAL_TYPES_FILE,
            R"(DISPLAY {{"bool_true" "negative_num"}})",
            "[true, -273.15]"
        },
        // The key and the value are both spelled with escapes in the document.
        DisplayCase{
            "ShowsUnicodeValue",
            UNICODE_FILE,
            R"(DISPLAY {"日本"})",
            "\"東京\""
        },
        // Every target has to resolve, so one missing leaf empties the result.
        DisplayCase{
            "ReturnsEmptyWhenOneLeafIsMissing",
            EMPLOYEE_FILE,
            R"(DISPLAY {"employees" {"0" {"name" "age"}}})",
            ""
        },
        // A key that is not in the document at all.
        DisplayCase{
            "ReturnsEmptyForMissingKey",
            EMPLOYEE_FILE,
            R"(DISPLAY {"budget"})",
            ""
        },
        // Index 0 of an empty array names no value to show.
        DisplayCase{
            "ReturnsEmptyForIndexIntoEmptyArray",
            EMPTY_STRUCTURES_FILE,
            R"(DISPLAY {"empty_array" {"0"}})",
            ""
        }
    ),
    caseName<DisplayCase>
);

// ====================
// FILTER queries
// ====================

const std::string removeWhitespaces(const std::string& str) {
    std::string result = "";

    for (char elem : str) {
        if(elem != ' ' && elem != '\n'){
            result += elem;
        }
    }

    return result;
}

struct FilterCase {
    const std::string name;
    const std::string filename;
    const std::string query;
    const std::string expected;
};

class JsonFilterTest : public ::testing::TestWithParam<FilterCase> {};

TEST_P(JsonFilterTest, ReturnsFiltered) {
    const auto& test = GetParam();
    std::ifstream file(test.filename);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    queryparser::parsequery(test.query);

    const std::string result =
        removeWhitespaces(json_parser::parsejson(jsonText, queryparser::getparsedquery()));

    EXPECT_EQ(result, test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    FilterQueries,
    JsonFilterTest,
    ::testing::Values(
        FilterCase{
            "ShowsAllEmployeeData",
            std::string(EMPLOYEE_FILE),
            R"(FILTER { employees { name }} .*)",
            removeWhitespaces(R"([{
                "id": 101,
                "name": "Laura",
                "department": "Product",
                "salary": 90000,
                "active": true,
                "skills": ["C++", "Git", "JSON"]
            }, 
            {
                "id": 102,
                "name": "Moustafa",
                "department": "IT Support",
                "salary": 80000,
                "active": true,
                "skills": ["Networking", "Linux", "Troubleshooting"]
            }, 
            {
                "id": 103,
                "name": "James",
                "department": "Engineering",
                "salary": 95000,
                "active": false,
                "skills": ["C++", "Testing", "CMake"]
            }])")
        },

        FilterCase{
            "ShowsIndividualEmployeeData",
            std::string(EMPLOYEE_FILE),
            R"(FILTER { employees { name }} ^L)",
            removeWhitespaces(R"({
                "id": 101,
                "name": "Laura",
                "department": "Product",
                "salary": 90000,
                "active": true,
                "skills": ["C++", "Git", "JSON"]
            })")
        },

        FilterCase {
            "ShowEmployeeSalaries",
            std::string(EMPLOYEE_FILE),
            R"(FILTER { employees { salary }} 90000 95000)",
            removeWhitespaces(R"([{
                "id": 101,
                "name": "Laura",
                "department": "Product",
                "salary": 90000,
                "active": true,
                "skills": ["C++", "Git", "JSON"]
            },
            {
                "id": 103,
                "name": "James",
                "department": "Engineering",
                "salary": 95000,
                "active": false,
                "skills": ["C++", "Testing", "CMake"]
            }])")
        },

        FilterCase {
            "ShowNoEmployees",
            std::string(EMPLOYEE_FILE),
            R"(FILTER { employees { name }} ^F)",
            "[]" 
        },

        FilterCase {
            "ShowAllProducts",
            std::string(PRODUCT_FILE),
            R"(FILTER { products { name }} .*)",
            removeWhitespaces(R"([{
                "id": 201,
                "name": "Mechanical Keyboard",
                "category": "Electronics",
                "price": 89.99,
                "in_stock": true,
                "tags": ["keyboard", "office", "usb"]
            },
            {
                "id": 202,
                "name": "USB-C Hub",
                "category": "Electronics",
                "price": 34.5,
                "in_stock": false,
                "tags": ["usb-c", "adapter", "laptop"]
            },
            {
                "id": 203,
                "name": "Notebook",
                "category": "Office Supplies",
                "price": 6.25,
                "in_stock": true,
                "tags": ["paper", "school", "notes"]
            }])")
        },

        FilterCase {
            "ShowProductPrices",
            std::string(PRODUCT_FILE),
            R"(FILTER { products { price }} 30.00 90.00)",
            removeWhitespaces(R"([{
                "id": 201,
                "name": "Mechanical Keyboard",
                "category": "Electronics",
                "price": 89.99,
                "in_stock": true,
                "tags": ["keyboard", "office", "usb"]
            },
            {
                "id": 202,
                "name": "USB-C Hub",
                "category": "Electronics",
                "price": 34.5,
                "in_stock": false,
                "tags": ["usb-c", "adapter", "laptop"]
            }])")
        },

        FilterCase {
            "FilterSpecialProductRegex",
            std::string(PRODUCT_FILE),
            R"(FILTER { products { tags }} adapter)",
            removeWhitespaces(R"({
                "id": 202,
                "name": "USB-C Hub",
                "category": "Electronics",
                "price": 34.5,
                "in_stock": false,
                "tags": ["usb-c", "adapter", "laptop"]
            })")
        },

        FilterCase {
            "FilterUniversityDepartmentCS",
            std::string(UNIVERSITY_FILE),
            R"(FILTER { university { departments { courses }}} CS)",
            removeWhitespaces(R"({
                "name": "Computer Science",
                "courses": ["CS 100", "CS 179K", "CS 180"]
            })")
        }
    ),
    caseName<FilterCase>
);

// ====================
// Unicode and escaped-key fixtures
// ====================

// Looks up a key containing an escaped quotation mark.
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
// result.
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

// All cases below exercise the same unicode.json fixture. 
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

// "" is a legal object key, so a path naming it should reach the value it holds
// rather than being treated as a missing component.
TEST(JsonParser, ReturnsValueForEmptyKey) {
    std::ifstream file(EMPTY_STRUCTURES_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    EXPECT_EQ(json_parser::parsejson(jsonText, {""}), "\"Empty key\"");
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

// ====================
// JSONL documents
// ====================

// repeatSearch() applies the existing query behavior to each JSONL record.
// A numeric first argument selects one zero-based record instead.
struct JsonlCase {
    const char* name;
    const char* query;
    std::vector<std::string> expected;
};

class JsonlQueryTest : public ::testing::TestWithParam<JsonlCase> {};

TEST_P(JsonlQueryTest, ReturnsPerRecordResults) {
    const auto& test = GetParam();

    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    queryparser::parsequery(test.query);

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_EQ(result, test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    JsonlQueries,
    JsonlQueryTest,
    ::testing::Values(

        // Reads the first employee name from every JSONL record.
        JsonlCase{
            "ShowsNestedValueFromEveryRecord",
            R"(DISPLAY "employees" {"0" {"name"}})",
            {"\"Laura\"", "\"James\"", "\"Liam\""}
        },

        // Multiple leaves are grouped separately for each record.
        JsonlCase{
            "ShowsSiblingValuesPerRecord",
            R"(DISPLAY "employees" {"0" {"name" "salary"}})",
            {
                R"(["Laura", 90000])",
                R"(["James", 95000])",
                R"(["Liam", 92000])"
            }
        },

        // FIND succeeds as soon as one JSONL record contains the target.
        JsonlCase{
            "FindReportsOneAnswerForFile",
            R"(FIND "employees")",
            {"true"}
        },

        // No JSONL record contains this root key.
        JsonlCase{
            "FindReportsFalseWhenNoRecordMatches",
            R"(FIND "nosuchkey")",
            {"false"}
        },

        // A number alone returns that entire JSONL record.
        JsonlCase{
            "IndexAloneReturnsWholeRecord",
            "DISPLAY 0",
            {
                R"({"employees":[{"name":"Laura","salary":90000,"department":"Engineering"},{"name":"Moustafa","salary":80000,"department":"Sales"}]})"
            }
        },

        // JSONL indexing is zero based, so index 2 is the third record.
        JsonlCase{
            "IndexCountsFromZero",
            "DISPLAY 2",
            {
                R"({"employees":[{"name":"Liam","salary":92000,"department":"Engineering"},{"name":"Nina","salary":88000,"department":"HR"}]})"
            }
        },

        // After choosing a record, normal nested JSON querying still applies.
        JsonlCase{
            "IndexNarrowsToOneRecord",
            R"(DISPLAY 1 "employees" {"0" {"name"}})",
            {"\"James\""}
        },

        // Indexed records can still return multiple nested sibling values.
        JsonlCase{
            "IndexCombinesWithNesting",
            R"(DISPLAY 2 "employees" {"0" {"name" "department"}})",
            {R"(["Liam", "Engineering"])"}
        },

        // FIND can also be limited to one selected JSONL record.
        JsonlCase{
            "IndexedFindReportsForSelectedRecord",
            R"(FIND 1 "employees" {"0" {"department"}})",
            {"true"}
        },

        // An index larger than the file's record count returns nothing.
        JsonlCase{
            "ReturnsEmptyForIndexPastLastRecord",
            "DISPLAY 99",
            {}
        },

        // The selected record exists, but the requested key does not.
        JsonlCase{
            "ReturnsEmptyForMissingKeyInIndexedRecord",
            R"(DISPLAY 1 "nosuchkey")",
            {}
        }
    ),
    caseName<JsonlCase>
);


// ====================
// JSONL FILTER queries
// ====================

// FILTER should collect matching objects from multiple JSONL records.
TEST(JsonParser, JsonlFilterFindsNumericMatchesAcrossRecords) {
    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(FILTER { employees { salary }} 90000 95000)"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 3u);

    EXPECT_EQ(
        result[0],
        R"({"name":"Laura","salary":90000,"department":"Engineering"})"
    );

    EXPECT_EQ(
        result[1],
        R"({"name":"James","salary":95000,"department":"Engineering"})"
    );

    EXPECT_EQ(
        result[2],
        R"({"name":"Liam","salary":92000,"department":"Engineering"})"
    );
}


// Regex FILTER should also search across separate JSONL records.
TEST(JsonParser, JsonlFilterFindsRegexMatchesAcrossRecords) {
    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(FILTER { employees { name }} ^L)"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 2u);

    EXPECT_EQ(
        result[0],
        R"({"name":"Laura","salary":90000,"department":"Engineering"})"
    );

    EXPECT_EQ(
        result[1],
        R"({"name":"Liam","salary":92000,"department":"Engineering"})"
    );
}


// A record with no FILTER matches should not contribute "[]" to JSONL output.
TEST(JsonParser, JsonlFilterSkipsEmptyPerRecordResults) {
    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(FILTER { employees { name }} ^L)"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    for (const std::string& value : result) {
        EXPECT_NE(value, "[]");
    }
}


// If no record contains a FILTER match, the JSONL result should be empty.
TEST(JsonParser, JsonlFilterReturnsEmptyWhenNothingMatches) {
    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(FILTER { employees { name }} ^Z)"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_TRUE(result.empty());
}


// ====================
// Larger JSONL fixture
// ====================

// academia.jsonl contains 300 records, so this verifies that a full sweep
// reaches every record rather than stopping after the first few.
TEST(JsonParser, JsonlAcademiaVisitsAllRecords) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(R"(DISPLAY "id")");

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 300u);

    EXPECT_EQ(result.front(), "\"paper_0001\"");
    EXPECT_EQ(result.back(), "\"paper_0300\"");
}


// Checks an exact top-level value from the first academia record.
TEST(JsonParser, JsonlAcademiaReadsIndexedTopLevelValue) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 0 "title")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"A Study of Topology\"");
}


// Checks nested object traversal after selecting a JSONL record.
TEST(JsonParser, JsonlAcademiaReadsNestedMetadata) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 0 "metadata" {"language"})"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"en\"");
}


// Checks object -> array -> object traversal inside a JSONL record.
TEST(JsonParser, JsonlAcademiaReadsNestedArrayValue) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 0 "authors" {"0" {"name"}})"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"Morgan Nguyen\"");
}


// JSON booleans should be preserved when read through the JSONL path.
TEST(JsonParser, JsonlAcademiaPreservesBooleanValue) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 0 "open_access")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "false");
}


// Numbers should also preserve their normal JSON representation.
TEST(JsonParser, JsonlAcademiaPreservesNumericValue) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 0 "year")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "2020");
}


// Reads the final record to catch zero-based indexing and end-of-file bugs.
TEST(JsonParser, JsonlAcademiaCanReadLastRecord) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 299 "id")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"paper_0300\"");
}


// Index 300 is one past the last valid academia record.
TEST(JsonParser, JsonlAcademiaRejectsIndexPastEnd) {
    std::ifstream file(JSONL_ACADEMIA_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 300 "id")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_TRUE(result.empty());
}


// ====================
// JSONL line edge cases
// ====================

// The final JSONL record does not need a trailing newline.
TEST(JsonParser, JsonlReadsFinalRecordWithoutTrailingNewline) {
    const std::string jsonText =
        R"({"name":"Alice"})" "\n"
        R"({"name":"Bob"})";

    queryparser::parsequery(
        R"(DISPLAY "name")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_EQ(
        result,
        (std::vector<std::string>{
            "\"Alice\"",
            "\"Bob\""
        })
    );
}


// Windows JSONL files use "\r\n" instead of only "\n".
TEST(JsonParser, JsonlSupportsWindowsLineEndings) {
    const std::string jsonText =
        "{\"name\":\"Alice\"}\r\n"
        "{\"name\":\"Bob\"}\r\n"
        "{\"name\":\"Charlie\"}\r\n";

    queryparser::parsequery(
        R"(DISPLAY "name")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_EQ(
        result,
        (std::vector<std::string>{
            "\"Alice\"",
            "\"Bob\"",
            "\"Charlie\""
        })
    );
}


// Empty lines should be ignored instead of becoming JSONL records.
TEST(JsonParser, JsonlSkipsEmptyLines) {
    const std::string jsonText =
        R"({"name":"Alice"})" "\n"
        "\n"
        R"({"name":"Bob"})" "\n"
        "\n"
        R"({"name":"Charlie"})";

    queryparser::parsequery(
        R"(DISPLAY "name")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_EQ(
        result,
        (std::vector<std::string>{
            "\"Alice\"",
            "\"Bob\"",
            "\"Charlie\""
        })
    );
}


// Empty lines must not change zero-based JSONL record indexes.
TEST(JsonParser, JsonlIndexIgnoresEmptyLines) {
    const std::string jsonText =
        R"({"name":"Alice"})" "\n"
        "\n"
        R"({"name":"Bob"})" "\n"
        "\n"
        R"({"name":"Charlie"})";

    queryparser::parsequery(
        R"(DISPLAY 1 "name")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"Bob\"");
}


// An entirely empty JSONL document contains no records.
TEST(JsonParser, JsonlEmptyDocumentReturnsNoDisplayResults) {
    const std::string jsonText = "";

    queryparser::parsequery(
        R"(DISPLAY "name")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_TRUE(result.empty());
}


// FIND on an empty JSONL document should report that no record matched.
TEST(JsonParser, JsonlFindOnEmptyDocumentReturnsFalse) {
    const std::string jsonText = "";

    queryparser::parsequery(
        R"(FIND "name")"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    EXPECT_EQ(
        result,
        (std::vector<std::string>{"false"})
    );
}


// Ensures a selected record cannot accidentally return data from its neighbor.
TEST(JsonParser, JsonlIndexDoesNotLeakAcrossRecords) {
    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);

    queryparser::parsequery(
        R"(DISPLAY 0 "employees" {"0" {"name"}})"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"Laura\"");
}

// Lines containing only spaces or tabs should not count as JSONL records.
TEST(JsonParser, JsonlIgnoresWhitespaceOnlyLines) {
    std::ifstream file(JSONL_TESTING_FILE);
    ASSERT_TRUE(file.is_open());

    const std::string original =
        json_parser::jsonToString(file);

    // Insert whitespace-only lines between the real records.
    std::string jsonText;
    size_t start = 0;

    while (start < original.size()) {
        size_t end = original.find('\n', start);

        if (end == std::string::npos) {
            jsonText += original.substr(start);
            break;
        }

        jsonText += original.substr(start, end - start + 1);

        // Add lines that contain whitespace but no JSON.
        jsonText += "      \n";
        jsonText += "\t\t\n";

        start = end + 1;
    }

    queryparser::parsequery(
        R"(DISPLAY 1 "employees" {"0" {"name"}})"
    );

    const std::vector<std::string> result =
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

    // Whitespace-only lines must not shift record index 1.
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], "\"James\"");
}

TEST(JsonParser, JsonlRejectsMalformedRecordWithLineNumber) {
    const std::string jsonText =
        R"({"name":"Alice"})" "\n"
        R"({"name":"Bob")" "\n"
        R"({"name":"Charlie"})";

    queryparser::parsequery(
        R"(DISPLAY "name")"
    );

    try {
        json_parser::repeatSearch(
            jsonText,
            queryparser::getparsedquery()
        );

        FAIL() << "Expected malformed JSONL to throw";
    }
    catch (const std::runtime_error& e) {
        EXPECT_EQ(
            std::string(e.what()),
            "Invalid JSONL record on line 2"
        );
    }
}

}

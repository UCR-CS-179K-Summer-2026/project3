#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../src/jsonparser.h"
#include "../src/queryparser.h"

// This test gives the parser a basic FIND query with one field name. -- Moustafa Test ParseSimpleFindQuery1
// It checks that parsequery() separates the command and field into two tokens.
TEST(QueryParser, ParsesSimpleFindQuery) {
    const std::string query = "FIND employees";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "employees"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test gives the parser a query with two field names after FIND. -- Moustafa Test ParseSimpleFindQuery2
// It checks that the tokens remain in the same order as the original query.
TEST(QueryParser, ParsesMultiFieldQuery) {
    const std::string query = "FIND employees name";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "employees",
        "name"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test includes 0 as part of the query path.
// The parser does not convert it to an integer, so the expected value is the string "0".
TEST(QueryParser, ParsesArrayIndexToken) {
    const std::string query = "FIND employees 0 name";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "employees",
        "0",
        "name"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test uses braces around part of the query.  -- Moustafa Test ParseNestedFindQuery1
// It checks that the values inside the braces are stored in a nested JSONTypes vector.
TEST(QueryParser, ParsesNestedQuery) {
    const std::string query = "FIND {employees birthday month} day";

    const std::vector<JSONTypes> expected = {
        "FIND",
        {"employees", "birthday", "month"},
        "day"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test places a nested group at the end of the query.  -- Moustafa Test ParseNestedFindQuery2
// It checks that the parser stores the final braced value as a nested vector.
TEST(QueryParser, ParsesNestedQueryAtEnd) {
    const std::string query = "FIND day {employees}";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "day",
        {"employees"}
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test contains several levels of braces. -- Moustafa Test ParseMultiNestedFindQuery1
// It checks that parsequery() can build nested JSONTypes vectors recursively.
TEST(QueryParser, ParsesDeeplyNestedQuery) {
    const std::string query = "FIND a b {c d {e f {g h}}}";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "a",
        "b",
        {
            "c",
            "d",
            {
                "e",
                "f",
                {"g", "h"}
            }
        }
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test contains several separate nested groups inside one outer group. -- Moustafa Test ParseMultiNestedFindQuery2
// It checks that each braced section is stored as its own nested vector.
TEST(QueryParser, ParsesMultipleNestedGroups) {
    const std::string query = "FIND ab cd {{ef gh} i {jk lm} {no pq}}";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "ab",
        "cd",
        {
            {"ef", "gh"},
            "i",
            {"jk", "lm"},
            {"no", "pq"}
        }
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test passes an empty string to parsequery().
// It checks that the parser returns an empty result instead of creating a token.
TEST(QueryParser, HandlesEmptyQuery) {
    queryparser::parsequery("");

    EXPECT_TRUE(queryparser::getparsedquery().empty());
}

// This test passes a query containing only spaces.
// It checks that spaces by themselves do not become query tokens.
TEST(QueryParser, HandlesWhitespaceOnlyQuery) {
    queryparser::parsequery("     ");

    EXPECT_TRUE(queryparser::getparsedquery().empty());
}

// This test adds several spaces between each query word.
// It checks that repeated spaces do not create empty values in the parsed result.
TEST(QueryParser, IgnoresRepeatedSpaces) {
    const std::string query = "FIND   employees    name";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "employees",
        "name"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test adds spaces before and after the query.
// It checks that those outside spaces are not included in the parsed tokens.
TEST(QueryParser, IgnoresLeadingAndTrailingSpaces) {
    const std::string query = "   FIND employees name   ";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "employees",
        "name"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test parses one query and then immediately parses a different query.
// It checks that the second call replaces the old parsed result instead of appending to it.
TEST(QueryParser, ReplacesPreviousParsedQuery) {
    queryparser::parsequery("FIND employees name");
    queryparser::parsequery("FIND products price");

    const std::vector<JSONTypes> expected = {
        "FIND",
        "products",
        "price"
    };

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test includes quotation marks around field names.
// It checks the parser's current behavior, which keeps the quotation marks inside each token.
TEST(QueryParser, PreservesQuotationMarks) {
    const std::string query = R"(FIND "employees" "name")";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "\"employees\"",
        "\"name\""
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

// This test includes a closing brace without a matching opening brace.
// It checks the parser's current behavior for that malformed query.
TEST(QueryParser, HandlesUnmatchedClosingBrace) {
    const std::string query = "FIND employees}";

    const std::vector<JSONTypes> expected = {
        "FIND",
        "employees"
    };

    queryparser::parsequery(query);

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

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

//
TEST(JsonParser, TestEscapeChar) {
    std::ifstream file(JSON_DATA_DIR "/test.json");
    ASSERT_TRUE(file.is_open());

    const std::string jsonText = json_parser::jsonToString(file);
    const std::vector<std::string> path = {
        "\"a"
    };

    const std::string_view result =
        json_parser::parsejson(jsonText, path);

    EXPECT_EQ(result, "1");
}
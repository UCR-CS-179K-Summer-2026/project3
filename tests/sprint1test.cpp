#include <fstream>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "../src/jsonparser.h"
#include "../src/queryparser.h"

// This test gives the parser a basic FIND query with one field name. -- James Test ParseSimpleFindQuery1
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

// This test gives the parser a query with two field names after FIND. -- James Test ParseSimpleFindQuery2
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

// This test uses braces around part of the query.  -- James Test ParseNestedFindQuery1
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

// This test places a nested group at the end of the query.  -- James Test ParseNestedFindQuery2
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

// This test contains several levels of braces. -- James Test ParseMultiNestedFindQuery1
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

// This test contains several separate nested groups inside one outer group. -- James Test ParseMultiNestedFindQuery2
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
    std::ifstream json("../src/jsonFiles/employee.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

// This test opens product.json from the jsonFiles folder.
// It checks that the stream is open and that json_parser::test() returns 0 for success.
TEST(JsonParser, OpensProductFile) {
    std::ifstream json("../src/jsonFiles/product.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

// This test opens university.json from the jsonFiles folder.
// It checks that the stream is open and that json_parser::test() returns 0 for success.
TEST(JsonParser, OpensUniversityFile) {
    std::ifstream json("../src/jsonFiles/university.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
}

// This test tries to open a file that does not exist.
// It checks that the stream remains closed and that json_parser::test() returns 1.
TEST(JsonParser, RejectsMissingFile) {
    std::ifstream json("../src/jsonFiles/missing.json");

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
    std::ifstream json("../src/jsonFiles/employee.json");

    ASSERT_TRUE(json.is_open());
    EXPECT_EQ(json_parser::test(json), 0);
    EXPECT_EQ(json_parser::test(json), 0);
}
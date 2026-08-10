#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "queryparser.h"

namespace {

// Groups valid query inputs that all follow the same test pattern:
// parse the query, then compare the parsed structure with the expected JSONType vector.
struct QueryParseCase {
    std::string name;
    std::string query;
    std::vector<JSONType> expected;
};

class QueryParserValidCaseTest
    : public ::testing::TestWithParam<QueryParseCase> {};

// Runs the same parser check for each valid query case below.
// Parameterizing these cases keeps the test logic consistent while making
// each input, expected structure, and behavior easy to compare.
TEST_P(QueryParserValidCaseTest, ParsesExpectedStructure) {
    const QueryParseCase& test = GetParam();

    queryparser::parsequery(test.query);

    EXPECT_EQ(queryparser::getparsedquery(), test.expected);
}

INSTANTIATE_TEST_SUITE_P(
    ValidQueries,
    QueryParserValidCaseTest,
    ::testing::Values(
        // Basic FIND query with one field.
        // Checks that the command and field are stored as two parsed elements.
        QueryParseCase{
            "ParsesSimpleFindQuery",
            "FIND {employees}",
            std::vector<JSONType>{
                "FIND",
                {"employees"}
            }
        },

        // FIND query with two fields.
        // Checks that field order is preserved.
        QueryParseCase{
            "ParsesMultiFieldQuery",
            "FIND {employees name}",
            std::vector<JSONType>{
                "FIND",
                {"employees", "name"}
            }
        },

        // Array index appears as part of the path.
        // The parser keeps 0 as the string "0" instead of converting it to an integer.
        QueryParseCase{
            "ParsesArrayIndexToken",
            "FIND {employees 0 name}",
            std::vector<JSONType>{
                "FIND",
                {"employees", "0", "name"}
            }
        },

        // Nested braces create a nested JSONType vector.
        QueryParseCase{
            "ParsesNestedQuery",
            "FIND {{employees birthday month} day}",
            std::vector<JSONType>{
                "FIND",
                {
                    {"employees", "birthday", "month"},
                    "day"
                }
            }
        },

        // Nested group appears at the end of the outer group.
        // Checks that the final braced value stays nested.
        QueryParseCase{
            "ParsesNestedQueryAtEnd",
            "FIND {day {employees}}",
            std::vector<JSONType>{
                "FIND",
                {
                    "day",
                    {"employees"}
                }
            }
        },

        // Multiple levels of braces test recursive nesting.
        QueryParseCase{
            "ParsesDeeplyNestedQuery",
            "FIND {a b {c d {e f {g h}}}}",
            std::vector<JSONType>{
                "FIND",
                {
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
                }
            }
        },

        // Several separate nested groups appear inside one outer group.
        // Each braced section should become its own nested vector.
        QueryParseCase{
            "ParsesMultipleNestedGroups",
            "FIND {ab cd {{ef gh} i {jk lm} {no pq}}}",
            std::vector<JSONType>{
                "FIND",
                {
                    "ab",
                    "cd",
                    {
                        {"ef", "gh"},
                        "i",
                        {"jk", "lm"},
                        {"no", "pq"}
                    }
                }
            }
        },

        // Repeated spaces should not create empty tokens.
        QueryParseCase{
            "IgnoresRepeatedSpaces",
            "FIND   {employees    name}",
            std::vector<JSONType>{
                "FIND",
                {"employees", "name"}
            }
        },

        // Spaces before and after the query should not become part of the tokens.
        QueryParseCase{
            "IgnoresLeadingAndTrailingSpaces",
            "   FIND {employees name}   ",
            std::vector<JSONType>{
                "FIND",
                {"employees", "name"}
            }
        },

        // Quotation marks around field names are preserved by the current parser behavior.
        QueryParseCase{
            "PreservesQuotationMarks",
            R"(FIND {"employees" "name"})",
            std::vector<JSONType>{
                "FIND",
                {"\"employees\"", "\"name\""}
            }
        }
    ),
    // Uses the descriptive case name above in GoogleTest output.
    [](const ::testing::TestParamInfo<QueryParseCase>& info) {
        return info.param.name;
    }
);


// Groups inputs that should produce no parsed tokens.
struct EmptyQueryCase {
    std::string name;
    std::string query;
};

class QueryParserEmptyCaseTest
    : public ::testing::TestWithParam<EmptyQueryCase> {};

// Empty and whitespace-only inputs should both leave the parsed result empty.
TEST_P(QueryParserEmptyCaseTest, ProducesEmptyResult) {
    const EmptyQueryCase& test = GetParam();

    queryparser::parsequery(test.query);

    EXPECT_TRUE(queryparser::getparsedquery().empty());
}

INSTANTIATE_TEST_SUITE_P(
    EmptyQueries,
    QueryParserEmptyCaseTest,
    ::testing::Values(
        // Empty input should not create any token.
        EmptyQueryCase{"HandlesEmptyQuery", ""},

        // Spaces alone should also produce no tokens.
        EmptyQueryCase{"HandlesWhitespaceOnlyQuery", "     "}
    ),
    [](const ::testing::TestParamInfo<EmptyQueryCase>& info) {
        return info.param.name;
    }
);


// Groups malformed queries that should be rejected with std::runtime_error.
struct InvalidQueryCase {
    std::string name;
    std::string query;
};

class QueryParserInvalidCaseTest
    : public ::testing::TestWithParam<InvalidQueryCase> {};

// Each malformed query below has an unmatched closing brace.
// The parser should reject the input instead of returning a partial structure.
TEST_P(QueryParserInvalidCaseTest, ThrowsRuntimeError) {
    const InvalidQueryCase& test = GetParam();

    EXPECT_THROW(queryparser::parsequery(test.query), std::runtime_error);
}

INSTANTIATE_TEST_SUITE_P(
    InvalidQueries,
    QueryParserInvalidCaseTest,
    ::testing::Values(
        // Closing brace appears without a matching opening brace after a token.
        InvalidQueryCase{
            "HandlesUnmatchedClosingBrace",
            "FIND employees }"
        },

        // Closing brace appears immediately after FIND.
        InvalidQueryCase{
            "ParseInvalidQuery1",
            "FIND }"
        }
    ),
    [](const ::testing::TestParamInfo<InvalidQueryCase>& info) {
        return info.param.name;
    }
);

}  // namespace


// This behavior is kept as a standalone test because it depends on two
// consecutive parsequery() calls rather than one independent input case.
// It checks that the second parse replaces the previous result instead of appending to it.
TEST(QueryParser, ReplacesPreviousParsedQuery) {
    queryparser::parsequery("FIND {employees name}");
    queryparser::parsequery("FIND {products price}");

    const std::vector<JSONType> expected = {
        "FIND",
        {"products", "price"}
    };

    EXPECT_EQ(queryparser::getparsedquery(), expected);
}

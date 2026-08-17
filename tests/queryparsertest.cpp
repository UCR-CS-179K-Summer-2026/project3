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
TEST_P(QueryParserValidCaseTest, ParsesExpectedStructure) {
    const QueryParseCase& test = GetParam();

    queryparser::parsequery(test.query);

    EXPECT_EQ(queryparser::getparsedquery().parts, test.expected);
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

        // Quotation marks are dropped so the token is ready to compare with a JSON key.
        QueryParseCase{
            "StripsQuotationMarks",
            R"(FIND {"employees" "name"})",
            std::vector<JSONType>{
                "FIND",
                {"employees", "name"}
            }
        },

        // A space inside quotes belongs to the key, so it must not split the token.
        QueryParseCase{
            "KeepsSpacesInsideQuotedKey",
            R"(FIND {"first name"})",
            std::vector<JSONType>{
                "FIND",
                {"first name"}
            }
        },

        // "" is a legal JSON key, so it must survive as an empty token.
        QueryParseCase{
            "KeepsEmptyQuotedKey",
            R"(FIND {""})",
            std::vector<JSONType>{
                "FIND",
                {""}
            }
        },

        // A backslash escape is kept as written, matching how the document spells the key.
        QueryParseCase{
            "KeepsEscapeInsideQuotedKey",
            R"(FIND {"\"a"})",
            std::vector<JSONType>{
                "FIND",
                {"\\\"a"}
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

    EXPECT_TRUE(queryparser::getparsedquery().parts.empty());
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
        },

        // A command the reader has no case for is rejected at parse time.
        InvalidQueryCase{
            "RejectsUnknownCommand",
            "FETCH {employees}"
        },

        // A quoted token with no closing quote is incomplete.
        InvalidQueryCase{
            "RejectsUnterminatedQuote",
            R"(FIND {"employees})"
        }
    ),
    [](const ::testing::TestParamInfo<InvalidQueryCase>& info) {
        return info.param.name;
    }
);

}  // namespace


// It checks that the second parse replaces the previous result instead of appending to it.
TEST(QueryParser, ReplacesPreviousParsedQuery) {
    queryparser::parsequery("FIND {employees name}");
    queryparser::parsequery("FIND {products price}");

    const std::vector<JSONType> expected = {
        "FIND",
        {"products", "price"}
    };

    EXPECT_EQ(queryparser::getparsedquery().parts, expected);
}

// The command is recorded so the JSON parser does not have to infer it.
TEST(QueryParser, RecordsCommand) {
    queryparser::parsequery("FIND {employees}");
    EXPECT_EQ(queryparser::getparsedquery().command, Query::FIND);

    queryparser::parsequery(R"(FILTER {"employees" {"name"}} "^L")");
    EXPECT_EQ(queryparser::getparsedquery().command, Query::FILTER);

    queryparser::parsequery("DISPLAY {employees}");
    EXPECT_EQ(queryparser::getparsedquery().command, Query::DISPLAY);

    queryparser::parsequery("ALLOF {employees name}");
    EXPECT_EQ(queryparser::getparsedquery().command, Query::ALLOF);
}

// One FILTER argument after the path means regex.
TEST(QueryParser, IdentifiesRegexFilter) {
    queryparser::parsequery(
        R"(FILTER {"employees" {"name"}} "^L")"
    );

    EXPECT_TRUE(queryparser::getparsedquery().isRegexFilter);
}

// Two FILTER arguments after the path mean numeric range.
TEST(QueryParser, IdentifiesNumericFilter) {
    queryparser::parsequery(
        R"(FILTER {"employees" {"salary"}} 80000 100000)"
    );

    EXPECT_FALSE(queryparser::getparsedquery().isRegexFilter);
}

// Make sure filter metadata does not carry into the next query.
TEST(QueryParser, ResetsFilterType) {
    queryparser::parsequery(
        R"(FILTER {"employees" {"name"}} "^L")"
    );

    EXPECT_TRUE(queryparser::getparsedquery().isRegexFilter);

    queryparser::parsequery("FIND {employees}");

    EXPECT_FALSE(queryparser::getparsedquery().isRegexFilter);
}


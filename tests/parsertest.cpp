#include <iostream>
#include <gtest/gtest.h>

#include "../src/queryparser.h"

// Test valid queries
TEST(QueryParser, ParseSimpleFindQuery1) {
    std::string query1 = "FIND employees";
    const std::vector<JSONTypes> expected1 = {"FIND", "employees"};
    queryparser::parsequery(query1);
    EXPECT_EQ(queryparser::getparsedquery(), expected1);
}


TEST(QueryParser, ParseSimpleFindQuery2){
    std::string query2 = "FIND employees name";
    const std::vector<JSONTypes> expected2 = {"FIND", "employees", "name"};
    queryparser::parsequery(query2);
    EXPECT_EQ(queryparser::getparsedquery(), expected2);
}

TEST(QueryParser, ParseNestedFindQuery1){
    std::string query3 = "FIND {employees birthday month} day";
    const std::vector<JSONTypes> expected3 = {"FIND", {"employees", "birthday", "month"}, "day"};
    queryparser::parsequery(query3);
    EXPECT_EQ(queryparser::getparsedquery(), expected3);
}

TEST(QueryParser, ParseNestedFindQuery2){
    std::string query4 = "FIND day {employees}";
    const std::vector<JSONTypes> expected4 = {"FIND", "day", {"employees"}};
    queryparser::parsequery(query4);
    EXPECT_EQ(queryparser::getparsedquery(), expected4);
}

TEST(QueryParser, ParseMultiNestedFindQuery1){
    std::string query5 = "FIND a b {c d {e f {g h}}}";
    const std::vector<JSONTypes> expected5 = {"FIND", "a", "b", {"c", "d", {"e, f", {"g", "h"}}}};
    queryparser::parsequery(query5);
    EXPECT_EQ(queryparser::getparsedquery(), expected5);
}

TEST(QueryParser, ParseMultiNestedFindQuery2){
    std::string query6 = "FIND ab cd {{ef gh} i {jk lm} {no pq}}";
    const std::vector<JSONTypes> expected6 = {"FIND", "ab", "cd", {{"ef", "gh"}, "i", {"jk", "lm"}, {"no", "pq"}}};
    queryparser::parsequery(query6);
    EXPECT_EQ(queryparser::getparsedquery(), expected6);
}

int main() {
    std::string query5 = "FIND a b {c d {e f {g h}}}";
    const std::vector<JSONTypes> expected5 = {"FIND", "a", "b", {"c", "d", {"e, f", {"g", "h"}}}};
    queryparser::parsequery(query5);
    queryparser::displaylastparsedquery();
    std::vector<JSONTypes> parsed = queryparser::getparsedquery();
    std::cout << (queryparser::getparsedquery() == expected5) << std::endl;
    return 0;
}

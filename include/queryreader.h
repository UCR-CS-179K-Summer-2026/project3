#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <string>

#include "queryparser.h"

enum class Query {
    FIND,
    FILTER,
    DISPLAY,
    ALLOF
};

class QueryConstruct {
    Query query;
    std::vector<JSONType> keys;
    std::string regex;
    std::vector<JSONType> params;

    public:
        // FIND, DISPLAY, ALLOF
        QueryConstruct(const Query& q, const std::vector<JSONType>& k) : query(q), keys(k) {}

        // FILTER
        QueryConstruct(const Query& q, const std::vector<JSONType>& k, const std::string& r)
        : query(q), keys(k), regex(r) {}

        // FILTER
        QueryConstruct(const Query& q, const std::vector<JSONType>& k, const std::vector<JSONType>& p)
        : query(q), keys(k), params(p) {}
};

namespace queryreader {
    const QueryConstruct readquery(const std::vector<JSONType>& parsedquery);
    void applyquery(const QueryConstruct& query, const std::string& json);
}
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

struct QueryConstruct {
    Query query;
    std::vector<JSONType> keys;
    std::string param;
    std::vector<JSONType> params;

    bool isregexquery = false;
    bool isparamsquery = true;
    
    // FIND, DISPLAY, ALLOF
    QueryConstruct(const Query& q, const std::vector<JSONType>& k) : query(q), keys(k) {}

    // FILTER
    QueryConstruct(const Query& q, const std::vector<JSONType>& k,
               const std::string& p, const bool isregex)
    : query(q),
      keys(k),
      param(p),
      isregexquery(isregex),
      isparamsquery(!isregex) {}

    // FILTER
    QueryConstruct(const Query& q, const std::vector<JSONType>& k, const std::vector<JSONType>& p)
    : query(q), keys(k), params(p) {}

    bool operator==(const QueryConstruct& other){
        return 
            (   
                query == other.query &&
                keys == other.keys &&
                (param == other.param || param.empty() && other.param.empty()) &&
                (params == other.params || params.empty() && other.params.empty()) &&
                isregexquery == other.isregexquery &&
                isparamsquery == other.isparamsquery
            );
    }
};

namespace queryreader {
    const QueryConstruct readquery(const std::vector<JSONType>& parsedquery);
    void applyquery(const QueryConstruct& query, const std::string& json);
}
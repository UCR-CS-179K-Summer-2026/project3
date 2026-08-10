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
    protected:
        std::vector<JSONType> jsonparams;
    public:
        virtual void applyquery() = 0; 
};

class QueryFind : QueryConstruct {
    public:
        QueryFind(const std::vector<JSONType>& json) : jsonparams(json) {}
};

class QueryFilter : QueryConstruct {
    private:
        std::string regex;
        int lowerbound;
        int upperbound;
    
    public:
        QueryFilter(const std::vector<JSONType>& json, const std::string& r)
        : jsonparams(json), regex(r) {}

        QueryFilter(const std::vector<JSONType>& json, int lower, int upper)
        : jsonparams(json), lowerbound(lower), upperbound(upper) {}
};

class QueryDisplay : QueryConstruct {
    // Laura
};

class QueryAllof : QueryConstruct {
    // Laura
};

namespace queryreader {
    const QueryConstruct readquery(const std::vector<JSONType>& parsedquery);
    void applyquery(const QueryConstruct& query, const std::string& json);
}
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
    public:
        virtual void applyquery() = 0; 
}

class QueryFind : QueryConstruct {

};

class QueryFilter : QueryConstruct {

};

class QueryDisplay : QueryConstruct {
    // Laura
};

class QueryAllof : QueryConstruct {
    // Laura
}

namespace queryreader {
    const QueryConstruct readquery(const std::vector<JSONType>& parsedquery);
    void applyquery(const QueryConstruct& query, const std::string& json);
}
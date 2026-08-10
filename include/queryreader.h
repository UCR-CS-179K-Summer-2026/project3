#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <memory>

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
        virtual ~QueryConstruct() = default;
        virtual void applyquery() = 0; 
<<<<<<< HEAD
        QueryConstruct(const std::vector<JSONType>& json) : jsonparams(json) {}
=======
>>>>>>> 65c13a9 (added semicolons)
};

class QueryFind : public QueryConstruct {
    public:
<<<<<<< HEAD
        QueryFind(const std::vector<JSONType>& json) : QueryConstruct(json) {}
        void applyquery() override;
=======
        QueryFind(const std::vector<JSONType>& json) : jsonparams(json) {};
>>>>>>> 65c13a9 (added semicolons)
};

class QueryFilter : public QueryConstruct {
    private:
        std::string regex;
        int lowerbound;
        int upperbound;
    
    public:
<<<<<<< HEAD
        QueryFilter(const std::vector<JSONType>& json, const std::string& r)
        : QueryConstruct(json), regex(r) {}

        QueryFilter(const std::vector<JSONType>& json, const std::string& lower, 
            const std::string& upper)
        : QueryConstruct(json), lowerbound(std::stoi(lower)), upperbound(std::stoi(upper)) {}

        void applyquery() override;
=======
        QueryFilter();
>>>>>>> 65c13a9 (added semicolons)
};

class QueryDisplay : public QueryConstruct {
    // Laura

    public:
        void applyquery() override;
};

class QueryAllof : QueryConstruct {
    // Laura
<<<<<<< HEAD

    public:
        void applyquery() override;
=======
>>>>>>> 65c13a9 (added semicolons)
};

namespace queryreader {
    std::unique_ptr<QueryConstruct> readquery(const std::vector<JSONType>& parsedquery);
}
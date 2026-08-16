#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <string>
#include <memory>

#include "queryparser.h"

class QueryConstruct {
    protected:
        std::vector<JSONType> jsonparams;
    public:
        virtual ~QueryConstruct() = default;
        virtual void applyquery() = 0; 
        QueryConstruct(const std::vector<JSONType>& json) : jsonparams(json) {}
};

class QueryFind : public QueryConstruct {
    public:
        QueryFind(const std::vector<JSONType>& json) : QueryConstruct(json) {}
        void applyquery() override;
};

class QueryFilter : public QueryConstruct {
    private:
        std::string regex;
        int lowerbound;
        int upperbound;
    
    public:
        QueryFilter(const std::vector<JSONType>& json, const std::string& r)
        : QueryConstruct(json), regex(r) {}

        QueryFilter(const std::vector<JSONType>& json, const std::string& lower, 
            const std::string& upper)
        : QueryConstruct(json), lowerbound(std::stoi(lower)), upperbound(std::stoi(upper)) {}

        void applyquery() override;
};

class QueryDisplay : public QueryConstruct {
    // Laura

    public:
        void applyquery() override;
};

class QueryAllof : QueryConstruct {
    // Laura

    public:
        void applyquery() override;
};

namespace queryreader {
    std::unique_ptr<QueryConstruct> readquery(const std::vector<JSONType>& parsedquery);
}
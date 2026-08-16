#pragma once

#include <string>
#include <variant>
#include <vector>
#include <initializer_list>

// JSONType will allow us to have a recursive structure.
// structure holds the structure of our query.

// {a b {c d { e f }}}
struct JSONType {
    std::variant<
        std::string,
        std::vector<JSONType>
    > structure;

    JSONType(const char* s) : structure(s) {}
    JSONType(const std::string& s) : structure(s) {}
    JSONType(const std::vector<JSONType>& v) : structure(v) {}
    JSONType(const std::initializer_list<JSONType>& v) : structure(std::vector<JSONType>(v)) {}

    // Getters

    bool isstring() const;
    bool isvector() const;
    const std::string& asstring() const;
    const std::vector<JSONType>& asvector() const;

    bool operator==(const JSONType& other) const = default;
};

enum class Query {
    FIND,
    FILTER,
    DISPLAY,
    ALLOF
};

// Stores the parsed query along with information that the JSON parser
// will need later so it does not have to infer the command or filter type.
struct ParsedQuery {
    std::vector<JSONType> parts;
    Query command = Query::FIND;
    bool isRegexFilter = false;

    bool operator==(const ParsedQuery& other) const = default;
};

namespace queryparser {
    void parsequery(const std::string& query);

    // For debugging purposes
    void displaylastparsedquery();

    // Returns last parsed query and metadata
    const ParsedQuery& getparsedquery();
}

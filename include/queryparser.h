#pragma once

#include <string>
#include <variant>
#include <vector>
#include <initializer_list>

// JSONTypes will allow us to have a recursive structure.
// structure holds the structure of our query.

// a b {c d { e f }}
struct JSONTypes {
    std::variant<
        std::string,
        std::vector<JSONTypes>
    > structure;

    JSONTypes(const char* s) : structure(s) {}
    JSONTypes(const std::string& s) : structure(s) {}
    JSONTypes(const std::vector<JSONTypes>& v) : structure(v) {}
    JSONTypes(const std::initializer_list<JSONTypes>& v) : structure(std::vector<JSONTypes>(v)) {};

    bool operator==(const JSONTypes& other) const = default;
};

namespace queryparser {
    void parsequery(const std::string& query);

    // For debugging purposes
    void displaylastparsedquery();

    // Returns last parsed query
    const std::vector<JSONTypes>& getparsedquery(); 
}
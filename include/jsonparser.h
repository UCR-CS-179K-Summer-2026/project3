#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <vector>

#include "queryparser.h"

namespace json_parser {

    int test(std::ifstream& json);

    std::vector<std::string> repeatSearch(std::string_view json, const ParsedQuery& query);

    std::string jsonToString(std::ifstream& json);

    int isFileOpen(std::ifstream& json);

    // Use MMAP to open the file instead of copying it to a string
    std::string_view mapFile(const std::string& path);

    // Returns the JSON text of the value the path names, or an empty string
    // when the path does not exist.
    std::string parsejson(std::string_view json, const std::vector<std::string>& path);

    // Runs a parsed query against the document.
    std::string parsejson(std::string_view json, const ParsedQuery& query);
}
#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <vector>

namespace json_parser {

    std::string jsonToString(std::ifstream& json);

    int isFileOpen(std::ifstream& json);

    // Returns the raw text of the value at `query`, or an empty view if the
    // path is not there. Points into `json`, so `json` must outlive the result.
    // (since its a string_view)
    std::string_view parsejson(std::string_view json, const std::vector<std::string>& query);

}

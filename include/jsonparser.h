#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <vector>

namespace json_parser {

    int test(std::ifstream& json);

    std::string jsonToString(std::ifstream& json);

    int isFileOpen(std::ifstream& json);

    std::string parsejson(std::string_view json, const std::vector<std::string>& query);

}

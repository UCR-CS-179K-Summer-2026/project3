#pragma once

#include <string>
#include <iostream>
#include <fstream>

namespace json_parser {
    
    std::string jsonToString(std::ifstream& json);

    void parsejson(std::string& json);

    int isFileOpen(std::ifstream& json);

}
// C++ STD libraries
#include<iostream>  // console IO
#include<fstream>   // file IO
#include<string>

// Header Files
#include "jsonparser.h"
#include "queryreader.h"

int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    std::string fileName;
    std::cout << "Please input json filename to query.\nFor example, \"EX.json\"" << std::endl;
    std::cin >> fileName;
    fileName = "src/jsonFiles/" + fileName;
    std::ifstream json(fileName);

    // verify file opened correctly
    if (!json.is_open()) {
        std::cerr << "Error: Could not open the file " << fileName << std::endl;
        return 1;
    }

    int x = json_parser::isFileOpen(json);
    std::cout << x << std::endl;

    std::string json_string = json_parser::jsonToString(json);
    std::cout << json_string << std::endl;
    std::cout << "----------------------------------" << std::endl;

    json_parser::parsejson(json_string);


    json.close();
    return 0;
}
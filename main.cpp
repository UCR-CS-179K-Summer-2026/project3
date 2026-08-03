// C++ STD libraries
#include<iostream>  // console IO
#include<fstream>   // file IO
#include<sstream>
#include<string>
#include<vector>

// Header Files
#include "jsonparser.h"
#include "queryreader.h"

int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    std::string fileName;
    std::cout << "Please input json filename to query.\nFor example, \"EX.json\"" << std::endl;
    std::getline(std::cin, fileName);
    fileName = "jsonFiles/" + fileName;

    std::ifstream json(fileName);

    // verify file opened correctly
    if (json_parser::isFileOpen(json) != 0) {
        std::cerr << "Error: Could not open the file " << fileName << std::endl;
        return 1;
    }

    // json_string owns the bytes; the view returned below points into it.
    const std::string json_string = json_parser::jsonToString(json);

    bool repeat = 1;
    while(repeat) {
        // these are temporary until the query reader is rigged up
        std::cout << "Query:: " << std::endl;
        std::string line;
        std::getline(std::cin, line);

        std::vector<std::string> query = {};
        std::istringstream components(line);
        std::string component;
        while (components >> component) {
            query.push_back(component);
        }

        if(query[0] == "X") {
            repeat = 0;
            break;
        }
        
        std::string_view value = json_parser::parsejson(json_string, query);

        if (value.empty()) {
            std::cout << "not found" << std::endl;
            return 1;
        }

        std::cout << value << "\nX to exit, otherwise, query again:: " << std::endl;
    }

    json.close();
    return 0;
}

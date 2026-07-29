// C++ STD libraries
#include<iostream>  // console IO
#include<fstream>   // file IO
#include<string>

// Header Files
#include "jsonparser.h";
#include "queryreader.h";

int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    std::string fileName;
    std::cout << "Please input json filename to query.\nFor example, \"EX.json\"" << std::endl;
    std::cin >> fileName;
    fileName = "src/jsonFiles/" + fileName;
    std::ifstream json(fileName);

    if (!json.is_open()) {
        std::cerr << "Error: Could not open the file " << fileName << std::endl;
        return 1;
    }

    //JSON parse


    json.close();
    return 0;
}
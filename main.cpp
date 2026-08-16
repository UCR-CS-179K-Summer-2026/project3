// C++ STD libraries
#include<iostream>  // console IO
#include<fstream>   // file IO
#include<sstream>
#include<string>
#include<vector>

// Header Files
#include "jsonparser.h"
#include "queryreader.h"

void printWelcomeUI(std::string& fileName) {

    using std::cout;
    using std::endl;
    
    cout << "+-----------------------------------------------+\n";
    cout << "|                                               |\n";
    cout << "|    Welcome to the JSON analytical Engine      |\n";
    cout << "|                                               |\n";
    cout << "|    Made by:                                   |\n";
    cout << "|   Laura Canon, Moustafa Soliman, James Mace   |\n";
    cout << "|                                               |\n";
    cout << "|                    -------                    |\n";
    cout << "|                                               |\n";
    cout << "| Please enter filename you would like to query |\n";
    cout << "| For example : big                             |\n";
    cout << "|                                               |\n";
    cout << "+-----------------------------------------------+\n\n";
}

void openJSONUI() {
    std::cout << "+-----------------------------------------------+\n";
    std::cout << "|                                               |\n";
    std::cout << "|           JSON opened successfully.           |\n";
    std::cout << "|                                               |\n";
    std::cout << "|              Please enter Query               |\n";
    std::cout << "|                                               |\n";
    std::cout << "+-----------------------------------------------+\n\n";
}

std::vector<std::string> getQuery() {
    std::cout << "> " << std::flush;
    // these are temporary until the query reader is rigged up
    std::string line = "";
    std::getline(std::cin, line);

    std::vector<std::string> query = {};
    std::istringstream components(line);
    std::string component = "";
    while (components >> component) {
        query.push_back(component);
    }
    return query;
}

void query(std::string json_string) {
    std::vector<std::string> query = getQuery();
        
    std::string value = json_parser::parsejson(json_string, query);

    std::cout << "\n";

    if (value.empty()) {
        std::cout << "+-----------------------------------------------+\n";
        std::cout << "|                                               |\n";
        std::cout << "|               Value not found.                |\n";
        std::cout << "|                                               |\n";
        std::cout << "|                 Query again?                  |\n";
        std::cout << "|                                               |\n";
        std::cout << "|                    (y/n)                      |\n";
        std::cout << "|                                               |\n";
        std::cout << "+-----------------------------------------------+\n";
    } else {
        std::cout << "+-----------------------------------------------+\n";
        std::cout << "|                                               |\n";
        std::cout << "|                Value found...                 |\n";
        std::cout << "|                                               |\n";
        std::cout << "|                Printing below.                |\n";
        std::cout << "|                                               |\n";
        std::cout << "+-----------------------------------------------+\n";
        std::cout << "\n" << value << "\n\n";
        std::cout << "+-----------------------------------------------+\n";
        std::cout << "|                                               |\n";
        std::cout << "|                 Query again?                  |\n";
        std::cout << "|                                               |\n";
        std::cout << "|                    (y/n)                      |\n";
        std::cout << "|                                               |\n";
        std::cout << "+-----------------------------------------------+\n";
    }
    std::cout << "\n> " << std::flush;
}

int main() {

    std::cin.tie(nullptr);
    std::ios_base::sync_with_stdio(false);

    std::string fileName;
    printWelcomeUI(fileName);

    std::cout << "> " << std::flush;
    std::getline(std::cin, fileName);
    fileName = "jsonFiles/" + fileName + ".json";
    
    std::cout << std::endl;
    std::ifstream json(fileName);

    // verify file opened correctly
    if (json_parser::isFileOpen(json) != 0) {
        std::cerr << "Error: Could not open the file " << fileName << std::endl;
        return 1;
    }

    // json_string owns the bytes; the view returned below points into it.
    const std::string json_string = json_parser::jsonToString(json);

    openJSONUI();
    bool looped = false;
    while(true) {
        if(looped) {
            std::cout << "\n";
            std::cout << "+-----------------------------------------------+\n";
            std::cout << "|                                               |\n";
            std::cout << "|              Please enter Query               |\n";
            std::cout << "|                                               |\n";
            std::cout << "+-----------------------------------------------+\n\n";
        }
        looped = true;
        
        query(json_string);

        // getline, not >>, so the newline does not survive into the next getQuery
        std::string userWantRepeat;
        if(!std::getline(std::cin, userWantRepeat)) {
            break;
        }
        if(!(userWantRepeat == "y" || userWantRepeat == "Y")) {
            break;
        }
    }
    std::cout << "\n";
    std::cout << "+-----------------------------------------------+\n";
    std::cout << "|                                               |\n";
    std::cout << "|                   Exiting...                  |\n";
    std::cout << "|                                               |\n";
    std::cout << "+-----------------------------------------------+\n";

    json.close();
    return 0;
}

// C++ STD libraries
#include<iostream>  // console IO
#include<fstream>   // file IO
#include<string>
#include<vector>

// Header Files
#include "jsonparser.h"
#include "queryparser.h"

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

void query(std::string json_string) {
    std::cout << "> " << std::flush;
    std::string line = "";
    std::getline(std::cin, line);

    std::string value;
    try {
        queryparser::parsequery(line);
        value = json_parser::parsejson(json_string, queryparser::getparsedquery());
    } catch (const std::exception& e) {
        std::cout << "\n" << e.what() << "\n";
        std::cout << "\n> " << std::flush;
        return;
    }

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

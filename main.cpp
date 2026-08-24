// C++ STD libraries
#include<iostream>  // console IO
#include<fstream>   // file IO
#include<string>
#include<vector>

// Header Files
#include "jsonparser.h"
#include "queryparser.h"

void printWelcomeUI(std::string& inputFileName) {

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
    cout << "| For example : big.json                        |\n";
    cout << "|                                               |\n";
    cout << "|      Before entering filename:                |\n";
    cout << "|            For settings,  enter: s            |\n";
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
    std::cout << "+-----------------------------------------------+\n" << std::endl;
}

void settingsUI() {
    std::cout << "+-----------------------------------------------+\n";
    std::cout << "|                                               |\n";
    std::cout << "|    Please choose setting                      |\n";
    std::cout << "|                                               |\n";
    std::cout << "|    Options                                    |\n";
    std::cout << "|                                               |\n";
    std::cout << "|       f   -    pipe results of query to file  |\n";
    std::cout << "|       c   -    pipe results of query to CLI   |\n";
    std::cout << "|       x   -    exit without making changes    |\n";
    std::cout << "|                                               |\n";
    std::cout << "+-----------------------------------------------+\n\n";
}

void query(std::string_view json, bool jsonl, bool outputToFile, std::string& outputFileName) {
    std::cout << "> " << std::flush;
    std::string line = "";
    std::getline(std::cin, line);

    std::vector<std::string> value;

    std::ofstream outputFile(outputFileName, std::ios::app);
    if (!outputFile.is_open()) {
        std::cerr << "Error opening the file pre-parse!" << std::endl;
        return;
    }

    try {
        queryparser::parsequery(line);
        if(jsonl) { 
            value = json_parser::repeatSearch(json, queryparser::getparsedquery());
        } else {
            std::string found = json_parser::parsejson(json, queryparser::getparsedquery());
            if(!found.empty()) value.push_back(std::move(found));
        }
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
        std::cout << "|                Printing below                 |\n";
        if(outputToFile) {
        std::cout << "|                 And to file                   |\n";
        }
        std::cout << "|                                               |\n";
        std::cout << "+-----------------------------------------------+\n";
        if(outputToFile) {
            if (!outputFile.is_open()) {
                std::cerr << "Error opening the file!" << std::endl;
                return;
            }
            outputFile << "New query: " << std::flush;
            queryparser::displaylastparsedquery(); 
            outputFile << "\n";
            for(const std::string& v : value) {
                outputFile << v << "\n";
                outputFile << std::endl;
            }
        }
        for(const std::string& v : value) {
            std::cout << v << "\n";
        }
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

    std::string inputFileName;
    std::string option;
    std::string outputFileName;
    printWelcomeUI(inputFileName);

    std::cout << "> " << std::flush;
    std::getline(std::cin, option);
    bool outputToFile = false;
    std::ofstream outputFile;
    if(option == "s") {
        settingsUI();
        std::cout << "> " << std::flush;
        std::getline(std::cin, option);
        if(option == "c") {
            outputFileName = "CLI";
        } else if (option == "f") {
            std::cout << "Enter outputFile path:\n> " << std::flush;
            std::getline(std::cin, outputFileName);
            outputToFile = true;
        }
        std::cout << "Now, enter fileName to parse\n> " << std::flush;
        std::getline(std::cin, inputFileName);
        inputFileName = "jsonFiles/" + inputFileName;
    } else {
        inputFileName = "jsonFiles/" + option;
    }
    
    std::cout << std::endl;
    
    std::string_view json = json_parser::mapFile(inputFileName);
    if(json == "") {
        std::cout << "Error opening file..." <<std::endl;
        return 1;
    }
    const bool jsonl = inputFileName.ends_with(".jsonl");

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
        
        query(json, jsonl, outputToFile, outputFileName);

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

    outputFile.close();

    return 0;
}

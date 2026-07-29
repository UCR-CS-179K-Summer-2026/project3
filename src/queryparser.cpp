#include <iostream>
#include "queryparser.h"

std::vector<std::string> parsedquery;

namespace queryparser {
    void parsequery(const std::string& query){
        parsedquery.clear();

        // The stack allows us to check if we are in a nested structure.
        std::stack<char> curlybraces;

        std::vector<std::string> querylayout;
        std::string append = "";

        for(char c : query) {
            switch(c){
                case ' ':
                    if(curlybraces.empty()){
                        querylayout.push_back(append);
                        append = "";
                    }
                    
                    break;
                case '{':
                    curlybraces.push('{');
                    append += c;
                    break;
                case '}':
                    if(!curlybraces.empty()){
                        curleybraces.pop();
                    }else{
                        std::cerr << "Error Parsing: Invalid use of brackets." << std::endl;
                        return;
                    }

                    break;
                default:
                    append += c;
                    break;
            }
        }

        parsedquery = querylayout;
    }

    void displaylastparsedquery() {
        std::cout << "[ ";

        for(std::string& query : parsedquery){
            std::cout << query << " ";
        }

        std::cout << "]\n";
    }

    const std::vector<std::string> getparsedquery() {
        return parsedquery;
    }
}
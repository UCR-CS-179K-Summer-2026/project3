#include <iostream>
#include <stack>

#include "queryparser.h"

// Private
namespace{
    std::vector<JSONTypes> parsedquery;

    // Private recursive display helper function for displaylastparsedquery
    void display(const std::vector<JSONTypes> queryStruct) {
        std::cout << "[ ";

        for(const JSONTypes& query : queryStruct){
            switch(query.structure.index()){
                case 0:
                    std::cout << std::get<std::string>(query.structure) << " ";
                    break;
                case 1:
                    // Recursively print the nested structure.
                    display(std::get<std::vector<JSONTypes>>(query.structure));
                    std::cout << " ";
                    break;
            }
        }

        std::cout << "]";
    }

    void parsenested(int& start, const std::string& query, std::vector<JSONTypes>& parsed) {
        start++; // Skip left curly brace
        std::string append = "";
        std::vector<JSONTypes> substructure;

        for(start; start < query.length(); start++){
            char c = query[start];

            switch(c){
                case ' ':
                    substructure.push_back(append);
                    append = "";
                    break;
                case '{':
                    parsenested(start, query, substructure);
                    break;
                case '}':
                    substructure.push_back(append);
                    parsed.push_back(substructure);
                    append = "";
                    substructure.clear();
                    return;
                default:
                    append += c;
                    break;
            }
        }

        substructure.clear();
    }
}

namespace queryparser {
    void parsequery(const std::string& query){
        parsedquery.clear();
        
        std::string append = "";

        for(int i = 0; i < query.length(); i++) {
            char c = query[i];

            switch(c){
                case ' ':
                    parsedquery.push_back(append);
                    append = "";
                    break;
                case '{':
                    parsenested(i, query, parsedquery);
                    i++;
                    break;
                case '}':
                    break; // Skip, we already know its valid.
                default:
                    append += c;
                    break;
            }
        }

        if(append != ""){
            parsedquery.push_back(append);
            append = "";
        }
    }

    // For debugging purposes

    void displaylastparsedquery(){
        display(parsedquery);
        std::cout << std::endl;
    }

    const std::vector<JSONTypes>& getparsedquery() {
        return parsedquery;
    }
}
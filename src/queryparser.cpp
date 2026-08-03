#include <iostream>
#include <stdexcept>
#include <stack>

#include "queryparser.h"

// Private
namespace{
    std::vector<JSONTypes> parsedquery;
    std::stack<char> curlybraces;

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
                    if(!append.empty()){
                        substructure.push_back(append);
                        append.clear();
                    }

                    break;
                case '{':
                    curlybraces.push('{');
                    parsenested(start, query, substructure);
                    break;
                case '}':
                    if(!curlybraces.empty() && curlybraces.top() != '{'){
                        throw std::runtime_error("Parsing Error: Invalid use of curly braces.");
                    }

                    curlybraces.pop();

                    if(!append.empty()){
                        substructure.push_back(append);
                        append.clear();
                    }

                    parsed.push_back(substructure);
                    substructure.clear();
                    return;
                default:
                    append += c;
                    break;
            }
        }
    }
}

namespace queryparser {
    void parsequery(const std::string& query){
        parsedquery.clear();

        if(!curlybraces.empty()){
            curlybraces = std::stack<char>();
        }
        
        std::string append = "";

        for(int i = 0; i < query.length(); i++) {
            char c = query[i];

            switch(c){
                case ' ':
                    if(!append.empty()){
                        parsedquery.push_back(append);
                        append.clear();
                    }

                    break;
                case '{':
                    curlybraces.push('{');
                    parsenested(i, query, parsedquery);
                    break;
                case '}':
                    if(!curlybraces.empty() && curlybraces.top() != '{'){
                        throw std::runtime_error("Parsing Error: Invalid use of curly braces.");
                    }

                    curlybraces.pop();
                    break;
                default:
                    append += c;
                    break;
            }
        }

        if(!curlybraces.empty()){
            throw std::runtime_error("Parsing Error: Invalid use of curly braces.");
        }

        if(append != ""){
            parsedquery.push_back(append);
            append.clear();
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
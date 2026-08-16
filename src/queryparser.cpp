#include <iostream>
#include <stdexcept>
#include <stack>

#include "queryparser.h"

bool JSONType::isstring() const {
    return std::holds_alternative<std::string>(structure);
}

bool JSONType::isvector() const {
    return std::holds_alternative<std::vector<JSONType>>(structure);
}

const std::string& JSONType::asstring() const {
    return std::get<std::string>(structure);
}

const std::vector<JSONType>& JSONType::asvector() const {
    return std::get<std::vector<JSONType>>(structure);
}

// Private
namespace{
    // Stores query parts and filter metadata.
    ParsedQuery parsedquery;

    std::stack<char> curlybraces;

    // Recursive display helper.
    void display(const std::vector<JSONType> queryStruct) {
        std::cout << "[ ";

        for(const JSONType& query : queryStruct){
            switch(query.structure.index()){
                case 0:
                    std::cout << std::get<std::string>(query.structure) << " ";
                    break;

                case 1:
                    display(std::get<std::vector<JSONType>>(query.structure));
                    std::cout << " ";
                    break;
            }
        }

        std::cout << "]";
    }

    void validatecurlybraces(){
        if(curlybraces.empty() || (!curlybraces.empty() && curlybraces.top() != '{')){
            throw std::runtime_error("Parsing Error: Invalid use of curly braces.");
        }

        curlybraces.pop();
    }

    void parsenested(int& start, const std::string& query, std::vector<JSONType>& parsed) {
        start++; // Skip left curly brace
        std::string append = "";
        std::vector<JSONType> substructure;

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
                    validatecurlybraces();
                    
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
        // Reset previous query data.
        parsedquery.parts.clear();
        parsedquery.isRegexFilter = false;
        
        if(!curlybraces.empty()){
            curlybraces = std::stack<char>();
        }
        
        std::string append = "";

        for(int i = 0; i < query.length(); i++) {
            char c = query[i];

            switch(c){
                case ' ':
                    if(!append.empty()){
                        parsedquery.parts.push_back(append);
                        append.clear();
                    }

                    break;

                case '{':
                    curlybraces.push('{');
                    parsenested(i, query, parsedquery.parts);
                    break;

                case '}':
                    validatecurlybraces();
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
            parsedquery.parts.push_back(append);
            append.clear();
        }

        // One FILTER parameter means regex.
        if(!parsedquery.parts.empty()
            && parsedquery.parts[0].isstring()
            && parsedquery.parts[0].asstring() == "FILTER") {

            if(parsedquery.parts.size() == 3){
                parsedquery.isRegexFilter = true;
            }
        }
    }

    // For debugging purposes
    void displaylastparsedquery(){
        display(parsedquery.parts);
        std::cout << std::endl;
    }

    // Return query parts and metadata.
    const ParsedQuery& getparsedquery() {
        return parsedquery;
    }
}
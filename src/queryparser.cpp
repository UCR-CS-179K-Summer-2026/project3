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

std::ostream& operator<<(std::ostream& os, const JSONType& jsontype){
    os << "{ ";
    if(jsontype.isvector()){
        for(const JSONType& elem : jsontype.asvector()){
            os << elem << " ";
        }
    }

    if(jsontype.isstring()){
        os << jsontype.asstring() << " ";
    }

    os << "}";

    return os;
}

// Private
namespace {
    // Stores query parts and filter metadata.
    ParsedQuery parsedquery;

    std::stack<char> curlybraces;

    // Recursive display helper.
    void display(const std::vector<JSONType>& queryStruct) {
        std::cout << "[ ";

        for(const JSONType& query : queryStruct) {
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

    // Reads a quoted token. start sits on the opening quote and lands on the
    // closing one. The quotes are dropped and inner spaces kept, so what is pushed
    // is ready to compare against a JSON key. A backslash escape is copied through
    // as written, because that is how the document spells the key too.
    void parsequoted(int& start, const std::string& query, std::vector<JSONType>& parsed) {
        std::string token = "";
        start++; // Skip opening quote

        for(; start < query.length(); start++){
            char c = query[start];

            if(c == '\\' && start + 1 < query.length()){
                token += c;
                token += query[++start];
                continue;
            }

            if(c == '"'){
                parsed.push_back(token); // Pushed here so "" stays an empty key
                return;
            }

            token += c;
        }

        throw std::runtime_error("Parsing Error: Unterminated quoted string.");
    }

    Query readcommand(const std::string& command) {
        if(command == "FIND")    return Query::FIND;
        if(command == "FILTER")  return Query::FILTER;
        if(command == "DISPLAY") return Query::DISPLAY;
        if(command == "ALLOF")   return Query::ALLOF;

        throw std::runtime_error("Parsing Error: Unknown command '" + command + "'.");
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

                case '"':
                    if(!append.empty()){
                        substructure.push_back(append);
                        append.clear();
                    }

                    parsequoted(start, query, substructure);
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
        parsedquery.command = Query::FIND;
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

                case '"':
                    if(!append.empty()){
                        parsedquery.parts.push_back(append);
                        append.clear();
                    }

                    parsequoted(i, query, parsedquery.parts);
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

        if(!parsedquery.parts.empty()){
            if(!parsedquery.parts[0].isstring()){
                throw std::runtime_error("Parsing Error: Query must begin with a command.");
            }

            parsedquery.command = readcommand(parsedquery.parts[0].asstring());

            // One FILTER parameter means regex.
            if(parsedquery.command == Query::FILTER && parsedquery.parts.size() == 3){
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
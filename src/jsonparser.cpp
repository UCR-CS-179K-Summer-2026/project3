#include "jsonparser.h"

namespace json_parser {

    std::string jsonToString(std::ifstream& json) {
        std::string json_string(
            (std::istreambuf_iterator<char>(json)),
            std::istreambuf_iterator<char>()
        );

        return json_string;
    }
    
    void parsejson(std::string& json, something query) { // TODO
        
        for(char c : json) {
            switch(c) {
                case '{':
                    std::cout << "Read '{' ";
                    break;
                case '}':
                    std::cout << "Read '}' ";
                    break;
                case '[':
                    std::cout << "Read '[' ";
                    break;
                case ']':
                    std::cout << "Read ']' ";
                    break;
                case ',':
                    std::cout << "Read ',' ";
                    break;
                default:
                    std::cout << "Unknown character: " << c << std::endl;
            }
        }

        return;
    }

    int isFileOpen(std::ifstream& json){
        if (!json.is_open()) {
            return 1;
        }
        return 0;
    }  
}
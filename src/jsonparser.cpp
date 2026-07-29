#include "jsonparser.h"

namespace json_parser {
    
    int test(std::ifstream& json){
        if (!json.is_open()) {
            return 1;
        }
        return 0;
    }    
}
#include "queryreader.h"

namespace queryreader {
    const QueryConstruct readquery(const std::vector<JSONType>& parsedquery) {
        Query query;

        if(std::get_if<std::string>(parsedquery[0].structure)){
            switch(std::get<std::string>(parsedquery[0].structure)){
                case "FIND":
                    query = Query::FIND;
                    break;
                case "FILTER":
                    query = Query::FILTER;
                    break;
                case "DISPLAY":
                    query = Query::DISPLAY;
                    break;
                case "ALLOF":
                    query = Query::ALLOF;
                    break;
                default:
                    throw std::runtime_error("Invalid query!");
            }
        }else{
            throw std::runtime_error("Invalid query!");
        }
    }

    void applyquery(const QueryConstruct& applyquery, const std::string& json) {
        // To be implemented
    }
}
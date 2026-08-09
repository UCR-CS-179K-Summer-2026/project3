#include "queryparser.h"
#include "queryreader.h"

namespace queryreader {
    const QueryConstruct readquery(const std::vector<JSONType>& parsedquery) {
        Query query;

        if(parsedquery[0].structure.index() == 0){
            const std::string compare = std::get<std::string>(parsedquery[0].structure);
            if(compare == "FIND")
                query = Query::FIND;
            else if(compare == "FILTER")
                query = Query::FILTER;
            else if(compare == "DISPLAY")
                query = Query::DISPLAY;
            else if(compare == "ALLOF")
                query = Query::ALLOF;
        }else{
            throw std::runtime_error("Invalid query!");
        }

        return QueryConstruct(query, queryparser::getparsedquery());
    }

    void applyquery(const QueryConstruct& applyquery, const std::string& json) {
        // To be implemented
    }
}
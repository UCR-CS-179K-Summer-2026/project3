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

        std::vector<JSONType> params;
        std::vector<JSONType> keys = parsedquery[1].asvector();

        for(int i = 2; i < parsedquery.size(); i++){
            params.push_back(parsedquery[i]);
        }

        switch(params.size()) {
            case 0:
                return QueryConstruct(query, keys);
            case 1:
                if(params[0].isstring())
                    return QueryConstruct(query, keys, params[0].asstring());
                else
                    return QueryConstruct(query, keys, params[0].asvector());
            default:
                return QueryConstruct(query, keys, params);
        }
    }

    void applyquery(const QueryConstruct& applyquery, const std::string& json) {
        // To be implemented
    }
}
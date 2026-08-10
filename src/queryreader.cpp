#include <regex>
#include "queryreader.h"

void QueryFind::applyquery() {
    // TODO
}

void QueryFilter::applyquery() {
    // TODO
}

void QueryDisplay::applyquery() {
    // Laura TODO
}

void QueryAllof::applyquery() {
    // Laura TODO
}

namespace queryreader {
    std::unique_ptr<QueryConstruct> readquery (const std::vector<JSONType>& parsedquery) {
        Query query;

        if(parsedquery[0].isstring()){
            const std::string compare = parsedquery[0].asstring();
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

        switch (query) {
            case Query::FIND:
                return std::make_unique<QueryFind>(keys);
            case Query::FILTER:
                if(params.size() == 1){
                    return std::make_unique<QueryFilter>(keys, params[0].asstring());
                } else {
                    return std::make_unique<QueryFilter>(keys, params[0].asstring(), params[1].asstring());
                }
            // Laura: Implement these cases.
        }

        throw std::runtime_error("Error Reading: Cannot read query.");
    }
}
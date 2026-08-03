#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <string>


struct JSONValueTypes {
    std::variant<
        int,
        double,
        std::string,
        std::vector<JSONValueTypes>
    > value;
};

namespace query_reader {
    
}
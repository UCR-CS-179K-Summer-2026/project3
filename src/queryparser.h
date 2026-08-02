#pragma once

#include <iostream>
#include <vector>
#include <stack>

namespace queryparser {
    void parsequery(const std::string& query);

    // For debugging purposes
    void displaylastparsedquery();
    const std::vector<std::string> getparsedquery(); 
}
#pragma once

#include <iostream>

namespace queryparser {
    void parsequery(std::string& query);

    // For debugging purposes
    void displaylastparsedquery();
}
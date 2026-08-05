#pragma once

#include <iostream>
#include <variant>
#include <vector>
#include <string>

namespace query_reader {
    enum class Query {
        FIND,
        FILTER,
        DISPLAY,
        ALLOF
    };
}
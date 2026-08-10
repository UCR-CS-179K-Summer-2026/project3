// Times reading the small Bookings file, 
// then reading entry 1800 out of the large one.

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "jsonparser.h"

#ifndef JSON_DATA_DIR
#define JSON_DATA_DIR "jsonFiles"
#endif

int singleQuery() {
    std::cout << "How many times to run test? :: " << std::flush;
    unsigned repeat = 0;
    std::cin >> repeat;

    if (!std::cin || repeat <= 0) {
        std::cerr << "Error: please enter a run count of at least 1.\n";
        return 1;
    }

    const std::string small = std::string(JSON_DATA_DIR) + "/Bookings_1-level_1-KB_formatted.json";
    const std::string large = std::string(JSON_DATA_DIR) + "/Bookings_1-level_1-MB_formatted.json";

    double smallTotal = 0.0;
    double largeTotal = 0.0;

    for (unsigned run = 0; run < repeat; ++run) {
        auto start = std::chrono::steady_clock::now();

        std::ifstream smallFile(small);
        std::string smallJson = json_parser::jsonToString(smallFile);
        std::string smallValue = json_parser::parsejson(smallJson, {"0", "Booking_ID"});

        std::chrono::duration<double, std::milli> smallTime = std::chrono::steady_clock::now() - start;
        smallTotal += smallTime.count();
        std::cout << "1 KB, entry 0:    " << smallValue << "  " << smallTime.count() << " ms\n";

        start = std::chrono::steady_clock::now();

        std::ifstream largeFile(large);
        std::string largeJson = json_parser::jsonToString(largeFile);
        std::string largeValue = json_parser::parsejson(largeJson, {"1800", "Booking_ID"});

        std::chrono::duration<double, std::milli> largeTime = std::chrono::steady_clock::now() - start;
        largeTotal += largeTime.count();
        std::cout << "1 MB, entry 1800: " << largeValue << "  " << largeTime.count() << " ms\n";
    }

    if (repeat > 1) {
        std::cout << "\naverage of " << repeat << " runs\n"
                  << "1 KB, entry 0:    " << smallTotal / repeat << " ms\n"
                  << "1 MB, entry 1800: " << largeTotal / repeat << " ms\n";
    }

    return 0;
}

int manyQueries() {
    std::cout << "not implemented yet!" << std::endl;
    return 0;
}

// Times a spread of paths through big.json. The file is read once so the
// timing covers parsing only, and each path is run repeat times because a
// single query on a file this size is too quick to measure.
int bigFile() {
    std::cout << "How many times to run each query? :: " << std::flush;
    unsigned repeat = 0;
    std::cin >> repeat;

    if (!std::cin || repeat < 1) {
        std::cerr << "Error: please enter a run count of at least 1.\n";
        return 1;
    }

    const std::string name = std::string(JSON_DATA_DIR) + "/big.json";
    std::ifstream file(name);
    if (json_parser::isFileOpen(file) != 0) {
        std::cerr << "Error: could not open " << name << "\n";
        return 1;
    }
    const std::string json = json_parser::jsonToString(file);

    struct Query {
        const char* label;
        std::vector<std::string> path;
    };

    // Paired up so the interesting comparisons sit next to each other: a key
    // early in an object against one at the end, a plain key against the same
    // key spelled with escapes, a shallow path against a deep one.
    const std::vector<Query> queries = {
        {"wide, first key",   {"wide", "k000"}},
        {"wide, last key",    {"wide", "last"}},
        {"wide, missing key", {"wide", "nope"}},
        {"raw utf8 key",      {"i18n", "naïve"}},
        {"escaped key",       {"i18n", "escaped", "café"}},
        {"surrogate key",     {"i18n", "escaped", "🔑"}},
        {"escaped quote key", {"escapes", "\\\"a"}},
        {"deep, 15 levels",   {"deep", "l1", "l2", "l3", "l4", "l5", "l6", "l7",
                               "l8", "l9", "l10", "l11", "l12", "l13", "l14", "bottom"}},
        {"deep, escaped keys", {"deepEscaped", "l1", "l2", "l3", "l4", "l5", "bottom"}},
        {"array index",       {"users", "5", "profile", "location", "city"}},
        {"long escaped value", {"escapes", "longEscaped"}},
        {"needle, last in doc", {"needle"}},
    };

    for (const Query& query : queries) {
        std::string value;

        auto start = std::chrono::steady_clock::now();
        for (unsigned run = 0; run < repeat; ++run) {
            value = json_parser::parsejson(json, query.path);
        }
        std::chrono::duration<double, std::micro> elapsed =
            std::chrono::steady_clock::now() - start;

        if (value.size() > 40) {
            value = value.substr(0, 37) + "...";
        }

        std::cout << std::left << std::setw(21) << query.label
                  << std::right << std::setw(9) << std::fixed << std::setprecision(2)
                  << elapsed.count() / repeat << " us   "
                  << (value.empty() ? "(not found)" : value) << "\n";
    }

    return 0;
}


int main() {

    std::cout << "Which test?\n1 : Single Query\n2 : Many Queries\n3 : Big File\n:: " << std::flush;
    int queryType;
    std::cin >> queryType;
    if(!std::cin || queryType < 1) {
        std::cout << "Invalid Query. 1, 2 or 3." << std::endl;
        return 1;
    }

    switch(queryType) {
        case 1:
            singleQuery();
            break;
        case 2:
            manyQueries();
            break;
        case 3:
            bigFile();
            break;
        default:
            return 1;
    }
    return 0;
}

// Times reading the small Bookings file, 
// then reading entry 1800 out of the large one.

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>

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


int main() {

    std::cout << "Which test?\n1 : Single Query\n2 : Many Queries\n:: " << std::flush;
    int queryType;
    std::cin >> queryType;
    if(!std::cin || queryType < 1) {
        std::cout << "Invalid Query. 1 or 2." << std::endl;
        return 1;
    }

    switch(queryType) {
        case 1:
            singleQuery();
            break;
        case 2:
            manyQueries();
            break;
        default:
            return 1;
    }
    return 0;
}

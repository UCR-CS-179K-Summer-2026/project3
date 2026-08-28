#include <stdexcept>
#include <string>
#include <regex>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "queryparser.h"
#include "jsonparser.h"

namespace json_parser {

    int test(std::ifstream& json){
        if (!json.is_open()) {
            return 1;
        }
        return 0;
    }

    namespace {

        bool isWs(char c) {
            return c == ' ' || c == '\t' || c == '\n' || c == '\r';
        }

        void skipWs(const char*& p, const char* end) {
            while (p < end && isWs(*p)) {
                ++p;
            }
        }

        // Reads the four hex digits of the \uXXXX escape whose backslash sits at i.
        // Fails if the escape is cut short by the end of raw or is not valid hex.
        bool readHex4(std::string_view raw, size_t i, unsigned& out) {
            if (i + 5 >= raw.size() || raw[i] != '\\' || raw[i + 1] != 'u') {
                return false;
            }

            unsigned code = 0;
            for (size_t digit = 0; digit < 4; ++digit) {
                char c = raw[i + 2 + digit];
                unsigned value;
                if (c >= '0' && c <= '9') {
                    value = static_cast<unsigned>(c - '0');
                } else if (c >= 'a' && c <= 'f') {
                    value = static_cast<unsigned>(c - 'a') + 10;
                } else if (c >= 'A' && c <= 'F') {
                    value = static_cast<unsigned>(c - 'A') + 10;
                } else {
                    return false;
                }
                code = code * 16 + value;
            }

            out = code;
            return true;
        }

        // this is pretty much defined by UTF specs, just implementing
        // the logic of moving the bits around and whatnot so that
        // == comparison works
        void appendUtf8(std::string& out, unsigned code) {
            if (code <= 0x7F) {
                out += static_cast<char>(code);
            } else if (code <= 0x7FF) {
                out += static_cast<char>(0xC0 | (code >> 6));
                out += static_cast<char>(0x80 | (code & 0x3F));
            } else if (code <= 0xFFFF) {
                out += static_cast<char>(0xE0 | (code >> 12));
                out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (code & 0x3F));
            } else {
                out += static_cast<char>(0xF0 | (code >> 18));
                out += static_cast<char>(0x80 | ((code >> 12) & 0x3F));
                out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                out += static_cast<char>(0x80 | (code & 0x3F));
            }
        }

        // Copies raw, replacing every \uXXXX escape with the UTF-8 encoding of the
        // character it names. A surrogate pair spells a single character and is
        // decoded as one unit. Malformed escapes and unpaired surrogates carry no
        // character, so they are left in place as literal text.
        std::string convertUnicode(std::string_view raw) {
            std::string out;
            out.reserve(raw.size());

            for (size_t i = 0; i < raw.size(); ++i) {
                unsigned code;
                if (!readHex4(raw, i, code)) {
                    out += raw[i];
                    continue;
                }

                size_t width = 6;   // \uXXXX

                if (code >= 0xD800 && code <= 0xDBFF) {          // leading surrogate
                    unsigned trailing;
                    if (!readHex4(raw, i + 6, trailing) ||
                        trailing < 0xDC00 || trailing > 0xDFFF) {
                        out += raw[i];
                        continue;
                    }

                    code = 0x10000 + ((code - 0xD800) << 10) + (trailing - 0xDC00);
                    width = 12;
                } else if (code >= 0xDC00 && code <= 0xDFFF) {   // trailing with no lead
                    out += raw[i];
                    continue;
                }

                appendUtf8(out, code);
                i += width - 1;   // the loop's ++i steps past the last hex digit
            }

            return out;
        }

        // p points at the opening quote. Leaves p just past the closing quote.
        // span is the text between them exactly as the document spells it, and
        // escaped says whether a backslash appears anywhere in that text.
        //
        // convertUnicode only ever rewrites at a backslash, so a span without
        // one already reads as the text it names and callers can compare it
        // where it lies instead of building a converted copy first.
        bool scanStringSpan(const char*& p, const char* end, std::string_view& span, bool& escaped) {
            if (p >= end || *p != '"') {
                return false;
            }

            const char* start = ++p;
            escaped = false;

            while (p < end) {
                if (*p == '\\') {   // escape: skip the backslash AND the next byte
                    if (p + 1 >= end) {
                        return false;   // trailing backslash: unterminated
                    }
                    escaped = true;
                    p += 2;
                    continue;
                }
                if (*p == '"') {
                    span = std::string_view(start, static_cast<size_t>(p - start));
                    ++p;
                    return true;
                }
                ++p;
            }

            return false;   // unterminated
        }

        // Moves p past a string nobody is going to read the contents of.
        bool skipString(const char*& p, const char* end) {
            std::string_view span;
            bool escaped;
            return scanStringSpan(p, end, span, escaped);
        }

        // Moves p past one complete value without interpreting it.
        bool skipValue(const char*& p, const char* end) {
            skipWs(p, end);
            if (p >= end) {
                return false;
            }

            if (*p == '"') {
                return skipString(p, end);
            }

            if (*p == '{' || *p == '[') {
                // count the depth to make sure we skip the correct object
                int depth = 0;
                do {
                    if (p >= end) {
                        return false;
                    }
                    if (*p == '"') {   // braces inside a string are not structure
                        if (!skipString(p, end)) {
                            return false;
                        }
                        continue;      // skipString already moved p
                    }
                    if (*p == '{' || *p == '[') {
                        ++depth;
                    } else if (*p == '}' || *p == ']') {
                        --depth;
                    }
                    ++p;
                } while (depth > 0);

                return true;
            }

            // number, true, false, null: run to the next delimiter
            while (p < end && !isWs(*p) && *p != ',' && *p != '}' && *p != ']') {
                ++p;
            }

            return true;
        }

        bool toIndex(const std::string& component, size_t& out) {
            if (component.empty()) {
                return false;
            }

            size_t value = 0;
            for (char c : component) {
                if (c < '0' || c > '9') {
                    return false;
                }
                value = value * 10 + static_cast<size_t>(c - '0');
            }

            out = value;
            return true;
        }

        // True when a value really sits at p. An index past the last element of an
        // array leaves p on the closing bracket, which names nothing.
        bool hasValue(const char*& p, const char* end) {
            skipWs(p, end);
            return p < end && *p != ']' && *p != '}';
        }

        // Moves p from the start of an object or array to the value the component
        // names. Returns false when this container does not hold it.
        bool seekComponent(const char*& p, const char* end, const std::string& want) {
            skipWs(p, end);
            if (p >= end) {
                return false;
            }

            if (*p == '{') {
                ++p;

                while (p < end) {
                    std::string_view key;
                    bool escaped;
                    skipWs(p, end);
                    if (!scanStringSpan(p, end, key, escaped)) {
                        return false;  // '}' of an empty object, or malformed
                    }

                    skipWs(p, end);
                    if (p >= end || *p != ':') {
                        return false;
                    }
                    ++p;

                    // Only a key with an escape needs to be built out before it can be compared.
                    if (escaped ? convertUnicode(key) == want : key == want) {
                        return true;  // p is now sitting on the value we want
                    }

                    if (!skipValue(p, end)) {
                        return false;
                    }
                    skipWs(p, end);
                    if (p >= end || *p != ',') {
                        return false;  // hit '}': the key is not in this object
                    }
                    ++p;
                }

                return false;  // ran off the end of the object
            }

            if (*p == '[') {
                size_t index = 0;
                if (!toIndex(want, index)) {
                    return false;  // array level needs a numeric component
                }

                ++p;
                for (size_t i = 0; i < index; ++i) {
                    if (!skipValue(p, end)) {
                        return false;
                    }
                    skipWs(p, end);
                    if (p >= end || *p != ',') {
                        return false;  // index past the end
                    }
                    ++p;
                }

                return true;
            }

            return false;  // the query descends but the value is a scalar
        }

        // Walks p along a linear path, one component per level.
        bool seekValue(const char*& p, const char* end, const std::vector<std::string>& path) {
            for (const std::string& want : path) {
                if (!seekComponent(p, end, want)) {
                    return false;
                }
            }

            return hasValue(p, end);
        }

        // Runs a query group against the container that starts at container.
        // Siblings each name a target at this level, so every one of them is looked
        // up from that same starting point. A nested group descends from the sibling
        // right before it, or stays at this level when nothing precedes it. Every
        // target has to be there for the group to hold.
        bool findGroup(const char* container, const char* end, const std::vector<JSONType>& group, size_t from = 0) {
            const char* previous = container;

            for (size_t i = from; i < group.size(); ++i) {
                if (group[i].isstring()) {
                    const char* p = container;
                    if (!seekComponent(p, end, group[i].asstring()) || !hasValue(p, end)) {
                        return false;
                    }

                    previous = p;
                } else if (!findGroup(previous, end, group[i].asvector())) {
                    return false;
                }
            }

            return true;
        }

        // Recurisvely find every key and append its value/return object to out vector.
        bool displayGroup(const char* container, const char* end, const std::vector<JSONType>& group, std::vector<std::string>& out, size_t from = 0) {
            const char* previous = container;

            for (size_t i = from; i < group.size(); ++i) {
                if (group[i].isvector()) {
                    if (!displayGroup(previous, end, group[i].asvector(), out)) {
                        return false;
                    }

                    continue;
                }

                const char* p = container;
                if (!seekComponent(p, end, group[i].asstring()) || !hasValue(p, end)) {
                    return false;
                }

                previous = p;

                // A group follows, so this key only names the way down.
                if (i + 1 < group.size() && group[i + 1].isvector()) {
                    continue;
                }

                const char* start = p;
                if (!skipValue(p, end)) {
                    return false;
                }

                out.push_back(convertUnicode(std::string_view(start, static_cast<size_t>(p - start))));
            }

            return true;
        }

        // Filter Helper Function
        void getWantedKey(const std::vector<JSONType>& group, std::string& want) {
            for(const JSONType& elem : group){
                if(elem.isstring()){
                    want = elem.asstring();
                }else if(elem.isvector()){
                    getWantedKey(elem.asvector(), want);
                }
            }
        }

        // Reaches presumed array
        void reachArray(const char*& p, const char* end, const std::string& want,
            const std::vector<JSONType>& group){
            for(const JSONType& elem : group){
                if(elem.isstring()){
                    if(elem.asstring() != want){
                        seekComponent(p, end, elem.asstring());
                    }
                }else if(elem.isvector()){
                    reachArray(p, end, want, elem.asvector());
                }
            }
        }

        // Function to check if a string is a valid double.
        bool isDouble(const std::string& test){
            try {
                std::stod(test);
            } catch (...) {
                return false;
            }

            return true;
        }

        std::vector<std::string> searchArray(const char*& p, const char* end, 
            std::string& want, double leftbound, double rightbound) {

            std::vector<std::string> found = {};

            skipWs(p, end);
            if (p >= end || *p != '[') return found;
            
            ++p;
            skipWs(p, end);

            if(p < end && *p == ']'){
                ++p;
                return found;
            }

            while (p < end && *p != ']') {
                const char* element = p;

                const char* q = element;              // search off a copy
                bool matches = false;
                if (seekComponent(q, end, want) && hasValue(q, end)) {
                    const char* start = q;
                    if (!skipValue(q, end)) return found;
                    std::regex pattern("^L");
                    std::string value = convertUnicode(std::string_view(start, static_cast<size_t>(q - start)));

                    // This helps prevent a weird stod bug if you were to try for example the name key in employee.json
                    if(isDouble(value)){
                        double doubleValue = std::stod(value);
                        if(doubleValue >= leftbound && doubleValue <= rightbound) {
                            matches = true;
                        }
                    }
                }

                p = element; // go back to beginning of object and skip the whole thing.
                // not that smart but really only way to work with skipvalue func
                if (!skipValue(p, end)) return found; // step over the whole element
                                    
                if(matches){
                    found.push_back(convertUnicode(std::string_view(element, p - element)));
                }

                skipWs(p, end);
                if (p >= end || *p != ',') break;     // ']' or malformed
                ++p; // was at ',', now at '{'
            }

            return found;
        }

        std::vector<std::string> searchArray(const char*& p, const char* end, 
            const std::string& want, 
            const std::string& regex) {
            
            std::vector<std::string> found = {};

            skipWs(p, end);
            if (p >= end || *p != '[') return found;
            
            ++p;
            skipWs(p, end);

            if(p < end && *p == ']'){
                ++p;
                return found;
            }

            while (p < end && *p != ']') {
                const char* element = p;

                const char* q = element;              // search off a copy
                bool matches = false;
                if (seekComponent(q, end, want) && hasValue(q, end)) {
                    const char* start = q;
                    if (!skipValue(q, end)) return found;
                    std::regex pattern(regex);
                    std::string value = convertUnicode(std::string_view(start, static_cast<size_t>(q - start)));

                    value = value.substr(1, value.size() - 2); // Ignore quotation marks.
                    matches = std::regex_search(value, pattern);
                }

                p = element; // go back to beginning of object and skip the whole thing.
                // not that smart but really only way to work with skipvalue func
                if (!skipValue(p, end)) return found; // step over the whole element
                                    
                if(matches){
                    found.push_back(convertUnicode(std::string_view(element, p - element)));
                }

                skipWs(p, end);
                if (p >= end || *p != ',') break;     // ']' or malformed
                ++p; // was at ',', now at '{'
            }

            return found;
        }

        void filterGroup(const char* container, 
            const char* end, 
            const std::vector<JSONType>& group, 
            std::vector<std::string>& output,
            size_t from,
            bool isRegexFilter
        ){
            std::string want;

            if(group[1].isvector()){
                getWantedKey(group[1].asvector(), want);
                reachArray(container, end, want, group[1].asvector());
            }else{
                throw std::runtime_error("Second parameter has to be a vector.");
            }

            if(isRegexFilter){
                if(group[2].isstring()){
                    output = searchArray(container, end, want, group[2].asstring());
                }else{
                    throw std::runtime_error("Invalid regex.");
                }
            } else {
                if(isDouble(group[2].asstring()) && isDouble(group[3].asstring())){
                    output = searchArray(container, end, want, std::stod(group[2].asstring()), std::stod(group[3].asstring()));
                }else{
                    throw std::runtime_error("Invalid doubles inserted.");
                }
            }
        }

        // If just one, print "x", if many, print as array
        // like ["x", "y", "z"].
        std::string joinValues(const std::vector<std::string>& values) {
            if (values.size() == 1) {
                return values[0];
            }

            std::string out = "[";
            for (size_t i = 0; i < values.size(); ++i) {
                if (i > 0) {
                    out += ", ";
                }
                out += values[i];
            }
            out += "]";

            return out;
        }

        // True when the line contains no non-whitespace characters.
        bool isBlank(std::string_view record) {
            for (char c : record) {
                if (!isWs(c)) {
                    return false;
                }
            }

            return true;
        }

        // Checks that one JSONL line contains exactly one complete JSON value.
        bool validJsonRecord(std::string_view record) {
            const char* p = record.data();
            const char* end = p + record.size();

            skipWs(p, end);

            if (p >= end) {
                return false;
            }

            if (!skipValue(p, end)) {
                return false;
            }

            // Nothing except whitespace should remain after the JSON value.
            skipWs(p, end);
            return p == end;
        }

        // Return one JSONL record by zero-based line index.
        std::string_view recordAt(std::string_view json, size_t index) {
            size_t start = 0;
            size_t current = 0;

            while (start < json.size()) {

                // Find the end of this JSONL record.
                size_t end = json.find('\n', start);

                // Last record may not end with '\n'.
                if (end == std::string_view::npos) {
                    end = json.size();
                }

                std::string_view record = json.substr(start, end - start);

                // Remove Windows '\r' from "\r\n".
                if (!record.empty() && record.back() == '\r') {
                    record.remove_suffix(1);
                }

                // Count only non-empty JSONL records.
                if (!isBlank(record)) {
                    if (current == index) {
                        return record;
                    }

                    ++current;
                }

                start = end + 1;
            }

            // Requested record does not exist.
            return {};
        }
    }

    std::string jsonToString(std::ifstream& json) {
        std::string json_string(
            (std::istreambuf_iterator<char>(json)),
            std::istreambuf_iterator<char>()
        );

        return json_string;
    }
    
    std::string parsejson(std::string_view json, const std::vector<std::string>& path) {

        const char* p = json.data();
        const char* end = p + json.size();

        if (!seekValue(p, end, path)) {
            return {};
        }

        const char* start = p;
        if (!skipValue(p, end)) {
            return {};
        }

        return convertUnicode(std::string_view(start, static_cast<size_t>(p - start)));
    }

    std::string parsejson(std::string_view json, const ParsedQuery& query) {
        std::vector<std::string> values;

        switch (query.command) {

            case Query::FIND: {
                const char* p = json.data();
                const char* end = p + json.size();

                // Everything after the command names targets to check.
                return findGroup(p, end, query.parts, 1) ? "true" : "false";
            }

            case Query::DISPLAY: {
                const char* p = json.data();
                const char* end = p + json.size();

                if (!displayGroup(p, end, query.parts, values, 1) || values.empty()) {
                    return {};  // a missing target means nothing was found
                }

                break;
            }

            case Query::FILTER: {
                const char* p = json.data();
                const char* end = p + json.size();

                filterGroup(p, end, query.parts, values, 1, query.isRegexFilter);
                
                break;
            }

            default:
                throw std::runtime_error("Unsupported query type.");
        }

        return joinValues(values);
    }

    // JSONL: process each line as one independent JSON record.
    std::vector<std::string> repeatSearch(std::string_view json, const ParsedQuery& query)
    {
        std::vector<std::string> values;

        // If the first query part is a number, query only that JSONL record.
        size_t index = 0;
        if (query.parts.size() > 1 &&
            query.parts[1].isstring() &&
            toIndex(query.parts[1].asstring(), index)) {

            std::string_view record = recordAt(json, index);

            // Requested JSONL record does not exist.
            if (record.empty()) {
                return {};
            }

            // Reject a malformed record selected by index.
            if (!validJsonRecord(record)) {
                throw std::runtime_error(
                    "Invalid JSONL record at index " +
                    std::to_string(index)
                );
            }

            // If only an index was given, return the whole record.
            if (query.parts.size() == 2) {
                return { convertUnicode(record) };
            }

            // Remove the record index before running the normal JSON query.
            ParsedQuery rest = query;
            rest.parts.erase(rest.parts.begin() + 1);

            std::string value = parsejson(record, rest);

            if (value.empty()) {
                return {};
            }

            return { std::move(value) };
        }

        size_t start = 0;
        size_t lineNumber = 1;

        while (start < json.size()) {

            // Find the end of the current JSONL record.
            size_t end = json.find('\n', start);

            // Last line may not end with '\n'.
            if (end == std::string_view::npos) {
                end = json.size();
            }

            // string_view avoids copying each record.
            std::string_view record = json.substr(start, end - start);

            // Support Windows-style "\r\n" line endings.
            if (!record.empty() && record.back() == '\r') {
                record.remove_suffix(1);
            }

            // Reject malformed non-blank JSONL records.
            if (!isBlank(record) && !validJsonRecord(record)) {
                throw std::runtime_error(
                    "Invalid JSONL record on line " +
                    std::to_string(lineNumber)
                );
            }
            // Skip blank and whitespace-only lines.
            if (!isBlank(record)) {
                std::string value = parsejson(record, query);

            // FIND only needs one successful match.
            if (query.command == Query::FIND) {
                if (value == "true") {
                    return {"true"};
                }
            }
            // Skip empty FILTER results from individual JSONL records.
            else if (!value.empty() && value != "[]") {
                values.push_back(std::move(value));
            }

            }
            // Move to the beginning of the next record.
            start = end + 1;
            ++lineNumber;
        }

        // No JSONL record matched FIND.
        if (query.command == Query::FIND) {
            return {"false"};
        }

        return values;
    }

    std::string_view mapFile(const std::string& path) {
        int fd = ::open(path.c_str(), O_RDONLY);
        if(fd < 0) return {};

        off_t size = ::lseek(fd, 0, SEEK_END);

        const char* data = static_cast<const char*>(::mmap(nullptr, size, PROT_READ, MAP_PRIVATE, fd, 0));
        
        ::close(fd);

        if(data == MAP_FAILED) return {};
        return std::string_view(data, size);
    }

    int isFileOpen(std::ifstream& json){
        if (!json.is_open()) {
            return 1;
        }
        return 0;
    }  
}
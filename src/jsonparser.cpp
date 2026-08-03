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

        // p points at the opening quote. Leaves p just past the closing quote,
        // and raw = the text between them.
        bool scanString(const char*& p, const char* end, std::string_view& raw) {
            if (p >= end || *p != '"') {
                return false;
            }

            const char* start = ++p;

            while (p < end) {
                if (*p == '\\') {   // escape: skip the backslash AND the next byte
                    p += 2;
                    continue;
                }
                if (*p == '"') {
                    raw = std::string_view(start, static_cast<size_t>(p - start));
                    ++p;
                    return true;
                }
                ++p;
            }

            return false;   // unterminated
        }

        // Moves p past one complete value without interpreting it.
        bool skipValue(const char*& p, const char* end) {
            skipWs(p, end);
            if (p >= end) {
                return false;
            }

            if (*p == '"') {
                std::string_view ignored;
                return scanString(p, end, ignored);
            }

            if (*p == '{' || *p == '[') {
                // One counter is enough: we are skipping, not validating, so it
                // does not matter whether a ']' closes a '{'.
                int depth = 0;
                do {
                    if (p >= end) {
                        return false;
                    }
                    if (*p == '"') {   // braces inside a string are not structure
                        std::string_view ignored;
                        if (!scanString(p, end, ignored)) {
                            return false;
                        }
                        continue;      // scanString already moved p
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

    }

    std::string jsonToString(std::ifstream& json) {
        std::string json_string(
            (std::istreambuf_iterator<char>(json)),
            std::istreambuf_iterator<char>()
        );

        return json_string;
    }
    
    std::string_view parsejson(std::string_view json, const std::vector<std::string>& query) {

        const char* p = json.data();
        const char* end = p + json.size();

        for (const std::string& want : query) {
            skipWs(p, end);
            if (p >= end) {
                return {};
            }

            if (*p == '{') {
                ++p;

                bool matched = false;
                while (p < end) {
                    std::string_view key;
                    skipWs(p, end);
                    if (!scanString(p, end, key)) {
                        return {};   // '}' of an empty object, or malformed
                    }

                    skipWs(p, end);
                    if (p >= end || *p != ':') {
                        return {};
                    }
                    ++p;

                    if (key == want) {
                        matched = true;
                        break;       // p is now sitting on the value we want
                    }

                    if (!skipValue(p, end)) {
                        return {};
                    }
                    skipWs(p, end);
                    if (p >= end || *p != ',') {
                        return {};   // hit '}': the key is not in this object
                    }
                    ++p;
                }

                if (!matched) {
                    return {};
                }
            } else if (*p == '[') {
                size_t index = 0;
                if (!toIndex(want, index)) {
                    return {};       // array level needs a numeric component
                }

                ++p;
                for (size_t i = 0; i < index; ++i) {
                    if (!skipValue(p, end)) {
                        return {};
                    }
                    skipWs(p, end);
                    if (p >= end || *p != ',') {
                        return {};   // index past the end
                    }
                    ++p;
                }
            } else {
                return {};           // path continues but the value is a scalar
            }
        }

        skipWs(p, end);
        const char* start = p;
        if (!skipValue(p, end)) {
            return {};
        }

        return std::string_view(start, static_cast<size_t>(p - start));
    }

    int isFileOpen(std::ifstream& json){
        if (!json.is_open()) {
            return 1;
        }
        return 0;
    }  
}
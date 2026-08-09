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

        // p points at the opening quote. Leaves p just past the closing quote,
        // and out = the text between them with \uXXXX escapes converted to ASCII.
        bool scanString(const char*& p, const char* end, std::string& out) {
            if (p >= end || *p != '"') {
                return false;
            }

            const char* start = ++p;

            while (p < end) {
                if (*p == '\\') {   // escape: skip the backslash AND the next byte
                    if (p + 1 >= end) {
                        return false;   // trailing backslash: unterminated
                    }
                    p += 2;
                    continue;
                }
                if (*p == '"') {
                    out = convertUnicode(std::string_view(start, static_cast<size_t>(p - start)));
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
                std::string ignored;
                return scanString(p, end, ignored);
            }

            if (*p == '{' || *p == '[') {
                // count the depth to make sure we skip the correct object
                int depth = 0;
                do {
                    if (p >= end) {
                        return false;
                    }
                    if (*p == '"') {   // braces inside a string are not structure
                        std::string ignored;
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
    
    std::string parsejson(std::string_view json, const std::vector<std::string>& query) {

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
                    std::string key;
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

        return convertUnicode(std::string(start, static_cast<size_t>(p - start)));
    }

    int isFileOpen(std::ifstream& json){
        if (!json.is_open()) {
            return 1;
        }
        return 0;
    }  
}
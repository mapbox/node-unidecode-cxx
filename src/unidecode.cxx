
#include <string>
#include "../deps/utf8cpp/utf8.h"
#include "data.cxx"

void unidecode(const char * data, std::size_t len, std::string & output) {

    const char* str_i = data;
    const char* end = str_i + len;

    while (str_i < end) {
        uint32_t code = 0;
        try {
            code = utf8::next(str_i, end); // get 32 bit code of a utf-8 symbol
        } catch (std::exception const &) {
            str_i++;
            continue;
        }

        if (code == 0) {
            output += '\x00';
            continue;
        }

        if (code > 0xFFFF) {
            // in this situation in the upstream code, ostensibly an underscore is appended
            // but the UTF8 parsing of >16bit characters in node-unidecode is incorrect as far
            // as I can tell, so this code path is never exercised and nothing is appended instead
            // so I'm emulating that behavior here

            //*output += '_';
            continue;
        } else {
            uint32_t h = code >> 8;

            // (18) 18 > h < 1e (30)
            if (h > 24 && h < 30) continue;

            //(d7) 215 > h < 249 (f9) no supported
            if (h > 215 && h < 249) continue;

            const auto * static_data = UNIDECODE_DATA[h];
            if (static_data) {
                uint32_t l = code & 0xFF;
                auto const& row = static_data[l];
                if (!row.empty()) {
                    output.append(row.data(),row.size());;
                }
            }
        }
    }
}

bool can_decode(const char * data, std::size_t len) {
    const char* str_i = data;
    const char* end = str_i + len;

    bool hit = false;
    try {
        while (str_i < end) {
            uint32_t code = 0;
            try {
                code = utf8::next(str_i, end);
            } catch (std::exception const &) {
                str_i++;
                continue;
            }

            if (code == 0) {
                hit = true;
                continue;
            }

            if (code > 0xFFFF) {
                continue;
            } else {
                uint32_t h = code >> 8;
                if (h > 24 && h < 30) continue;
                if (h > 215 && h < 249) continue;
                if (UNIDECODE_DATA[h]) {
                    hit = true;
                }
            }
        };
    } catch (std::exception const&) {
        return false;
    }
    return true;
}
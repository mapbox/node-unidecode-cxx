#include <iostream>
#include <string>
#include <sstream>
#include "../deps/utf8cpp/utf8.h"

#include "data.cxx"

using namespace std;

void unidecode(string input, stringbuf* output_buf) {
    ostream output_stream(output_buf);

    char* str_i = (char*) input.data();
    char* end = str_i + input.length();

    do {
        uint32_t code;
        try {
            code = utf8::next(str_i, end); // get 32 bit code of a utf-8 symbol
        } catch (utf8::exception) {
            str_i++;
            continue;
        }

        if (code == 0) {
            output_buf->sputc('\x00');
            continue;
        }

        if (code > 0xFFFF) {
            // in this situation in the upstream code, ostensibly an underscore is appended
            // but the UTF8 parsing of >16bit characters in node-unidecode is incorrect as far
            // as I can tell, so this code path is never exercised and nothing is appended instead
            // so I'm emulating that behavior here

            //output_stream << "_";
            continue;
        } else {
            uint32_t h = code >> 8;
            uint32_t l = code & 0xFF;

            // (18) 18 > h < 1e (30)
            if (h > 24 && h < 30) continue;

            //(d7) 215 > h < 249 (f9) no supported
            if (h > 215 && h < 249) continue;

            if (UNIDECODE_DATA[h]) {
                output_stream << UNIDECODE_DATA[h][l];
            }
        }
    } while (str_i < end);
}

// int main() {
//     stringbuf output_buf;
//     unidecode("\xe0\xb4\x85\xe0\xb4\xad\xe0\xb4\xbf\xe0\xb4\x9c\xe0\xb5\x80\xe0\xb4\xa4", &output_buf);
//     cout << output_buf.str() << endl;
// }
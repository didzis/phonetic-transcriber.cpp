#include <utf8.h>

#include "util.h"


std::vector<uint32_t> utf8ToCodePoints(const std::string& text) {

    // https://stackoverflow.com/a/2856241
    char *start = (char*)text.c_str();    // utf-8 string
    char *end = start + text.size();      // end iterator
    char *cur = start;                    // string iterator

    std::vector<uint32_t> codepoints;
    codepoints.reserve(text.size());      // at most

    while(cur < end) {
        uint32_t code = utf8::next(cur, end); // get 32 bit code of a utf-8 symbol
        if (code == 0)
            continue;
        codepoints.push_back(code);
    }

    return codepoints;
}

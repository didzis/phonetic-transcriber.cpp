#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include "utf8lower.h"

// https://stackoverflow.com/questions/36897781/how-to-uppercase-lowercase-utf-8-characters-in-c
// https://stackoverflow.com/a/69555744
// https://www.alphabet.se/download/UtfConv.c
// $ curl -O https://www.alphabet.se/download/UtfConv.c
//
// possible alternative approach: https://pastebin.com/fuw4Uizk

extern "C" {
    // defined in https://www.alphabet.se/download/UtfConv.c
    unsigned char* Utf8StrMakeLwrUtf8Str(const unsigned char* s);
}

std::string toLowerCase(const std::string& s) {
    auto lower = Utf8StrMakeLwrUtf8Str((unsigned const char*)s.c_str());
    std::string output((const char*)lower);
    free(lower);
    return output;
}

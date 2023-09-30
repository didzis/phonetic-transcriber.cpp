
#include <iostream>

#include <utf8.h>

#include "converter.h"
#include "data.h"



AlphaNumericSimplifiedCharacterConverter::AlphaNumericSimplifiedCharacterConverter() {
    fromIPAafter = getCharsetMap("AlphaNumericSimplifiedCharacterConverter_fromIPAafter");
    fromIPAbefore = getCharsetMap("AlphaNumericSimplifiedCharacterConverter_fromIPAbefore");
    fromIPAresult = getCharsetMap("AlphaNumericSimplifiedCharacterConverter_fromIPAresult");
}

std::string AlphaNumericSimplifiedCharacterConverter::toIPAchar(const std::string& str) const {
    return str;
}

std::string AlphaNumericSimplifiedCharacterConverter::fromIPAchar(const std::string& str) const {
    char *start = (char*)str.c_str();
    char *end = start + std::strlen(start);
    if(utf8::distance(start, end) > 1) {
        char *after = end;
        utf8::advance(after, -1, start);
        char *before = start;
        utf8::advance(before, 1, end);
        if(fromIPAafter.count(std::string(after, end - after)) > 0) {
            end = after;
        }
        if(fromIPAbefore.count(std::string(start, before - start)) > 0) {
            start = before;
        }
    }
    auto it = fromIPAresult.find(std::string(start, end - start));
    if(it != fromIPAresult.end()) {
        return it->second;
    }
    return "";
}

std::vector<std::string> AlphaNumericSimplifiedCharacterConverter::tokenize(const std::string& word) const {
    return std::vector<std::string>();
}




AlphaNumericCharacterConverter::AlphaNumericCharacterConverter() {
    toIPAafter = getCharsetMap("AlphaNumericCharacterConverter_toIPAafter");
    toIPAbefore = getCharsetMap("AlphaNumericCharacterConverter_toIPAbefore");
    toIPAresult = getCharsetMap("AlphaNumericCharacterConverter_toIPAresult");
    fromIPAafter = getCharsetMap("AlphaNumericCharacterConverter_fromIPAafter");
    fromIPAbefore = getCharsetMap("AlphaNumericCharacterConverter_fromIPAbefore");
    fromIPAresult = getCharsetMap("AlphaNumericCharacterConverter_fromIPAresult");
}

std::string AlphaNumericCharacterConverter::toIPAchar(const std::string& str) const {
    std::string a, b;

    char *start = (char*)str.c_str();
    char *end = start + std::strlen(start);
    auto d = utf8::distance(start, end);
    if(d > 4) {
        char *after = end;
        utf8::advance(after, -4, start);
        char *before = start;
        utf8::advance(before, 4, end);

        std::string s;

        {
            s = std::string(after, end - after);
            auto it = toIPAafter.find(s);
            if(it != toIPAafter.end()) {
                end = after;
                a = it->second;
            }
        }

        {
            s = std::string(start, before - start);
            auto it = toIPAbefore.find(s);
            if(it != toIPAbefore.end()) {
                start = before;
                b = it->second;
            }
        }

    }
    std::string result;
    if(start < end) {
        auto it = toIPAresult.find(std::string(start, end - start));
        if(it != toIPAresult.end())
            result = it->second.c_str();
    }
    if(result.empty()) {
        return result;
    }
    if(!b.empty())
        result = b + result;
    if(!a.empty())
        result += a;

    return result;
}

std::string AlphaNumericCharacterConverter::fromIPAchar(const std::string& str) const {
    std::string a, b;

    char *start = (char*)str.c_str();
    char *end = start + std::strlen(start);
    auto d = utf8::distance(start, end);
    if(d > 1) {
        char *after = end;
        utf8::advance(after, -1, start);
        char *before = start;
        utf8::advance(before, 1, end);

        std::string s;

        {
            s = std::string(after, end - after);
            auto it = fromIPAafter.find(s);
            if(it != fromIPAafter.end()) {
                end = after;
                a = it->second;
            }
        }

        {
            s = std::string(start, before - start);
            auto it = fromIPAbefore.find(s);
            if(it != fromIPAbefore.end()) {
                start = before;
                b = it->second;
            }
        }

    }
    std::string result;
    if(start < end) {
        auto it = fromIPAresult.find(std::string(start, end - start));
        if(it != fromIPAresult.end())
            result = it->second.c_str();
    }
    if(result.empty()) {
        return result;
    }
    if(!b.empty())
        result = b + result;
    if(!a.empty())
        result += a;

    return result;
}

std::vector<std::string> AlphaNumericCharacterConverter::tokenize(const std::string& word) const {
    return std::vector<std::string>();
}




AlphabeticCharacterConverter::AlphabeticCharacterConverter() {
    toIPAafter = getCharsetMap("AlphabeticCharacterConverter_toIPAafter");
    toIPAbefore = getCharsetMap("AlphabeticCharacterConverter_toIPAbefore");
    toIPAresult = getCharsetMap("AlphabeticCharacterConverter_toIPAresult");
    fromIPAafter = getCharsetMap("AlphabeticCharacterConverter_fromIPAafter");
    fromIPAbefore = getCharsetMap("AlphabeticCharacterConverter_fromIPAbefore");
    fromIPAresult = getCharsetMap("AlphabeticCharacterConverter_fromIPAresult");
}

std::string AlphabeticCharacterConverter::toIPAchar(const std::string& str) const {
    std::string a, b;

    char *start = (char*)str.c_str();
    char *end = start + std::strlen(start);
    auto d = utf8::distance(start, end);
    if(d > 1) {
        char *after = end;
        utf8::advance(after, -1, start);
        char *before = start;
        utf8::advance(before, 1, end);

        std::string s;

        {
            s = std::string(after, end - after);
            auto it = toIPAafter.find(s);
            if(it != toIPAafter.end()) {
                end = after;
                a = it->second;
            }
        }

        {
            s = std::string(start, before - start);
            auto it = toIPAbefore.find(s);
            if(it != toIPAbefore.end()) {
                start = before;
                b = it->second;
            }
        }

    }
    std::string result;
    if(start < end) {
        auto it = toIPAresult.find(std::string(start, end - start));
        if(it != toIPAresult.end())
            result = it->second.c_str();
    }
    if(!b.empty())
        result = b + result;
    if(!a.empty())
        result += a;

    return result;
}

std::string AlphabeticCharacterConverter::fromIPAchar(const std::string& str) const {
    std::string a, b;

    char *start = (char*)str.c_str();
    char *end = start + std::strlen(start);
    auto d = utf8::distance(start, end);
    if(d > 0) {
        char *after = end;
        utf8::advance(after, -1, start);
        char *before = start;
        utf8::advance(before, 1, end);

        std::string s;

        if(d > 2 && utf8::peek_next(after, end) == 0x02d0) {
            a = "=";
            end = after;
        } else {
            s = std::string(after, end - after);
            auto it = fromIPAafter.find(s);
            if(it != fromIPAafter.end()) {
                end = after;
                a = it->second;
            }
        }

        {
            s = std::string(start, before - start);
            auto it = fromIPAbefore.find(s);
            if(it != fromIPAbefore.end()) {
                start = before;
                b = it->second;
            }
        }

    }
    std::string result;
    if(start < end) {
    auto it = fromIPAresult.find(std::string(start, end - start));
    if(it != fromIPAresult.end())
        result = it->second.c_str();
    }
    if(!b.empty())
        result = b + result;
    if(!a.empty())
        result += a;

    return result;
}

std::vector<std::string> AlphabeticCharacterConverter::tokenize(const std::string& word) const {
    return std::vector<std::string>();
}




IPASimplifiedCharacterConverter::IPASimplifiedCharacterConverter() {
    fromIPAafter = getCharsetMap("IPASimplifiedCharacterConverter_fromIPAafter");
    fromIPAbefore = getCharsetMap("IPASimplifiedCharacterConverter_fromIPAbefore");
    fromIPAresult = getCharsetMap("IPASimplifiedCharacterConverter_fromIPAresult");
    auto& charsets = getCharsetMap("IPASimplifiedCharacterConverter_charsets");
    charset0 = charsets.at("charset0");
    charset1 = charsets.at("charset1");
    charset2 = charsets.at("charset2");
}

std::string IPASimplifiedCharacterConverter::toIPAchar(const std::string& str) const {
    return str;
}

std::string IPASimplifiedCharacterConverter::fromIPAchar(const std::string& str) const {
    // std::string a, b;

    char *start = (char*)str.c_str();
    char *end = start + std::strlen(start);
    auto d = utf8::distance(start, end);
    if(d > 1) {
        char *after = end;
        utf8::advance(after, -1, start);
        char *before = start;
        utf8::advance(before, 1, end);

        std::string s;

        if(d > 2 && utf8::peek_next(after, end) == 0x02d0) {
            // a = "=";
            end = after;
        } else {
            s = std::string(after, end - after);
            auto it = fromIPAafter.find(s);
            if(it != fromIPAafter.end()) {
                end = after;
                // a = it->second;
            }
        }

        {
            s = std::string(start, before - start);
            auto it = fromIPAbefore.find(s);
            if(it != fromIPAbefore.end()) {
                start = before;
                // b = it->second;
            }
        }

    }
    std::string result;
    if(start < end) {
        auto it = fromIPAresult.find(std::string(start, end - start));
        if(it != fromIPAresult.end())
            result = it->second.c_str();
    }
    // if(!b.empty())
    //     result = b + result;
    // if(!a.empty())
    //     result += a;

    return result;
}

std::vector<std::string> IPASimplifiedCharacterConverter::tokenize(const std::string& word) const {
    std::vector<std::string> tokens;
    int state = 0;
    std::string token;
    char *start = (char*)word.c_str();
    char *end = start + word.size();
    char *cur = start, *next;
    for(; cur < next; ) {
        next = cur;
        utf8::next(next, end);
        if(state == 0) {
            if(cur > start && (charset0.find(cur, 0, next-cur) != std::string::npos || charset1.find(cur, 0, next-cur) != std::string::npos)) {
                tokens.push_back(token);
                token.clear();
            }
            if(charset0.find(cur, 0, next-cur) != std::string::npos) {
                state = 1;
            // } else if(charset2.find(cur, 0, next-cur) != std::string::npos) {
            } else if(charset2.compare(0, charset2.size(), cur, (next-cur)) == 0) {
                state = 1;
            }
        } else if(state == 1) {
            state = 0;
        }
        token.insert(token.size(), cur, next-cur);
    }
    return tokens;
}




IPACharacterConverter::IPACharacterConverter() {
    auto& charsets = getCharsetMap("IPACharacterConverter_charsets");
    charset0 = charsets.at("charset0");
    charset1 = charsets.at("charset1");
    charset2 = charsets.at("charset2");
}

std::string IPACharacterConverter::toIPAchar(const std::string& str) const {
    return str;
}

std::string IPACharacterConverter::fromIPAchar(const std::string& str) const {
    return str;
}

std::vector<std::string> IPACharacterConverter::tokenize(const std::string& word) const {
    std::vector<std::string> tokens;
    int state = 0;
    std::string token;
    char *start = (char*)word.c_str();
    char *end = start + word.size();
    char *cur = start, *next;
    for(; cur < next; ) {
        next = cur;
        utf8::next(next, end);
        if(state == 0) {
            if(cur > word && (charset0.find(cur, 0, next-cur) != std::string::npos || charset1.find(cur, 0, next-cur) != std::string::npos)) {
                tokens.push_back(token);
                token.clear();
            }
            if(charset0.find(cur, 0, next-cur) != std::string::npos) {
                state = 1;
            // } else if(charset2.find(cur, 0, next-cur) != std::string::npos) {
            } else if(charset2.compare(0, charset2.size(), cur, (next-cur)) == 0) {
                state = 1;
            }
        } else if(state == 1) {
            state = 0;
        }
        token.insert(token.size(), cur, next-cur);
    }
    return tokens;
}




void test_eq(const std::string& s1, const std::string& s2) {
    std::cout << (s1 == s2 ? "OK  " : "FAIL") << "  " << s1 << " ?= " << s2 << std::endl;
}

void test_alphabetic_character_converter() {
    auto converter = AlphabeticCharacterConverter();

    test_eq("%", converter.fromIPAchar("\u02cc"));
    test_eq("\u02cc", converter.toIPAchar("%"));
    test_eq("\"", converter.fromIPAchar("\u02c8"));
    test_eq("\u02c8", converter.toIPAchar("\""));
    test_eq("aq", converter.fromIPAchar("\u0251\u02c0"));
    test_eq("\u0251\u02c0", converter.toIPAchar("aq"));
    test_eq("aa=", converter.fromIPAchar("\u0251\u02d0\u02d0"));
    test_eq("\u0251\u02d0\u02d0", converter.toIPAchar("aa="));
    test_eq("aa=", converter.fromIPAchar(converter.toIPAchar("aa=")));
}

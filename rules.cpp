
#include <cstring>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <utf8.h>

#include "rules.h"
#include "data.h"
#include "util.h"



Rules::Rules() {

    for(const auto rule : getRules()) {
        char *str = (char*)rule.text.c_str();                     // utf-8 string
        uint32_t code = utf8::next(str, str + rule.text.size());  // get 32 bit code of a utf-8 symbol
        rules[code].push_back(rule);
    }

    metaRules = getMetaRules();

    // print output for debug
    // for(const auto& it : rules) {
    //     const auto& code = it.first;
    //     for(const auto& r : it.second) {
    //         std::cout << "RULE with code:" << code << "  rule:" << r.text << " -> " << r.replacement << " L:" << r.left.size() << " R:" << r.right.size() << std::endl;
    //         for(int i=0; i<r.left.size(); i++) {
    //             std::cout << "left:" << (int)r.left[i].type << " " << r.left[i].value << std::endl;
    //         }
    //         for(int i=0; i<r.right.size(); i++) {
    //             std::cout <<t "right:" << (int)r.right[i].type << " " << r.right[i].value << std::endl;
    //         }
    //     }
    // }

    // print output for debug
    // for(const auto& it : metaRules) {
    //     std::cout << "METARULE " << it.first << ": ";
    //     for(const auto& t : it.second) {
    //         std::cout << t << ", ";
    //     }
    //     std::cout << std::endl;
    // }

    auto controlCodes = utf8ToCodePoints("?#^*");

    for(const auto& it : rules) {
        for(const auto& rule : it.second) {
            auto codes = utf8ToCodePoints(rule.text);
            ruleTextCodePoints.insert(codes.begin(), codes.end());
            for(const auto& left : rule.left) {
                if(left.type == RuleInstructionType::Unused) {
                    auto codes = utf8ToCodePoints(left.value);
                    ruleTextCodePoints.insert(codes.begin(), codes.end());
                }
            }
            for(const auto& right : rule.right) {
                if(right.type == RuleInstructionType::Unused) {
                    auto codes = utf8ToCodePoints(right.value);
                    ruleTextCodePoints.insert(codes.begin(), codes.end());
                }
            }
        }
    }

    for(const auto& it : metaRules) {
        for(const auto& t : it.second) {
            auto codes = utf8ToCodePoints(t);
            ruleTextCodePoints.insert(codes.begin(), codes.end());
        }
    }

    // remove rule control codes from codeset
    for(auto code : controlCodes) {
        ruleTextCodePoints.erase(code);
    }
}


std::string Rules::transcribe(const std::string& text) const {

    std::string result;

    char *start = (char*)text.c_str();  // utf-8 string
    char *it = start;                   // string iterator
    char *end = start + text.size();    // end iterator
    char *cur;

    for(; it < end; ) {
        cur = it;
        uint32_t code = utf8::next(it, end);  // get 32 bit code of a utf-8 symbol
        // if(code == 0)
        //     continue;
        if(this->rules.count(code) == 0) {
            // no rules
            throw std::out_of_range("no rules");
            // continue;
        }
        const auto rules = this->rules.at(code);
        for(const auto& rule : rules) {
            if(test_rule(rule, start, cur, end)) {
                it = cur + rule.text.size();
                if(result.empty() || (!rule.replacement.empty() && rule.replacement[0] == '#')) {
                    result += rule.replacement;
                } else {
                    result += "_" + rule.replacement;
                }
                break;
            }
        }
    }

    return result;
}


bool Rules::test_rule(const Rule& rule, char *start, char *cur, char *end) const {
    if(rule.text.size() > end - cur)
        return false;
    if(std::strncmp(cur, rule.text.c_str(), rule.text.size()) != 0)
        return false;
    char *after = cur + rule.text.size();
    for(const auto& right : rule.right) {
        if(right.type == RuleInstructionType::Unused) {
            if(after + right.value.size() > end || std::strncmp(after, right.value.c_str(), right.value.size()) != 0)
                return false;
            after += right.value.size();
            continue;
        }
        if(right.type != RuleInstructionType::Meta)
            return false;
        if(right.value == "?") {
            // one char is allowed by metarule
            if(after >= end)
                return false;
            utf8::next(after, end); // point to next char
        } else if(right.value == "#") {
            // no chars can be left in source text
            if(after < end)
                return false;
            break;
        } else if(right.value == "^") {
            // there must be one or more chars to the right of the position:
            if(after >= end)
                return false;
            break;
        } else if(right.value == "*") {
            // there can be any amount of chars to the right, no more rules are processed
            break;
        } else {
            bool match = false;
            for(const auto& t : metaRules.at(right.value)) {
                if(after + t.size() > end)
                    continue;
                if(std::strncmp(after, t.c_str(), t.size()) == 0) {
                    after += t.size();
                    match = true;
                    break;
                }
            }
            if(!match)
                return false;
        }
    }
    char *before = cur;
    for(const auto& left : rule.left) {
        if(left.type == RuleInstructionType::Unused) {
            if(before - left.value.size() < start || std::strncmp(before - left.value.size(), left.value.c_str(), left.value.size()) != 0)
                return false;
            before -= left.value.size();
            continue;
        }
        if(left.type != RuleInstructionType::Meta)
            return false;
        if(left.value == "?") {
            // one char is allowed by metarule
            if(before <= start)
                return false;
            utf8::prior(before, start); // point to previous char
        } else if(left.value == "#") {
            // no chars can be left in source text
            if(before > start)
                return false;
            break;
        } else if(left.value == "^") {
            // there must be one or more chars to the left of the position:
            if(before <= start)
                return false;
            break;
        } else if(left.value == "*") {
            // there can be any amount of chars to the left, no more rules are processed
            break;
        } else {
            bool match = false;
            for(const auto& t : metaRules.at(left.value)) {
                if(before - t.size() < start)
                    continue;
                if(std::strncmp(before - t.size(), t.c_str(), t.size()) == 0) {
                    before += t.size();
                    match = true;
                    break;
                }
            }
            if(!match)
                return false;
        }
    }
    return true;
}

#pragma once

#include <vector>
#include <map>
#include <set>

#include "data.h"


class Rules {
public:
    Rules();

    std::string transcribe(const std::string& text) const;
    const std::set<uint32_t>& RuleCodePoints() const { return ruleTextCodePoints; }

private:
    bool test_rule(const Rule& rule, char *start, char *cur, char *end) const;

    std::map<uint32_t, std::vector<Rule>> rules;
    std::map<std::string, std::vector<std::string>> metaRules;
    std::set<uint32_t> ruleTextCodePoints;
};

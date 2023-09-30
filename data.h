#pragma once

#include <vector>
#include <map>
#include <string>
#include <initializer_list>
#include <cstring>



enum class RuleInstructionType {
    Unused,
    Meta,
};


struct RuleInstruction {
    RuleInstructionType type;
    std::string value;

    RuleInstruction(RuleInstructionType type, std::string value) : type(type), value(value) {}
};


class RuleInstructions : public std::vector<RuleInstruction> {
public:
    RuleInstructions() : std::vector<RuleInstruction>() {}
    RuleInstructions(std::initializer_list<RuleInstruction> list) : std::vector<RuleInstruction>(list) {}

    RuleInstructions(const char *init_list) {
        size_t size = std::strlen(init_list);
        std::string tag, text;
        int state = 0;
        for(int i=0; i < size; i++) {
            auto c = init_list[i];
            if(c == ' ') {
                if(state == 2 && tag.size() > 0 && text.size() > 0) {
                    RuleInstructionType t;
                    if(tag == "U") {
                        t = RuleInstructionType::Unused;
                    } else if(tag == "M") {
                        t = RuleInstructionType::Meta;
                    }
                    this->emplace_back(t, text);
                    tag.clear();
                    text.clear();
                    state = 0;
                }
            } else if(state == 1 && c == ':') {
                state = 2;
            } else if(state == 0) {
                if(tag.size() == 0) {
                    state = 1;
                    tag += c;
                }
            } else if(state == 1) {
                tag += c;
            } else if(state == 2) {
                text += c;
            }
        }
        if(state == 2 && tag.size() > 0 && text.size() > 0) {
            RuleInstructionType t;
            if(tag == "U") {
                t = RuleInstructionType::Unused;
            } else if(tag == "M") {
                t = RuleInstructionType::Meta;
            }
            this->emplace_back(t, text);
        }
    }
};


typedef struct {
    RuleInstructions left;
    std::string text;
    RuleInstructions right;
    std::string replacement;
} Rule;


typedef struct {
    std::string name;
    std::vector<std::string> texts;
} Metarule;


extern const std::vector<Rule>& getRules();
extern const std::map<std::string, std::vector<std::string>>& getMetaRules();
extern const std::map<std::string, std::string>& getExceptions();
extern const std::map<std::string, std::string>& getCharsetMap(const std::string& name);

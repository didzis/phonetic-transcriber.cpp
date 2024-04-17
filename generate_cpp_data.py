#!/usr/bin/env python3

import os, json, sys


class jsdict(dict):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
    def __getattr__(self, name):
        return self.get(name)
    def __setattr__(self, name, value):
        self[name] = value
    def __delattr__(self, name):
        try:
            del self[name]
        except KeyError:
            pass

basedir = os.path.dirname(__file__)


def escape(s):
    s = s.replace('\\', '\\\\')
    s = s.replace('"', '\\"')
    return s

def escape_all(*s):
    return tuple(escape(x) for x in s)


def write_header(output):
    if os.path.exists(output):
        print(f'header {output} exists, skipping')
        return
    print(f'writing {output}')
    with open(output, 'w') as of:
        print('''#pragma once

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
extern const std::map<std::string, std::string>& getCharsetMap(const std::string& name);''', file=of)


def convert_rules(input, output, header='data.h'):
    if not os.path.isfile(input):
        print(f'file {input} not found')
        return
    print(f'reading {input}')
    with open(input, 'r') as f:
        data = json.load(f, object_hook=jsdict)
    print(f'writing {output}')
    with open(output, 'w') as of:
        print(f'''#include "{header}"

// const RuleInstructionType M = RuleInstructionType::Meta;
// const RuleInstructionType U = RuleInstructionType::Unused;
''', file=of)
        # output metarule list
        print('''
const std::map<std::string, std::vector<std::string>>& getMetaRules() {
\tstatic std::map<std::string, std::vector<std::string>> metaRules = {''', file=of)
        for name, values in data.metarules.items():
            # print('\t{"%s", {%s}},' % (name, ', '.join('"%s"' % v for v in values)), file=of)
            values = ',\n\t\t\t\t'.join(', '.join('"%s"' % v for v in vs) for vs in (values[start:start+10] for start in range(0, len(values), 10)))
            print('\t\t{"%s", {%s}},' % (name, values), file=of)
        print('''\t};
\treturn metaRules;
}''', file=of)
        # output rule list
        print('''

const std::vector<Rule>& getRules() {
\tstatic std::vector<Rule> rules = {''', file=of)
        for rule in data.rules:
            left = ' '.join('%s:%s' % (r.tag.upper(), r.text) for r in reversed(rule.left))
            right = ' '.join('%s:%s' % (r.tag.upper(), r.text) for r in rule.right)
            print('\t\t{/* left */ % 24s, /* text */ % 14s, /* right */ % 24s, /* -> */ "%s"},' % ('"%s"' % left,
                                                '"%s"' % rule.text, '"%s"' % right, escape(rule.repl)), file=of)
            # left = ', '.join('{%s, "%s"}' % (r.tag.upper(), r.text) for r in reversed(rule.left))
            # right = ', '.join('{%s, "%s"}' % (r.tag.upper(), r.text) for r in rule.right)
            # print('\t\t{/* left */ {%s}, /* text */ "%s", /* right */ {%s}, /* -> */ "%s"},' % (left, rule.text, right, rule.repl.replace('\\', '\\\\')), file=of)
        print('''\t};
\treturn rules;
}''', file=of)


def convert_exceptions(input, output):
    if not os.path.isfile(input):
        print(f'file {input} not found')
        return
    print(f'reading {input}')
    with open(input, 'r') as f:
        data = json.load(f)
    print(f'writing {output}')
    with open(output, 'w') as of:
        print('''
#include <string>
#include <map>

const std::map<std::string, std::string>& getExceptions() {
\tstatic std::map<std::string, std::string> exceptions = {''', file=of)
        for key, value in data.items():
            print('\t\t{"%s", "%s"},' % (key, escape(value)), file=of)
        print('''\t};
\treturn exceptions;
}''', file=of)


def convert_charsets(input, output):
    if not os.path.isfile(input):
        print(f'file {input} not found')
        return
    print(f'reading {input}')
    with open(input, 'r') as f:
        data = json.load(f, object_hook=jsdict)
    print(f'writing {output}')
    with open(output, 'w') as of:
        print('''
#include <string>
#include <map>

const std::map<std::string, std::string>& getCharsetMap(const std::string& name) {
\tstatic std::map<std::string, std::map<std::string, std::string>> data = {''', file=of)
        for name, subdata in data.items():
            for subname, mapping in subdata.items():
                if subname == 'charsets':
                    print('\t\t{"%s_%s", {' % (name, subname), file=of)
                    for i, charset in enumerate(mapping):
                        print('\t\t\t{"charset%i", "%s"},' % (i, charset), file=of)
                    print('\t\t}},', file=of)
                else:
                    N = 8
                    items = list(mapping.items())
                    print('\t\t{"%s_%s", {' % (name, subname), file=of)
                    print('\t\t\t%s,' % ',\n\t\t'.join(', '.join('{"%s", "%s"}' % escape_all(*kv) for kv in items[start:start+N]) for start in range(0, len(items), N)), file=of)
                    print('\t\t}},', file=of)
        print('''\t};
\treturn data[name];
}''', file=of)



if __name__ == '__main__':

    import argparse

    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--rules', '-r', metavar='FILE', type=str, default='rules.json', help='input rules.json')
    parser.add_argument('--exceptdb', '-e', metavar='FILE', type=str, default='exceptions.json', help='input exceptions.json')
    parser.add_argument('--charsets', '-c', metavar='FILE', type=str, default='phonetic_converter_dataset.json', help='input phonetic_converter_dataset.json')
    parser.add_argument('--input', '-i', metavar='FILE', type=str, default='python', help='base path where input data files ar found')
    parser.add_argument('--output', '-o', metavar='FILE', type=str, default='data', help='path where C++ .cpp/.h files to generate')

    args = parser.parse_args()

    if os.path.isdir(args.output):
        args.output = os.path.join(args.output, 'data')

    write_header(f'{args.output}.h')

    if args.rules:
        if not args.rules.startswith('./') and not args.rules.startswith('../') and not args.rules.startswith('/'):
            args.rules = os.path.join(args.input, args.rules)
        convert_rules(args.rules, f'{args.output}_rules.cpp', os.path.basename(f'{args.output}.h'))
    if args.exceptdb:
        if not args.exceptdb.startswith('./') and not args.exceptdb.startswith('../') and not args.exceptdb.startswith('/'):
            args.exceptdb = os.path.join(args.input, args.exceptdb)
        convert_exceptions(args.exceptdb, f'{args.output}_exceptions.cpp')
    if args.charsets:
        if not args.charsets.startswith('./') and not args.charsets.startswith('../') and not args.charsets.startswith('/'):
            args.charsets = os.path.join(args.input, args.charsets)
        convert_charsets(args.charsets, f'{args.output}_charsets.cpp')

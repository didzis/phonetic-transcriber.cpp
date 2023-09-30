#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include "utf8lower.h"
#include "transcriber.h"

void test_alphabetic_character_converter();

std::map<std::string, std::string> loadTSVMap(const std::string& filename)
{
    std::ifstream f(filename);

    std::map<std::string, std::string> m;

    if(!f.is_open())
        return m;

    std::string line;

    while(std::getline(f, line)) {
        if(line.back() == '\n' || line.back() == '\r')
            line.pop_back();
        if(line.back() == '\n' || line.back() == '\r')
            line.pop_back();
        auto p = line.find('\t');
        if(p != std::string::npos) {
            auto key = line.substr(0, p);
            auto val = line.substr(p+1);
            // std::cout << key << " -> " << val << std::endl;
            m[key] = val;
        }
    }

    return m;
}

int main(int argc, char* argv[])
{
    std::vector<std::string> texts;

    bool run_test = false;

    std::string phonemeMapTSV;

    for(int i=1; i<argc; i++) {
        std::string arg(argv[i]);

        if(arg.size() == 0)
            continue;

        if(arg[0] != '-') {
            texts.emplace_back(arg);
            continue;
        }

        if(arg == "-t" ||  arg == "--test") {
            run_test = true;
        }

        if(arg == "-m" ||  arg == "--map") {
            i++;
            if(i < argc && std::string(argv[i]) != "--") {
                phonemeMapTSV = argv[i];
            } else {
                std::cerr << "missing argument to " << argv[i-1] << std::endl;
                return 1;
            }
            continue;
        }

        if(arg == "-h" ||  arg == "--help") {
            std::cout << argv[0] << " [options] [text] [text] ..." << std::endl;
            std::cout << std::endl;
            std::cout << "options:" << std::endl;
            std::cout << "    --help, -h              help" << std::endl;
            std::cout << "    --map FILE, -m FILE     phoneme translation map .tsv file" << std::endl;
            std::cout << "    --test, -t              run test" << std::endl;
            std::cout << std::endl;

            return 0;
        }

        if(arg == "--") {
            texts.insert(texts.end(), &argv[i+1], &argv[argc]);
            break;
        }
    }

    if(run_test)
        test_alphabetic_character_converter();

    if(texts.size() == 0)
        return 0;

    IPACharacterConverter converter;
    Transcriber transcriber(converter);
    // Transcriber transcriber(IPACharacterConverter()); // bad usage, do not do this, the IPACharacterConverter is a temporary value

    std::map<std::string, std::string> phonemeMap;
    if(!phonemeMapTSV.empty())
        phonemeMap = loadTSVMap(phonemeMapTSV);

    for(const auto& text : texts) {
        std::cout << "transcribing: " << text << std::endl;

        std::string result;
        auto chunks = transcriber.transcribe(toLowerCase(text), TranscribeOptions::Full);
        for(const auto& chunk : chunks) {
            std::cout << "chunk (" << TranscribedChunkTypeString[chunk.type] << ") with ";
            if(chunk.text.size() > 0) {
                std::cout << "text: >" << chunk.text << "<";
                result += chunk.text;
            }
            if(chunk.tokens.size() > 0) {
                std::cout << "tokens [" << chunk.tokens.size() << "]:";
                for(auto &token : chunk.tokens)
                    if(phonemeMap.empty()) {
                        std::cout << " " << token;
                        result += token;
                    } else {
                        auto it = phonemeMap.find(token);
                        if(it != phonemeMap.end()) {
                            std::cout << " " << it->second;
                            result += it->second;
                        } else {
                            std::cout << " " << token;
                            result += token;
                        }
                    }
            }
            std::cout << std::endl;
        }
        std::cout << "output: " << result << std::endl;
    }

    return 0;
}

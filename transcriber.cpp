
#include <iostream>
#include <utility>

#include <utf8.h>


#include "transcriber.h"
#include "data.h"
#include "util.h"



std::map<TranscribedChunkType, std::string> TranscribedChunkTypeString = {
    {TranscribedChunkType::PhonemizedTokens, "PhonemizedTokens"},
    {TranscribedChunkType::Whitespace, "Whitespace"},
    {TranscribedChunkType::Paragraph, "Paragraph"},
    {TranscribedChunkType::Unknown, "Unknown"},
};


// https://stackoverflow.com/a/24298250

// inline TranscribeOptions operator |(TranscribeOptions a, TranscribeOptions b) {
//     return static_cast<TranscribeOptions>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
// }
//
// inline TranscribeOptions operator &(TranscribeOptions a, TranscribeOptions b) {
//     return static_cast<TranscribeOptions>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
// }
//
// inline bool operator !(TranscribeOptions a) {
//     return static_cast<unsigned>(a) == 0;
// }


uint32_t reduceWhitespaceCodepoint(uint32_t codepoint) {
    switch(codepoint) {
        case (uint32_t)' ':
        case (uint32_t)'\t':
        case (uint32_t)'\r':
        case (uint32_t)'\f':
        case (uint32_t)'\v':
            // any other unicode whitespace?
            return (uint32_t)' ';
        case (uint32_t)'\n':
            return (uint32_t)'\n';
    }
    return codepoint;
}


Transcriber::Transcriber(const CharacterConverter& encoder) :
    converter(decoder, encoder), exceptions(getExceptions()) {
}


// possible output chunksets:
// a. phonemized array, unknown string with whitepaces
// b. phonemized array, unknown string, whitespace string
// c. phonemized array, unknown string, whitespace string, separate paragraph string
// d. phonemized array, unknown string, collapsed whitespace string, separate paragraph string
std::vector<TranscribedChunk> Transcriber::transcribe(const std::string& text, TranscribeOptions options) const {

    const auto& phonemizableCharset = rules.RuleCodePoints();
    auto textCodePoints = utf8ToCodePoints(text);

    std::string phonemizable;
    auto inserter = std::back_inserter(phonemizable);

    std::vector<TranscribedChunk> chunks;

    TranscribedChunkType type;
    std::string chunk;

    for(auto codepoint : textCodePoints) {
        if(phonemizableCharset.count(codepoint) > 0) {
            if(phonemizable.empty()) {
                if(!chunk.empty()) {
                    chunks.emplace_back(type, chunk);
                    // reset
                    chunk.clear();
                }
                inserter = std::back_inserter(phonemizable);
            }
            utf8::append(codepoint, inserter);
            continue;
        } else if(!phonemizable.empty()) {
            chunks.emplace_back(transcribe(phonemizable));
            // reset
            phonemizable.clear();
            chunk.clear();
        }

        // process non-phonemizable codepoints
        if(!(options & TranscribeOptions::Unknown))
            continue;

        if(chunk.empty()) {
            inserter = std::back_inserter(chunk);
            type = TranscribedChunkType::Unknown;
        }

        // reduce whitespaces
        if(!!(options & TranscribeOptions::ReduceWhitespace))
            codepoint = reduceWhitespaceCodepoint(codepoint);

        if(!!(options & TranscribeOptions::Whitespace) || !!(options & TranscribeOptions::Paragraph)) {
            auto whitespace = !!(options & TranscribeOptions::ReduceWhitespace) ? codepoint : reduceWhitespaceCodepoint(codepoint);

            if(!!(options & TranscribeOptions::Whitespace) && whitespace == (uint32_t)' ' && type != TranscribedChunkType::Whitespace) {
                if(!chunk.empty())
                    chunks.emplace_back(type, chunk);
                chunk.clear();
                type = TranscribedChunkType::Whitespace;
            }

            if(!!(options & TranscribeOptions::Paragraph) && whitespace == (uint32_t)'\n' && type != TranscribedChunkType::Paragraph) {
                if(!chunk.empty())
                    chunks.emplace_back(type, chunk);
                chunk.clear();
                type = TranscribedChunkType::Paragraph;
            }

            if(!(options & TranscribeOptions::Paragraph) && whitespace == (uint32_t)'\n' && type != TranscribedChunkType::Whitespace) {
                if(!chunk.empty())
                    chunks.emplace_back(type, chunk);
                chunk.clear();
                type = TranscribedChunkType::Whitespace;
            }

            if(!!(options & TranscribeOptions::CollapseWhitespace)
                    && whitespace == (uint32_t)' ' && type == TranscribedChunkType::Whitespace && !chunk.empty()) {
                continue;
            }
        }

        utf8::append(codepoint, inserter);
    }
    if(!phonemizable.empty()) {
        chunks.emplace_back(transcribe(phonemizable));
    } else if(!chunk.empty()) {
        chunks.emplace_back(type, chunk);
    }

    return chunks;
}


std::vector<std::string> Transcriber::transcribe(const std::string& word) const {
    std::string result;
    auto eit = exceptions.find(word);
    if(eit != exceptions.end()) {
        result = eit->second;
    } else {
        result = rules.transcribe(word);
    }

    // split string
    const auto& s = result;
    std::string delimiter = "_";
    std::vector<std::string> tokens;
    size_t last = 0, next = 0;
    while((next = s.find(delimiter, last)) != std::string::npos) {
        tokens.emplace_back(s.substr(last, next-last));
        last = next + delimiter.size();
    }
    tokens.emplace_back(s.substr(last));

    tokens = converter.convert(tokens);
    if(tokens.size() == 0)
        return tokens;

    return tokens;
}


std::string Transcriber::transcribe(const std::string& word, const std::string& separator) const {
    std::string result;
    for(const auto& token : transcribe(word)) {
        if(!separator.empty() && !result.empty())
            result.append(separator);
        result.append(token);
    }
    return result;
}


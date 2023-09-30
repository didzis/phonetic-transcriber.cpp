#pragma once

#include <string>
#include <vector>
#include <map>

#include "rules.h"
#include "converter.h"


enum class TranscribedChunkType : unsigned {
    PhonemizedTokens,
    Whitespace,
    Paragraph,
    Unknown,
};

extern std::map<TranscribedChunkType, std::string> TranscribedChunkTypeString;


enum class TranscribeOptions : unsigned {
    None = 0x0,

    Unknown = 0x01,
    Whitespace = 0x02,
    Paragraph = 0x04,
    ReduceWhitespace = 0x08,
    CollapseWhitespace = 0x10,

    Default = 0x01 + 0x02 + 0x04,             // Unknown + Whitespace + Paragraph
    Full = 0x01 + 0x02 + 0x04 + 0x08 + 0x10,  // Unknown + Whitespace + Paragraph + ReduceWhitespace + CollapseWhitespace
};

inline TranscribeOptions operator |(TranscribeOptions a, TranscribeOptions b) {
    return static_cast<TranscribeOptions>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}

inline TranscribeOptions operator &(TranscribeOptions a, TranscribeOptions b) {
    return static_cast<TranscribeOptions>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
}

inline bool operator !(TranscribeOptions a) {
    return static_cast<unsigned>(a) == 0;
}


struct TranscribedChunk {
    TranscribedChunkType type;
    std::string text;
    std::vector<std::string> tokens;

    TranscribedChunk(std::vector<std::string> tokens) : tokens(tokens), type(TranscribedChunkType::PhonemizedTokens) {}
    TranscribedChunk(TranscribedChunkType type, std::string text) : type(type), text(text) {}
};


class Transcriber {
public:
    Transcriber(const CharacterConverter& encoder);
    std::vector<TranscribedChunk> transcribe(const std::string& text, TranscribeOptions options) const;
    std::vector<std::string> transcribe(const std::string& word) const;
    std::string transcribe(const std::string& word, const std::string& separator) const;
private:
    const AlphabeticCharacterConverter decoder;
    PhoneticConverter converter;
    const std::map<std::string, std::string>& exceptions;
    Rules rules;
};

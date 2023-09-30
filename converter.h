#pragma once

#include <vector>
#include <map>



class CharacterConverter {
public:
    virtual std::string toIPAchar(const std::string& str) const = 0;
    virtual std::string fromIPAchar(const std::string& str) const = 0;
    virtual std::vector<std::string> tokenize(const std::string& word) const = 0;
};


class AlphaNumericSimplifiedCharacterConverter : public CharacterConverter {
public:
    AlphaNumericSimplifiedCharacterConverter();
    std::string toIPAchar(const std::string& str) const;
    std::string fromIPAchar(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& word) const;
private:
    std::map<std::string, std::string> fromIPAafter;
    std::map<std::string, std::string> fromIPAbefore;
    std::map<std::string, std::string> fromIPAresult;
};


class AlphaNumericCharacterConverter : public CharacterConverter {
public:
    AlphaNumericCharacterConverter();
    std::string toIPAchar(const std::string& str) const;
    std::string fromIPAchar(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& word) const;
private:
    std::map<std::string, std::string> toIPAafter;
    std::map<std::string, std::string> toIPAbefore;
    std::map<std::string, std::string> toIPAresult;
    std::map<std::string, std::string> fromIPAafter;
    std::map<std::string, std::string> fromIPAbefore;
    std::map<std::string, std::string> fromIPAresult;
};


class AlphabeticCharacterConverter : public CharacterConverter {
public:
    AlphabeticCharacterConverter();
    std::string toIPAchar(const std::string& str) const;
    std::string fromIPAchar(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& word) const;
private:
    std::map<std::string, std::string> toIPAafter;
    std::map<std::string, std::string> toIPAbefore;
    std::map<std::string, std::string> toIPAresult;
    std::map<std::string, std::string> fromIPAafter;
    std::map<std::string, std::string> fromIPAbefore;
    std::map<std::string, std::string> fromIPAresult;
};


class IPASimplifiedCharacterConverter : public CharacterConverter {
public:
    IPASimplifiedCharacterConverter();
    std::string toIPAchar(const std::string& str) const;
    std::string fromIPAchar(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& word) const;
private:
    std::map<std::string, std::string> fromIPAafter;
    std::map<std::string, std::string> fromIPAbefore;
    std::map<std::string, std::string> fromIPAresult;
    std::string charset0;
    std::string charset1;
    std::string charset2;
};


class IPACharacterConverter : public CharacterConverter {
public:
    IPACharacterConverter();
    std::string toIPAchar(const std::string& str) const;
    std::string fromIPAchar(const std::string& str) const;
    std::vector<std::string> tokenize(const std::string& word) const;
private:
    std::string charset0;
    std::string charset1;
    std::string charset2;
};


class PhoneticConverter {
public:
    PhoneticConverter(const CharacterConverter& decoder, const CharacterConverter& encoder) : encoder(encoder), decoder(decoder) {}

    std::string convert(const std::string c) const {
        return encoder.fromIPAchar(decoder.toIPAchar(c));
    }

    std::vector<std::string> convert(const std::vector<std::string> tokens) const {
        std::vector<std::string> output;
        output.reserve(tokens.size());
        for(const auto& token : tokens) {
            output.emplace_back(convert(token));
        }
        return output;
    }

    std::string convert(const std::string word, const std::string separator) const {
        std::string result;
        for(const auto& token : convert(decoder.tokenize(word))) {
            if(!result.empty() && !separator.empty())
                result.append(separator);
            result.append(token);
        }
        return result;
    }

private:
    const CharacterConverter& encoder;
    const CharacterConverter& decoder;
};


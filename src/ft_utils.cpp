#include <algorithm>
#include "../include/ft_utils.hpp"

std::string FtUtils::joinVec(const std::vector<std::string>& vec) {
    std::string buffer{};
    for(auto it = vec.begin(); it != vec.end(); ++it) {
        if (it != vec.begin()) buffer += ' ';
        buffer += *it;
    }
    return buffer;
}

std::string FtUtils::getStringFromQuotes(std::istringstream& stream) {
    if (stream.peek() == ' ') stream.ignore(1);
    char chr;
    std::string buffer{};
    while (stream.get(chr)) {
        if (chr == '"') break;
        buffer += chr;
    }
    return buffer;
}

FtError FtUtils::getNumber(const std::string& word, cell& value) {
    try {
        std::size_t pos{};
        cell number = std::stoll(word, &pos);
        if (pos != word.size()) 
            return FtError{ ErrorCode::BadNumber, word };
        value = number;
    } catch(std::invalid_argument&) { return FtError{ ErrorCode::UnknownWord, word }; }
    return FtError{ ErrorCode::Ok };
}

bool FtUtils::isDigit(const std::string& word) {
    return std::all_of(word.begin(), word.end(), ::isdigit);
}

std::string FtUtils::getError(FtError error) {
    switch (error.error_code) {
        case ErrorCode::Ok: return "ok"; 
        case ErrorCode::BadNumber: return "bad number format: " + error.descr; 
        case ErrorCode::UnknownWord: return "unknown word: " + error.descr; 
        case ErrorCode::DivisionByZero: return "division by zero"; 
        case ErrorCode::EmptyStack: return "the stack is empty"; 
        case ErrorCode::StackOutOfMemory: return "stack out of memory"; 
        case ErrorCode::ParametersExpected: return "expected more parameters";  
        case ErrorCode::WordNameNotSet: return "word name not set";
        case ErrorCode::AlreadyInCompilingMode: return "already in compiling mode";
        case ErrorCode::NotInCompilingMode: return "not in compiling mode";
        case ErrorCode::MemoryOverflow: return "memory is overflow";
        case ErrorCode::BadAddress: return "bad memory address: " + error.descr;
        default: return "undefined error";
    }
}
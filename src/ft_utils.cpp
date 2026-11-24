#include <algorithm>
#include "../include/ft_utils.hpp"

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
            return FtError{ FtErrorCode::BadNumber, word };
        value = number;
    } catch(std::out_of_range&) { return FtError{ FtErrorCode::NumberTooBig, word }; }
    return FtError{ FtErrorCode::Ok };
}

bool FtUtils::isNumber(const std::string& word) {
    return std::all_of(word.begin(), word.end(), [](char ch) { return std::isdigit(static_cast<unsigned char>(ch)); });
}

std::string FtUtils::getError(FtError error) {
    switch (error.error_code) {
        case FtErrorCode::Ok: return "ok"; 
        case FtErrorCode::BadNumber: return "bad number format: " + error.descr; 
        case FtErrorCode::NumberTooBig: return "number too big: " + error.descr;
        case FtErrorCode::UnknownWord: return "unknown word: " + error.descr; 
        case FtErrorCode::DivisionByZero: return "division by zero"; 
        case FtErrorCode::EmptyStack: return "the stack is empty"; 
        case FtErrorCode::StackOutOfMemory: return "stack out of memory"; 
        case FtErrorCode::WordNameNotSet: return "empty string for word name";
        case FtErrorCode::AlreadyInCompilingMode: return "already in compiling mode";
        case FtErrorCode::NotInCompilingMode: return "not in compiling mode";
        case FtErrorCode::NotInLoop: return "not in loop";
        case FtErrorCode::MemoryOverflow: return "memory is overflow";
        case FtErrorCode::BadAddress: return "bad memory address: " + error.descr;
        case FtErrorCode::UnterminatedIf: return "unterminated if";
        case FtErrorCode::UnterminatedLoop: return "unterminated loop";
        case FtErrorCode::UnexpectedThen: return "unexpected \"then\"";
        case FtErrorCode::UnexpectedWhile: return "unexpected \"while\"";
        case FtErrorCode::UnexpectedRepeat: return "unexpected \"repeat\"";
        case FtErrorCode::UnexpectedUntil: return "unexpected \"until\"";
        case FtErrorCode::UnexpectedLoop: return "unexpected \"loop\"";
        case FtErrorCode::WordNotSupportedInCompileMode: return "word not supported in compile mode";
        case FtErrorCode::NotImplemented: return "word not implemented";
        case FtErrorCode::CannotRedefineBuiltinWord: return "cannot redefine built-in word: " + error.descr;
        default: return "undefined error";
    }
}
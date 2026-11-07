#include "../include/interpreter.hpp"

std::string Interpreter::getStringFromQuotes(std::istringstream& stream) const {
    if (stream.peek() == ' ') stream.ignore(1);
    char chr;
    std::string buffer{};
    while (stream.get(chr)) {
        if (chr == '"') break;
        buffer += chr;
    }
    return buffer;
}

ErrorCode Interpreter::getNumber(const std::string& word, cell& value) {
    try {
        std::size_t pos{};
        long long number = std::stoll(word, &pos);
        if (pos != word.size()) 
            return ErrorCode::BadNumber;
        value = number;
    } catch(std::invalid_argument&) { return ErrorCode::UnknownWord; }
    return ErrorCode::Ok;
}

ExecutionToken* Interpreter::isWordBuiltIn(const std::string& word) {
    const auto element = std::find_if(words_.begin(), words_.end(), [&](ExecutionToken& token) {
        return token.name == word;
    });
    return (element != words_.end()) ? &(*element) : nullptr;
}

bool Interpreter::isDigit(const std::string& word) const {
    return std::all_of(word.begin(), word.end(), ::isdigit);
}

std::string Interpreter::getError(ErrorCode error_code) const {
    switch (error_code) {
        case ErrorCode::Ok: return "ok"; 
        case ErrorCode::BadNumber: return "bad number format"; 
        case ErrorCode::UnknownWord: return "unknown word"; 
        case ErrorCode::DivisionByZero: return "division by zero"; 
        case ErrorCode::EmptyStack: return "the stack is empty"; 
        case ErrorCode::StackOutOfMemory: return "stack out of memory"; 
        case ErrorCode::ParametersExpected: return "expected more parameters";  
        case ErrorCode::WordNameNotSet: return "word name not set";
        case ErrorCode::AlreadyInCompilingMode: return "already in compiling mode";
        case ErrorCode::NotInCompilingMode: return "not in compiling mode";
        case ErrorCode::MemoryOverflow: return "memory is overflow";
        case ErrorCode::BadAddress: return "bad memory address";
        default: return "undefined error";
    }
}
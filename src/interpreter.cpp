#include "../include/interpreter.hpp"

void Interpreter::registerWords() {
    words_.emplace_back("drop", [this]() -> ErrorCode { return forth_drop(); } );
    words_.emplace_back("dup", [this]() -> ErrorCode { return forth_dup(); } );
    words_.emplace_back("swap", [this]() -> ErrorCode { return forth_swap(); } );
    words_.emplace_back("rot", [this]() -> ErrorCode { return forth_rot(); } );
    words_.emplace_back("over", [this]() -> ErrorCode { return forth_over(); } );

    words_.emplace_back("if", [this](std::istringstream& stream) -> ErrorCode { return forth_if(stream); } ); // IMPL!!!

    words_.emplace_back("+", [this]() -> ErrorCode { return forth_plus(); } );
    words_.emplace_back("-", [this]() -> ErrorCode { return forth_minus(); } );
    words_.emplace_back("*", [this]() -> ErrorCode { return forth_multiply(); } );
    words_.emplace_back("/", [this]() -> ErrorCode { return forth_divide(); } );
    words_.emplace_back("mod", [this]() -> ErrorCode { return forth_mod(); } );

    words_.emplace_back("<", [this]() -> ErrorCode { return forth_less(); } );
    words_.emplace_back(">", [this]() -> ErrorCode { return forth_greater(); } );
    words_.emplace_back("=", [this]() -> ErrorCode { return forth_equal(); } );
    words_.emplace_back("negate", [this]() -> ErrorCode { return forth_negate(); } );
    words_.emplace_back("invert", [this]() -> ErrorCode { return forth_invert(); } );
    words_.emplace_back("and", [this]() -> ErrorCode { return forth_and(); } );
    words_.emplace_back("or", [this]() -> ErrorCode { return forth_or(); } );

    words_.emplace_back(".", [this]() -> ErrorCode { return forth_dot(); } );
    words_.emplace_back("emit", [this]() -> ErrorCode { return forth_emit(); } );
    words_.emplace_back("cr", [this]() -> ErrorCode { return forth_cr(); } );
    words_.emplace_back(".\"", [this](std::istringstream& stream) -> ErrorCode { return forth_dotquote(stream); } );
    
    words_.emplace_back("variable", [this](std::istringstream& stream) -> ErrorCode { return forth_variable(stream); } );
    words_.emplace_back("allot", [this]() -> ErrorCode { RETURN_IF_ERROR(memory_.allot()); return ErrorCode::Ok; } );
    words_.emplace_back("here", [this]() -> ErrorCode { return forth_here(); } );
    words_.emplace_back("cell", [this]() -> ErrorCode { return forth_cell(); } );
    words_.emplace_back("cell+", [this]() -> ErrorCode { return forth_cell_plus(); } );
    words_.emplace_back("@", [this]() -> ErrorCode { RETURN_IF_ERROR(memory_.fetch()); return ErrorCode::Ok; } );
    words_.emplace_back("!", [this]() -> ErrorCode { RETURN_IF_ERROR(memory_.store()); return ErrorCode::Ok; } );

    words_.emplace_back(":", [this](std::istringstream& stream) -> ErrorCode { return forth_colon(stream); } );
    words_.emplace_back(";", [this](std::istringstream& stream) -> ErrorCode { return forth_semicolon(); } );
}

ErrorCode Interpreter::interpretWord(const std::string& code) {
    if (code.empty()) return ErrorCode::Ok;
    std::string buffer{};
    std::istringstream stream(code);
    while (stream >> buffer) {
        if (ExecutionToken* token = isWordBuiltIn(buffer)) {
            if (is_compiling_ && !token->is_immediate)
                current_token_.data.push_back(*token);
            else if (token->is_immediate) 
                RETURN_IF_ERROR(token->immediate(stream));
            else 
                RETURN_IF_ERROR(token->code());
            continue;
        }
        cell value = 0;
        RETURN_IF_ERROR(getNumber(buffer, value));
        if (is_compiling_) {
            current_token_.data.emplace_back("nlit", [this, value]() -> ErrorCode {
                RETURN_IF_ERROR(memory_.pushv(value));
                return ErrorCode::Ok;
            } );
        } else RETURN_IF_ERROR(memory_.pushv(value));
    }
    return ErrorCode::Ok;
}

void Interpreter::run() {
    registerWords();
    std::string line{};
    std::cout << ">> ";
    while (std::getline(std::cin, line) && line != "exit") {
        if (line.empty()) continue;
        ErrorCode err = interpretWord(line);
        if (!is_compiling_ || err != ErrorCode::Ok)
            std::cout << getError(err) << "\n";
        std::cout << ">> ";
    }
}
#include <iostream>
#include "../include/ft_interpreter.hpp"
#include "../include/ft_utils.hpp"

void FtInterpreter::run() {
    initDictionary();
    std::string buffer{};
    std::cout << ">> ";
    while (std::getline(std::cin, buffer) && buffer != "bye") {
        if (buffer.empty()) continue;
        FtError err = evaluate(buffer);
        if (compile_ctx_.is_compiling) {
            if (err.error_code != FtErrorCode::Ok) {
                abortCompile();
                runtime_ctx_.output_buffer.writeStringToBuffer(FtUtils::getError(err));
                std::cout << runtime_ctx_.output_buffer.take() << "\n";
            }
        } else {
            runtime_ctx_.output_buffer.writeStringToBuffer(FtUtils::getError(err));
            std::cout << runtime_ctx_.output_buffer.take() << "\n";
        }
        std::cout << ">> ";
    }
}

FtError FtInterpreter::evaluate(const std::string& code) {
    std::string buffer{};
    std::istringstream stream(code);
    while (stream >> buffer) {
        const int word_index = dictionary_.findWord(buffer);
        if (word_index != -1) {
            FtWord* word = dictionary_.getWordAt(word_index);
            if (compile_ctx_.is_compiling && !word->is_immediate)
                dictionary_.compileCall(compile_ctx_, word);
            else if (word->is_immediate)
                RETURN_IF_ERROR((word->immediate != nullptr) ? word->immediate(stream) : word->code());
            else
                RETURN_IF_ERROR(word->code());
            continue;
        }
        if (FtUtils::isNumber(buffer)) {
            cell value = 0;
            RETURN_IF_ERROR(FtUtils::getNumber(buffer, value));
            if (compile_ctx_.is_compiling) 
                dictionary_.compileCall(compile_ctx_, 
                    [this, value](std::size_t&) -> FtError {
                    RETURN_IF_ERROR(runtime_ctx_.stack.pushv(value));
                    return FtError{ FtErrorCode::Ok };
                });
            else RETURN_IF_ERROR(runtime_ctx_.stack.pushv(value));
            continue;
        }
        return FtError{ FtErrorCode::UnknownWord, buffer };
    }
    return FtError{ FtErrorCode::Ok };
}

void FtInterpreter::initDictionary() {
    // Built-in variables
    dictionary_.addWord("I", [this]() -> FtError { 
        if (compile_ctx_.loop_frames.empty()) 
            return FtError{ FtErrorCode::EmptyStack };
        return runtime_ctx_.stack.pushv(compile_ctx_.loop_frames.back().index);
    } );

    // Stack methods
    dictionary_.addWord("drop", [this]() -> FtError { return forth_drop(); } );
    dictionary_.addWord("dup", [this]() -> FtError { return forth_dup(); } );
    dictionary_.addWord("swap", [this]() -> FtError { return forth_swap(); } );
    dictionary_.addWord("rot", [this]() -> FtError { return forth_rot(); } );
    dictionary_.addWord("over", [this]() -> FtError { return forth_over(); } );

    dictionary_.addWord("if", [this](std::istringstream& stream) -> FtError { return forth_if(stream); } );
    dictionary_.addWord("else", [this](std::istringstream& stream) -> FtError { return forth_else(stream); } );
    dictionary_.addWord("then", [this](std::istringstream& stream) -> FtError { return forth_then(stream); } );

    dictionary_.addWord("do", [this](std::istringstream& stream) -> FtError { return forth_do(stream); } );
    dictionary_.addWord("loop", [this](std::istringstream& stream) -> FtError { return forth_loop(stream); } );
    dictionary_.addWord("begin", [this](std::istringstream& stream) -> FtError { return forth_begin(stream); } );
    dictionary_.addWord("until", [this](std::istringstream& stream) -> FtError { return forth_until(stream); } );
    dictionary_.addWord("while", [this](std::istringstream& stream) -> FtError { return forth_while(stream); } );
    dictionary_.addWord("repeat", [this](std::istringstream& stream) -> FtError { return forth_repeat(stream); } );
    dictionary_.addWord("leave", [this]() -> FtError { return forth_leave(); }, true );

    dictionary_.addWord("+", [this]() -> FtError { return forth_plus(); } );
    dictionary_.addWord("-", [this]() -> FtError { return forth_minus(); } );
    dictionary_.addWord("*", [this]() -> FtError { return forth_multiply(); } );
    dictionary_.addWord("/", [this]() -> FtError { return forth_divide(); } );
    dictionary_.addWord("mod", [this]() -> FtError { return forth_mod(); } );

    dictionary_.addWord("<", [this]() -> FtError { return forth_less(); } );
    dictionary_.addWord(">", [this]() -> FtError { return forth_greater(); } );
    dictionary_.addWord("=", [this]() -> FtError { return forth_equal(); } );
    dictionary_.addWord("negate", [this]() -> FtError { return forth_negate(); } );
    dictionary_.addWord("invert", [this]() -> FtError { return forth_invert(); } );
    dictionary_.addWord("and", [this]() -> FtError { return forth_and(); } );
    dictionary_.addWord("or", [this]() -> FtError { return forth_or(); } );

    dictionary_.addWord(".", [this]() -> FtError { return forth_dot(); } );
    dictionary_.addWord("emit", [this]() -> FtError { return forth_emit(); } );
    dictionary_.addWord("cr", [this]() -> FtError { return forth_cr(); } );
    dictionary_.addWord(".\"", [this](std::istringstream& stream) -> FtError { return forth_dotquote(stream); } );
    
    dictionary_.addWord("variable", [this](std::istringstream& stream) -> FtError { return forth_variable(stream); } );
    // dictionary_.addWord("allot", [this]() -> FtError { RETURN_IF_ERROR(memory_.allot()); return FtErrorCode::Ok; } );
    dictionary_.addWord("here", [this]() -> FtError { return forth_here(); } );  
    dictionary_.addWord("cell", [this]() -> FtError { return forth_cell(); } );
    dictionary_.addWord("cell+", [this]() -> FtError { return forth_cell_plus(); } );
    dictionary_.addWord("@", [this]() -> FtError { return forth_fetch(); } );
    dictionary_.addWord("!", [this]() -> FtError { return forth_store(); } );

    dictionary_.addWord(":", [this](std::istringstream& stream) -> FtError { return beginDefinition(stream); } );
    dictionary_.addWord(";", [this]() -> FtError { return endDefinition(); }, true );
}

void FtInterpreter::createEmptyWord() {
    compile_ctx_.last_definition_at = dictionary_.size();
    dictionary_.createEmptyWord();
}

void FtInterpreter::abortCompile() {
    if (compile_ctx_.last_definition_at == dictionary_.size() - 1) dictionary_.pop();
    else dictionary_.erase(compile_ctx_.last_definition_at);
    runtime_ctx_.reset();
    compile_ctx_.reset();
}

void FtInterpreter::setLastDefinitionIndex(int index) {
    if (index < 0 || index > dictionary_.size())
        return;
    compile_ctx_.last_definition_at = index;
}

FtWord* FtInterpreter::getLastDefinedWord() { return dictionary_.getWordAt(compile_ctx_.last_definition_at); }
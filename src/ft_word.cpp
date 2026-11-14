#include "../include/ft_word.hpp"

void FtWordDictionary::initDictionary() {
    compileNewWord("drop", [this]() -> FtError { return forth_drop(); } );
    compileNewWord("dup", [this]() -> FtError { return forth_dup(); } );
    compileNewWord("swap", [this]() -> FtError { return forth_swap(); } );
    compileNewWord("rot", [this]() -> FtError { return forth_rot(); } );
    compileNewWord("over", [this]() -> FtError { return forth_over(); } );

    compileNewWord("if", [this](std::istringstream& stream) -> FtError { return forth_if(stream); } );

    compileNewWord("+", [this]() -> FtError { return forth_plus(); } );
    compileNewWord("-", [this]() -> FtError { return forth_minus(); } );
    compileNewWord("*", [this]() -> FtError { return forth_multiply(); } );
    compileNewWord("/", [this]() -> FtError { return forth_divide(); } );
    compileNewWord("mod", [this]() -> FtError { return forth_mod(); } );

    compileNewWord("<", [this]() -> FtError { return forth_less(); } );
    compileNewWord(">", [this]() -> FtError { return forth_greater(); } );
    compileNewWord("=", [this]() -> FtError { return forth_equal(); } );
    compileNewWord("negate", [this]() -> FtError { return forth_negate(); } );
    compileNewWord("invert", [this]() -> FtError { return forth_invert(); } );
    compileNewWord("and", [this]() -> FtError { return forth_and(); } );
    compileNewWord("or", [this]() -> FtError { return forth_or(); } );

    compileNewWord(".", [this]() -> FtError { return forth_dot(); } );
    compileNewWord("emit", [this]() -> FtError { return forth_emit(); } );
    compileNewWord("cr", [this]() -> FtError { return forth_cr(); } );
    compileNewWord(".\"", [this](std::istringstream& stream) -> FtError { return forth_dotquote(stream); } );
    
    compileNewWord("variable", [this](std::istringstream& stream) -> FtError { return forth_variable(stream); } );
    // compileNewWord("allot", [this]() -> FtError { RETURN_IF_ERROR(memory_.allot()); return ErrorCode::Ok; } );
    compileNewWord("here", [this]() -> FtError { return forth_here(); } );  
    compileNewWord("cell", [this]() -> FtError { return forth_cell(); } );
    compileNewWord("cell+", [this]() -> FtError { return forth_cell_plus(); } );
    compileNewWord("@", [this]() -> FtError { return forth_fetch(); } );
    compileNewWord("!", [this]() -> FtError { return forth_store(); } );

    compileNewWord(":", [this](std::istringstream& stream) -> FtError { return forth_colon(stream); } );
    compileNewWord(";", [this]() -> FtError { return forth_semicolon(); }, true );
}

void FtWordDictionary::createEmptyWord() {
    last_definition_at_ = words_.size();
    words_.emplace_back(std::make_unique<FtWord>());
    words_.back()->is_hidden = true;
}

void FtWordDictionary::deleteLastDefinition() {
    if (last_definition_at_ == -1)
        return;
    std::rotate(words_.begin(), words_.begin() + last_definition_at_, words_.end());
    words_.pop_back();
}

void FtWordDictionary::compileNewWord(const std::string& name, Func func, bool is_immediate) {
    words_.emplace_back(std::make_unique<FtWord>(name, func, is_immediate));
}

void FtWordDictionary::compileNewWord(const std::string& name, ImmediateFunc imm_func, bool is_immediate) {
    words_.emplace_back(std::make_unique<FtWord>(name, imm_func, is_immediate));
}

int FtWordDictionary::findWord(const std::string& word) {
    const auto index = std::find_if(words_.begin(), words_.end(), [&](const std::unique_ptr<FtWord>& token) {
        return token->name == word && !token->is_hidden;
    } );
    return (index != words_.end()) ? std::distance(words_.begin(), index) : -1;
}

FtError FtWordDictionary::evaluate(const std::string& code) {
    std::string buffer{};
    std::istringstream stream(code);
    while (stream >> buffer) {
        const int word_index = findWord(buffer);
        if (word_index >= 0) {
            FtWord* token = getWordAt(word_index);
            if (FtStateManager::getState() == FtState::Compiling && !token->is_immediate)
                getLastDefinition()->data.emplace_back(token->code);
            else if (token->is_immediate) 
                RETURN_IF_ERROR(((token->immediate != nullptr) ? token->immediate(stream) : token->code()));
            else 
                RETURN_IF_ERROR(token->code());
            continue;
        }
        cell value = 0;
        RETURN_IF_ERROR(FtUtils::getNumber(buffer, value));
        if (FtStateManager::getState() == FtState::Compiling) {
            getLastDefinition()->data.emplace_back(
                [this, value]() -> FtError {
                    RETURN_IF_ERROR(stack_.pushv(value));
                    return FtError{ ErrorCode::Ok };
            } );
        } else RETURN_IF_ERROR(stack_.pushv(value));
    }
    return FtError{ ErrorCode::Ok };
}

FtWord* FtWordDictionary::getWordAt(int index) { return words_.at(index).get(); }
FtWord* FtWordDictionary::getLastDefinition() { return words_.at(last_definition_at_).get(); }
FtMemory& FtWordDictionary::getMemory() { return memory_; }
const FtMemory& FtWordDictionary::getMemory() const { return memory_; }
FtStack& FtWordDictionary::getStack() { return stack_; }
const FtStack& FtWordDictionary::getStack() const { return stack_; }
FtOutputBuffer& FtWordDictionary::getOutputBuffer() { return output_buffer_; }
const FtOutputBuffer& FtWordDictionary::getOutputBuffer() const { return output_buffer_; }
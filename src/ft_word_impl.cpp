#include "../include/ft_word.hpp"
#include "../include/ft_utils.hpp"
#include "../include/ft_state.hpp"

/* STACK METHODS */
FtError FtWordDictionary::forth_drop() { 
    RETURN_IF_ERROR(stack_.pop());
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_dup() { 
    if (stack_.isStackEmpty()) return FtError{ ErrorCode::EmptyStack };
    cell val = 0; RETURN_IF_ERROR(stack_.topv(val));
    RETURN_IF_ERROR(stack_.pushv(val)); 
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_swap() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val2)); RETURN_IF_ERROR(stack_.pushv(val1));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_rot() {
    cell val1 = 0, val2 = 0, val3 = 0; RETURN_IF_ERROR(stack_.pop3v(val1, val2, val3));
    RETURN_IF_ERROR(stack_.pushv(val2)); RETURN_IF_ERROR(stack_.pushv(val3)); RETURN_IF_ERROR(stack_.pushv(val1));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_over() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    RETURN_IF_ERROR(stack_.topv(val2)); RETURN_IF_ERROR(stack_.pushv(val1)); RETURN_IF_ERROR(stack_.pushv(val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_if(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ ErrorCode::NotInCompilingMode };
    int depth = 0;
    bool is_in_else_block = false;
    std::string buffer{};
    std::vector<std::string> true_branch; 
    std::vector<std::string> false_branch; 
    while (stream >> buffer) {
        if (buffer == "if") {
            if (!is_in_else_block)
                --depth;
            is_in_else_block = false;
            continue;
        } else if (buffer == "else") {
            if (is_in_else_block)
                ++depth;
            is_in_else_block = true;
            continue;
        } else if (buffer == "then") {
            if (depth == 0)
                break;
            continue;
        }
        (!is_in_else_block ? true_branch : false_branch).push_back(buffer);
    }
    FtWord* word = getLastDefinition();
    word->data.emplace_back([this, true_branch = std::move(true_branch), false_branch = std::move(false_branch)]() {
        cell condition = 0; RETURN_IF_ERROR(stack_.popv(condition));
        std::string code = (CONVERT_FORTH_TO_BOOL(condition)) ? 
            code = FtUtils::joinVec(true_branch) : code = FtUtils::joinVec(false_branch);
            RETURN_IF_ERROR(evaluate(code));
        return FtError{ ErrorCode::Ok };
    });
    return FtError{ ErrorCode::Ok };
}

/* MATH OPERATIONS */
FtError FtWordDictionary::forth_plus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val1 + val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_minus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val1 - val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_multiply() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val1 * val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_divide() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    if (val2 == 0) return FtError{ ErrorCode::DivisionByZero };
    RETURN_IF_ERROR(stack_.pushv(val1 / val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_mod() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    if (val2 == 0) return FtError{ ErrorCode::DivisionByZero };
    RETURN_IF_ERROR(stack_.pushv(val1 % val2));
    return FtError{ ErrorCode::Ok };
}

/* LOGICAL OPERATIONS */
FtError FtWordDictionary::forth_equal() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(CONVERT_TO_FORTH_BOOL(val1 == val2)));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_negate() {
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    RETURN_IF_ERROR(stack_.pushv(-1 * val1));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_less() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(CONVERT_TO_FORTH_BOOL(val1 < val2)));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_greater() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(CONVERT_TO_FORTH_BOOL(val1 > val2)));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_or() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val1 | val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_and() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val1 & val2));
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_invert() {
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    RETURN_IF_ERROR(stack_.pushv(~val1));
    return FtError{ ErrorCode::Ok };
}

/* MEMORY METHODS */
FtError FtWordDictionary::forth_variable(std::istringstream& stream) {
    std::string buffer{};
    if(!(stream >> buffer)) 
        return FtError{ ErrorCode::WordNameNotSet };
    std::size_t address = memory_.here();
    RETURN_IF_ERROR(memory_.reserveMemory(sizeof(cell)));
    compileNewWord(buffer, [this, address]() {
        RETURN_IF_ERROR(stack_.pushv(address));
        return FtError{ ErrorCode::Ok };
    }, true );
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_here() {
    if (stack_.isStackFull()) return FtError{ ErrorCode::StackOutOfMemory };
    RETURN_IF_ERROR(stack_.pushv(memory_.here())); 
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_cell() {
    cell n = 0; RETURN_IF_ERROR(stack_.popv(n));
    RETURN_IF_ERROR(stack_.pushv(n * sizeof(cell)));
    return FtError{ ErrorCode::Ok }; 
}

FtError FtWordDictionary::forth_cell_plus() {
    cell n = 0; RETURN_IF_ERROR(stack_.popv(n));
    RETURN_IF_ERROR(stack_.pushv(n + sizeof(cell)));
    return FtError{ ErrorCode::Ok }; 
}

FtError FtWordDictionary::forth_fetch() {
    cell address = 0, value = 0; RETURN_IF_ERROR(stack_.popv(address));
    RETURN_IF_ERROR(memory_.fetch(address, value));
    RETURN_IF_ERROR(stack_.pushv(value));
    return FtError{ ErrorCode::Ok };
}
FtError FtWordDictionary::forth_store() {
    cell address = 0, value = 0; RETURN_IF_ERROR(stack_.pop2v(address, value));
    RETURN_IF_ERROR(memory_.store(address, value));
    return FtError{ ErrorCode::Ok };
}

/* OUTPUT METHODS */
FtError FtWordDictionary::forth_dot() { 
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    output_buffer_.writeNumberToBuffer(val1, FtPrintState::WithSpace);
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_dotquote(std::istringstream& stream) {
    std::string buffer = FtUtils::getStringFromQuotes(stream);
    if (FtStateManager::getState() == FtState::Compiling) {
        FtWord* word = getLastDefinition();
        word->data.emplace_back([this, buffer = std::move(buffer)](){
            output_buffer_.writeStringToBuffer(buffer);
            return FtError{ ErrorCode::Ok };
        } );
    } else output_buffer_.writeStringToBuffer(buffer, FtPrintState::WithSpace);
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_emit() { 
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    output_buffer_.writeCharToBuffer(static_cast<char>(val1), FtPrintState::WithSpace);
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_cr() { 
    output_buffer_.writeNewLineToBuffer();
    return FtError{ ErrorCode::Ok };
}

/* COMPILE METHODS */
FtError FtWordDictionary::forth_colon(std::istringstream& stream) {
    if (FtStateManager::getState() == FtState::Compiling) 
        return FtError{ ErrorCode::AlreadyInCompilingMode };
    std::string buffer{};
    createEmptyWord();
    FtWord* new_word = getLastDefinition();
    if (!(stream >> new_word->name))
        return FtError{ ErrorCode::WordNameNotSet };
    new_word->data.clear();
    new_word->is_immediate = false;
    new_word->is_hidden = true;
    FtStateManager::setState(FtState::Compiling);
    return FtError{ ErrorCode::Ok };
}

FtError FtWordDictionary::forth_semicolon() {
    if (FtStateManager::getState() != FtState::Compiling) 
        return FtError{ ErrorCode::NotInCompilingMode };
    FtWord* new_word = getLastDefinition();
    auto body = std::move(new_word->data);
    new_word->code = [this, body = std::move(body)]() -> FtError {
        for(auto& token: body) 
            RETURN_IF_ERROR(token());
        return FtError{ ErrorCode::Ok };
    };
    new_word->is_hidden = false;
    FtStateManager::setState(FtState::Inline);
    return FtError{ ErrorCode::Ok }; 
}
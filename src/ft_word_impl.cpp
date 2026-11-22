#include <cstdint>
#include "../include/ft_word.hpp"
#include "../include/ft_utils.hpp"
#include "../include/ft_state.hpp"

/* STACK METHODS */
FtError FtWordDictionary::forth_drop() { return stack_.pop(); }

FtError FtWordDictionary::forth_dup() { 
    cell val = 0; RETURN_IF_ERROR(stack_.topv(val));
    return stack_.pushv(val);
}

FtError FtWordDictionary::forth_swap() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    RETURN_IF_ERROR(stack_.pushv(val2));
    return stack_.pushv(val1);
}

FtError FtWordDictionary::forth_rot() {
    cell val1 = 0, val2 = 0, val3 = 0; RETURN_IF_ERROR(stack_.pop3v(val1, val2, val3));
    RETURN_IF_ERROR(stack_.pushv(val2)); RETURN_IF_ERROR(stack_.pushv(val3));
    return stack_.pushv(val1);
}

FtError FtWordDictionary::forth_over() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    RETURN_IF_ERROR(stack_.topv(val2)); RETURN_IF_ERROR(stack_.pushv(val1));
    return stack_.pushv(val2);
}

/* IF / ELSE / THEN */
FtError FtWordDictionary::forth_if(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    auto& body = getLastDefinition()->data;
    ctrl_flow_.emplace_back(FtCtrlFlow::Kind::IfKind, body.size());
    compileNewWordBody([](std::size_t&) -> FtError {
        return FtError{ FtErrorCode::NotImplemented };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_else(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (ctrl_flow_.empty() || ctrl_flow_.back().kind != FtCtrlFlow::Kind::IfKind)
        return FtError{ FtErrorCode::UnterminatedIf };
    auto& body = getLastDefinition()->data;
    std::size_t if_position = ctrl_flow_.back().position; ctrl_flow_.pop_back();
    std::size_t after_else_position = body.size() + 1;
    body[if_position] = [this, after_else_position](std::size_t& next) -> FtError {
        cell condition = 0; RETURN_IF_ERROR(stack_.popv(condition));
        if (!CONVERT_FORTH_TO_BOOL(condition)) next = after_else_position;
        return FtError{ FtErrorCode::Ok };
    };
    ctrl_flow_.emplace_back(FtCtrlFlow::Kind::ElseKind, body.size());
    compileNewWordBody([](std::size_t&) -> FtError {
        return FtError{ FtErrorCode::NotImplemented };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_then(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (ctrl_flow_.empty()) 
        return FtError{ FtErrorCode::UnexpectedThen };
    auto& body = getLastDefinition()->data;
    std::size_t target = body.size();
    FtCtrlFlow cf = ctrl_flow_.back(); ctrl_flow_.pop_back();
    if (cf.kind == FtCtrlFlow::Kind::IfKind) {
        body[cf.position] = [this, target](std::size_t& next) -> FtError {
            cell condition = 0; RETURN_IF_ERROR(stack_.popv(condition));
            if (!CONVERT_FORTH_TO_BOOL(condition)) next = target;
            return FtError{ FtErrorCode::Ok };
        };
    } else if (cf.kind == FtCtrlFlow::Kind::ElseKind) {
        body[cf.position] = [this, target](std::size_t& next) -> FtError {
            next = target;
            return FtError{ FtErrorCode::Ok };
        };
    } else return FtError{ FtErrorCode::UnexpectedThen };
    return FtError{ FtErrorCode::Ok };
}

/* DO-LOOP / BEGIN-UNTIL / BEGIN-WHILE-REPEAT */
FtError FtWordDictionary::forth_do(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    auto& body = getLastDefinition()->data;
    compileNewWordBody([this](std::size_t&) -> FtError {
        cell index = 0, limit = 0; RETURN_IF_ERROR(stack_.pop2v(limit, index));
        loop_frames_.emplace_back(index, limit);
        return FtError{ FtErrorCode::Ok };
    } );
    ctrl_flow_.emplace_back(FtCtrlFlow::Kind::DoKind, body.size());
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_loop(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (ctrl_flow_.empty() || ctrl_flow_.back().kind != FtCtrlFlow::Kind::DoKind)
        return FtError{ FtErrorCode::UnexpectedLoop };
    auto& body = getLastDefinition()->data;
    FtCtrlFlow cf = std::move(ctrl_flow_.back()); ctrl_flow_.pop_back();
    std::size_t target = cf.position;
    compileNewWordBody([this, target](std::size_t& next) -> FtError {
        auto& frame = loop_frames_.back();
        if (++frame.index < frame.limit) next = target;
        else loop_frames_.pop_back();
        return FtError{ FtErrorCode::Ok };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_begin(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    ctrl_flow_.emplace_back(FtCtrlFlow::Kind::BeginKind, getLastDefinition()->data.size());
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_until(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (ctrl_flow_.empty() || ctrl_flow_.back().kind != FtCtrlFlow::Kind::BeginKind)
        return FtError{ FtErrorCode::UnexpectedUntil };
    auto& body = getLastDefinition()->data;
    FtCtrlFlow cf = std::move(ctrl_flow_.back()); ctrl_flow_.pop_back();
    std::size_t target = cf.position;
    compileNewWordBody([this, target](std::size_t& next) -> FtError {
        cell condition = 0; RETURN_IF_ERROR(stack_.popv(condition));
        if (!CONVERT_FORTH_TO_BOOL(condition)) next = target;
        return FtError{ FtErrorCode::Ok };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_while(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (ctrl_flow_.empty() || ctrl_flow_.back().kind != FtCtrlFlow::Kind::BeginKind)
        return FtError{ FtErrorCode::UnexpectedWhile };
    auto& body = getLastDefinition()->data;
    std::size_t patch_position = body.size();
    compileNewWordBody([](std::size_t&) -> FtError {
        return FtError{ FtErrorCode::NotImplemented };
    } );
    ctrl_flow_.emplace_back(FtCtrlFlow::Kind::WhileKind, patch_position);
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_repeat(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (ctrl_flow_.empty() || ctrl_flow_.back().kind != FtCtrlFlow::Kind::WhileKind)
        return FtError{ FtErrorCode::UnexpectedRepeat };
    auto& body = getLastDefinition()->data;
    FtCtrlFlow while_cf = std::move(ctrl_flow_.back()); ctrl_flow_.pop_back();
    FtCtrlFlow begin_cf = std::move(ctrl_flow_.back()); ctrl_flow_.pop_back();
    std::size_t after_repeat_position = body.size() + 1;
    body[while_cf.position] = [this, after_repeat_position](std::size_t& next) -> FtError {
        cell condition = 0; RETURN_IF_ERROR(stack_.popv(condition));
        if (!CONVERT_FORTH_TO_BOOL(condition))
            next = after_repeat_position;
        return FtError{ FtErrorCode::Ok };
    };
    std::size_t begin_position = begin_cf.position;
    compileNewWordBody([begin_position](std::size_t& next) -> FtError {
        next = begin_position;
        return FtError{ FtErrorCode::Ok };
    } );
    return FtError{ FtErrorCode::Ok };
}

/* MATH OPERATIONS */
FtError FtWordDictionary::forth_plus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(val1 + val2);
}

FtError FtWordDictionary::forth_minus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(val1 - val2);
}

FtError FtWordDictionary::forth_multiply() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(val1 * val2);
}

FtError FtWordDictionary::forth_divide() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    if (val2 == 0) return FtError{ FtErrorCode::DivisionByZero };
    return stack_.pushv(val1 / val2);
}

FtError FtWordDictionary::forth_mod() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    if (val2 == 0) return FtError{ FtErrorCode::DivisionByZero };
    return stack_.pushv(val1 % val2);
}

/* LOGICAL OPERATIONS */
FtError FtWordDictionary::forth_equal() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(CONVERT_TO_FORTH_BOOL(val1 == val2));
}

FtError FtWordDictionary::forth_negate() {
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    return stack_.pushv(-1 * val1);
}

FtError FtWordDictionary::forth_less() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(CONVERT_TO_FORTH_BOOL(val1 < val2));
}

FtError FtWordDictionary::forth_greater() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(CONVERT_TO_FORTH_BOOL(val1 > val2));
}

FtError FtWordDictionary::forth_or() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(val1 | val2);
}

FtError FtWordDictionary::forth_and() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(stack_.pop2v(val1, val2));
    return stack_.pushv(val1 & val2);
}

FtError FtWordDictionary::forth_invert() {
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    return stack_.pushv(~val1);
}

/* MEMORY METHODS */
FtError FtWordDictionary::forth_variable(std::istringstream& stream) {
    if (FtStateManager::getState() != FtState::Inline) 
        return FtError{ FtErrorCode::WordNotSupportedInCompileMode };
    std::string buffer{};
    if(!(stream >> buffer)) 
        return FtError{ FtErrorCode::WordNameNotSet };
    std::size_t address = memory_.here();
    RETURN_IF_ERROR(memory_.reserveMemory(sizeof(cell)));
    compileNewWord(buffer, [this, address]() {
        return stack_.pushv(address);
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_here() {
    if (stack_.isStackFull()) return FtError{ FtErrorCode::StackOutOfMemory };
    return stack_.pushv(memory_.here());
}

FtError FtWordDictionary::forth_cell() {
    cell n = 0; RETURN_IF_ERROR(stack_.popv(n));
    return stack_.pushv(n * sizeof(cell)); 
}

FtError FtWordDictionary::forth_cell_plus() {
    cell n = 0; RETURN_IF_ERROR(stack_.popv(n));
    return stack_.pushv(n + sizeof(cell)); 
}

FtError FtWordDictionary::forth_fetch() {
    cell address = 0, value = 0; RETURN_IF_ERROR(stack_.popv(address));
    RETURN_IF_ERROR(memory_.fetch(address, value));
    return stack_.pushv(value);
}
FtError FtWordDictionary::forth_store() {
    cell address = 0, value = 0; RETURN_IF_ERROR(stack_.pop2v(value, address));
    return memory_.store(address, value);
}

/* OUTPUT METHODS */
FtError FtWordDictionary::forth_dot() { 
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    output_buffer_.writeNumberToBuffer(val1);
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_dotquote(std::istringstream& stream) {
    std::string buffer = FtUtils::getStringFromQuotes(stream);
    if (FtStateManager::getState() == FtState::Compiling) {
        compileNewWordBody([this, buffer = std::move(buffer)](std::size_t&){
            output_buffer_.writeStringToBuffer(buffer);
            return FtError{ FtErrorCode::Ok };
        } );
    } else output_buffer_.writeStringToBuffer(buffer);
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_emit() { 
    cell val1 = 0; RETURN_IF_ERROR(stack_.popv(val1));
    output_buffer_.writeCharToBuffer(static_cast<char>(val1));
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_cr() { 
    output_buffer_.writeNewLineToBuffer();
    return FtError{ FtErrorCode::Ok };
}

/* COMPILE METHODS */
FtError FtWordDictionary::forth_colon(std::istringstream& stream) {
    if (FtStateManager::getState() == FtState::Compiling) 
        return FtError{ FtErrorCode::AlreadyInCompilingMode };
    std::string buffer{};
    if (!(stream >> buffer))
        return FtError{ FtErrorCode::WordNameNotSet };
    int word_index = findWord(buffer);
    FtWord* word = nullptr;
    if (word_index != -1) {
        word = getWordAt(word_index);
        word->is_hidden = true;
        setLastDefinition(word_index);
    } else {
        createEmptyWord();
        word = getLastDefinition();
        word->name = std::move(buffer);
        word->is_immediate = false;
    }
   
    word->data.clear();
    FtStateManager::setState(FtState::Compiling);
    return FtError{ FtErrorCode::Ok };
}

FtError FtWordDictionary::forth_semicolon() {
    if (FtStateManager::getState() != FtState::Compiling) 
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (!ctrl_flow_.empty())
        return FtError{ FtErrorCode::UnterminatedIf };
    if (!loop_frames_.empty())
        return FtError{ FtErrorCode::UnterminatedLoop };
    FtWord* new_word = getLastDefinition();
    auto body = std::move(new_word->data);
    new_word->code = [this, body = std::move(body)]() -> FtError {
        std::size_t ip = 0;
        while (ip < body.size()) {
            std::size_t next = ip + 1;
            RETURN_IF_ERROR(body[ip](next));
            ip = next;
        }
        return FtError{ FtErrorCode::Ok };
    };
    new_word->is_hidden = false;
    FtStateManager::setState(FtState::Inline);
    return FtError{ FtErrorCode::Ok }; 
}
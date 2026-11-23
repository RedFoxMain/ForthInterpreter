#include <cstdint>
#include "../include/ft_interpreter.hpp"
#include "../include/ft_utils.hpp"

/* STACK METHODS */
FtError FtInterpreter::forth_drop() { return runtime_ctx_.stack.pop(); }

FtError FtInterpreter::forth_dup() { 
    cell val = 0; RETURN_IF_ERROR(runtime_ctx_.stack.topv(val));
    return runtime_ctx_.stack.pushv(val);
}

FtError FtInterpreter::forth_swap() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    RETURN_IF_ERROR(runtime_ctx_.stack.pushv(val2));
    return runtime_ctx_.stack.pushv(val1);
}

FtError FtInterpreter::forth_rot() {
    cell val1 = 0, val2 = 0, val3 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop3v(val1, val2, val3));
    RETURN_IF_ERROR(runtime_ctx_.stack.pushv(val2)); RETURN_IF_ERROR(runtime_ctx_.stack.pushv(val3));
    return runtime_ctx_.stack.pushv(val1);
}

FtError FtInterpreter::forth_over() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(val1));
    RETURN_IF_ERROR(runtime_ctx_.stack.topv(val2)); RETURN_IF_ERROR(runtime_ctx_.stack.pushv(val1));
    return runtime_ctx_.stack.pushv(val2);
}

/* IF / ELSE / THEN */
FtError FtInterpreter::forth_if(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    auto& body = getLastDefinedWord()->data;
    runtime_ctx_.ctrl_flow.emplace_back(FtCtrlFlow::Kind::If, body.size());
    dictionary_.compileCall(compile_ctx_, [this](std::size_t&) -> FtError {
        return FtError{ FtErrorCode::NotImplemented };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_else(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (runtime_ctx_.ctrl_flow.empty() || runtime_ctx_.ctrl_flow.back().kind != FtCtrlFlow::Kind::If)
        return FtError{ FtErrorCode::UnterminatedIf };
    auto& body = getLastDefinedWord()->data;
    std::size_t if_position = runtime_ctx_.ctrl_flow.back().position; runtime_ctx_.ctrl_flow.pop_back();
    std::size_t after_else_position = body.size() + 1;
    body[if_position] = [this, after_else_position](std::size_t& next) -> FtError {
        cell condition = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(condition));
        if (!CONVERT_FORTH_TO_BOOL(condition)) next = after_else_position;
        return FtError{ FtErrorCode::Ok };
    };
    runtime_ctx_.ctrl_flow.emplace_back(FtCtrlFlow::Kind::Else, body.size());
    dictionary_.compileCall(compile_ctx_, [this](std::size_t&) -> FtError {
        return FtError{ FtErrorCode::NotImplemented };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_then(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (runtime_ctx_.ctrl_flow.empty()) 
        return FtError{ FtErrorCode::UnexpectedThen };
    auto& body = getLastDefinedWord()->data;
    std::size_t target = body.size();
    FtCtrlFlow cf = runtime_ctx_.ctrl_flow.back(); runtime_ctx_.ctrl_flow.pop_back();
    if (cf.kind == FtCtrlFlow::Kind::If) {
        body[cf.position] = [this, target](std::size_t& next) -> FtError {
            cell condition = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(condition));
            if (!CONVERT_FORTH_TO_BOOL(condition)) next = target;
            return FtError{ FtErrorCode::Ok };
        };
    } else if (cf.kind == FtCtrlFlow::Kind::Else) {
        body[cf.position] = [this, target](std::size_t& next) -> FtError {
            next = target;
            return FtError{ FtErrorCode::Ok };
        };
    } else return FtError{ FtErrorCode::UnexpectedThen };
    return FtError{ FtErrorCode::Ok };
}

/* DO-LOOP / BEGIN-UNTIL / BEGIN-WHILE-REPEAT */
FtError FtInterpreter::forth_do(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    auto& body = getLastDefinedWord()->data;
    dictionary_.compileCall(compile_ctx_, [this](std::size_t&) -> FtError {
        cell index = 0, limit = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(limit, index));
        compile_ctx_.loop_frames.emplace_back(index, limit);
        return FtError{ FtErrorCode::Ok };
    } );
    runtime_ctx_.ctrl_flow.emplace_back(FtCtrlFlow::Kind::Do, body.size());
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_loop(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (runtime_ctx_.ctrl_flow.empty() || runtime_ctx_.ctrl_flow.back().kind != FtCtrlFlow::Kind::Do)
        return FtError{ FtErrorCode::UnexpectedLoop };
    auto& body = getLastDefinedWord()->data;
    FtCtrlFlow cf = std::move(runtime_ctx_.ctrl_flow.back()); runtime_ctx_.ctrl_flow.pop_back();
    std::size_t loop_position = body.size();
    std::size_t after_loop_position = loop_position + 1;
    for (std::size_t leave_pos: cf.leave_positions) {
        body[leave_pos] = [this, after_loop_position](std::size_t& next) -> FtError {
            if (compile_ctx_.loop_frames.empty())
                return FtError{ FtErrorCode::NotInLoop };
            compile_ctx_.loop_frames.pop_back();
            next = after_loop_position;
            return FtError{ FtErrorCode::Ok };
        };
    }
    std::size_t loop_start = cf.position;
    dictionary_.compileCall(compile_ctx_, [this, loop_start](std::size_t& next) -> FtError {
        auto& frame = compile_ctx_.loop_frames.back();
        if (++frame.index < frame.limit) next = loop_start;
        else compile_ctx_.loop_frames.pop_back();
        return FtError{ FtErrorCode::Ok };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_begin(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    runtime_ctx_.ctrl_flow.emplace_back(FtCtrlFlow::Kind::Begin, getLastDefinedWord()->data.size());
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_until(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (runtime_ctx_.ctrl_flow.empty() || runtime_ctx_.ctrl_flow.back().kind != FtCtrlFlow::Kind::Begin)
        return FtError{ FtErrorCode::UnexpectedUntil };
    auto& body = getLastDefinedWord()->data;
    FtCtrlFlow cf = std::move(runtime_ctx_.ctrl_flow.back()); runtime_ctx_.ctrl_flow.pop_back();
    std::size_t target = cf.position;
    dictionary_.compileCall(compile_ctx_, [this, target](std::size_t& next) -> FtError {
        cell condition = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(condition));
        if (!CONVERT_FORTH_TO_BOOL(condition)) next = target;
        return FtError{ FtErrorCode::Ok };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_while(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (runtime_ctx_.ctrl_flow.empty() || runtime_ctx_.ctrl_flow.back().kind != FtCtrlFlow::Kind::Begin)
        return FtError{ FtErrorCode::UnexpectedWhile };
    auto& body = getLastDefinedWord()->data;
    std::size_t patch_position = body.size();
    dictionary_.compileCall(compile_ctx_, [](std::size_t&) -> FtError {
        return FtError{ FtErrorCode::NotImplemented };
    } );
    runtime_ctx_.ctrl_flow.emplace_back(FtCtrlFlow::Kind::While, patch_position);
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_repeat(std::istringstream& stream) {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (runtime_ctx_.ctrl_flow.empty() || runtime_ctx_.ctrl_flow.back().kind != FtCtrlFlow::Kind::While)
        return FtError{ FtErrorCode::UnexpectedRepeat };
    auto& body = getLastDefinedWord()->data;
    FtCtrlFlow while_cf = std::move(runtime_ctx_.ctrl_flow.back()); runtime_ctx_.ctrl_flow.pop_back();
    FtCtrlFlow begin_cf = std::move(runtime_ctx_.ctrl_flow.back()); runtime_ctx_.ctrl_flow.pop_back();
    std::size_t after_repeat_position = body.size() + 1;
    body[while_cf.position] = [this, after_repeat_position](std::size_t& next) -> FtError {
        cell condition = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(condition));
        if (!CONVERT_FORTH_TO_BOOL(condition))
            next = after_repeat_position;
        return FtError{ FtErrorCode::Ok };
    };
    std::size_t begin_position = begin_cf.position;
    dictionary_.compileCall(compile_ctx_, [begin_position](std::size_t& next) -> FtError {
        next = begin_position;
        return FtError{ FtErrorCode::Ok };
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_leave() {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    auto& body = getLastDefinedWord()->data;
    for (auto it = runtime_ctx_.ctrl_flow.rbegin(); it != runtime_ctx_.ctrl_flow.rend(); ++it) {
        if (it->kind == FtCtrlFlow::Kind::Do) {
            it->leave_positions.push_back(body.size());
            dictionary_.compileCall(compile_ctx_, [](std::size_t& next) -> FtError {
                return FtError{ FtErrorCode::NotImplemented };
            } );
            return FtError{ FtErrorCode::Ok };
        }
    }
    return FtError{ FtErrorCode::Ok };
}

/* MATH OPERATIONS */
FtError FtInterpreter::forth_plus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(val1 + val2);
}

FtError FtInterpreter::forth_minus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(val1 - val2);
}

FtError FtInterpreter::forth_multiply() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(val1 * val2);
}

FtError FtInterpreter::forth_divide() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    if (val2 == 0) return FtError{ FtErrorCode::DivisionByZero };
    return runtime_ctx_.stack.pushv(val1 / val2);
}

FtError FtInterpreter::forth_mod() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    if (val2 == 0) return FtError{ FtErrorCode::DivisionByZero };
    return runtime_ctx_.stack.pushv(val1 % val2);
}

/* LOGICAL OPERATIONS */
FtError FtInterpreter::forth_equal() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(CONVERT_TO_FORTH_BOOL(val1 == val2));
}

FtError FtInterpreter::forth_negate() {
    cell val1 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(val1));
    return runtime_ctx_.stack.pushv(-1 * val1);
}

FtError FtInterpreter::forth_less() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(CONVERT_TO_FORTH_BOOL(val1 < val2));
}

FtError FtInterpreter::forth_greater() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(CONVERT_TO_FORTH_BOOL(val1 > val2));
}

FtError FtInterpreter::forth_or() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(val1 | val2);
}

FtError FtInterpreter::forth_and() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(val1, val2));
    return runtime_ctx_.stack.pushv(val1 & val2);
}

FtError FtInterpreter::forth_invert() {
    cell val1 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(val1));
    return runtime_ctx_.stack.pushv(~val1);
}

/* MEMORY METHODS */
FtError FtInterpreter::forth_variable(std::istringstream& stream) {
    if (compile_ctx_.is_compiling) 
        return FtError{ FtErrorCode::WordNotSupportedInCompileMode };
    std::string buffer{};
    if(!(stream >> buffer)) 
        return FtError{ FtErrorCode::WordNameNotSet };
    std::size_t address = runtime_ctx_.memory.here();
    RETURN_IF_ERROR(runtime_ctx_.memory.reserveMemory(sizeof(cell)));
    dictionary_.addWord(buffer, [this, address]() -> FtError {
        return runtime_ctx_.stack.pushv(address);
    } );
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_allot() {
    cell n = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(n));
    return runtime_ctx_.memory.allot(n);
}

FtError FtInterpreter::forth_here() {
    if (runtime_ctx_.stack.isFull()) return FtError{ FtErrorCode::StackOutOfMemory };
    return runtime_ctx_.stack.pushv(runtime_ctx_.memory.here());
}

FtError FtInterpreter::forth_cells() {
    cell n = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(n));
    return runtime_ctx_.stack.pushv(n * sizeof(cell)); 
}

FtError FtInterpreter::forth_cell_plus() {
    cell n = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(n));
    return runtime_ctx_.stack.pushv(n + sizeof(cell)); 
}

FtError FtInterpreter::forth_fetch() {
    cell address = 0, value = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(address));
    RETURN_IF_ERROR(runtime_ctx_.memory.fetch(address, value));
    return runtime_ctx_.stack.pushv(value);
}
FtError FtInterpreter::forth_store() {
    cell address = 0, value = 0; RETURN_IF_ERROR(runtime_ctx_.stack.pop2v(value, address));
    return runtime_ctx_.memory.store(address, value);
}

/* OUTPUT METHODS */
FtError FtInterpreter::forth_dot() { 
    cell val1 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(val1));
    runtime_ctx_.output_buffer.writeNumberToBuffer(val1);
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_dotquote(std::istringstream& stream) {
    std::string buffer = FtUtils::getStringFromQuotes(stream);
    if (compile_ctx_.is_compiling) {
        dictionary_.compileCall(compile_ctx_,  [this, buffer = std::move(buffer)](std::size_t&){
            runtime_ctx_.output_buffer.writeStringToBuffer(buffer);
            return FtError{ FtErrorCode::Ok };
        } );
    } else runtime_ctx_.output_buffer.writeStringToBuffer(buffer);
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_emit() { 
    cell val1 = 0; RETURN_IF_ERROR(runtime_ctx_.stack.popv(val1));
    runtime_ctx_.output_buffer.writeCharToBuffer(static_cast<char>(val1));
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::forth_cr() { 
    runtime_ctx_.output_buffer.writeNewLineToBuffer();
    return FtError{ FtErrorCode::Ok };
}

/* COMPILE METHODS */
FtError FtInterpreter::beginDefinition(std::istringstream& stream) {
    if (compile_ctx_.is_compiling) 
        return FtError{ FtErrorCode::AlreadyInCompilingMode };
    std::string buffer{};
    if (!(stream >> buffer))
        return FtError{ FtErrorCode::WordNameNotSet };
    int word_index = dictionary_.findWord(buffer);
    FtWord* word = nullptr;
    if (word_index != -1) {
        word = dictionary_.getWordAt(word_index);
        word->is_hidden = true;
        setLastDefinitionIndex(word_index);
    } else {
        createEmptyWord();
        word = getLastDefinedWord();
        word->name = std::move(buffer);
        word->is_immediate = false;
    }
   
    word->data.clear();
    compile_ctx_.is_compiling = true;
    return FtError{ FtErrorCode::Ok };
}

FtError FtInterpreter::endDefinition() {
    if (!compile_ctx_.is_compiling)
        return FtError{ FtErrorCode::NotInCompilingMode };
    if (!runtime_ctx_.ctrl_flow.empty())
        return FtError{ FtErrorCode::UnterminatedIf };
    if (!compile_ctx_.loop_frames.empty())
        return FtError{ FtErrorCode::UnterminatedLoop };
    FtWord* new_word = getLastDefinedWord();
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
    compile_ctx_.is_compiling = false;
    return FtError{ FtErrorCode::Ok }; 
}
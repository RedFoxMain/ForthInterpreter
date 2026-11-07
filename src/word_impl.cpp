#include "../include/interpreter.hpp"

/* STACK METHODS */
ErrorCode Interpreter::forth_drop() { 
    RETURN_IF_ERROR(memory_.pop());
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_dup() { 
    if (memory_.isStackEmpty()) return ErrorCode::EmptyStack;
    cell val = 0; RETURN_IF_ERROR(memory_.topv(val));
    RETURN_IF_ERROR(memory_.pushv(val)); 
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_swap() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(val2)); RETURN_IF_ERROR(memory_.pushv(val1));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_rot() {
    cell val1 = 0, val2 = 0, val3 = 0; RETURN_IF_ERROR(memory_.pop3v(val1, val2, val3));
    RETURN_IF_ERROR(memory_.pushv(val2)); RETURN_IF_ERROR(memory_.pushv(val3)); RETURN_IF_ERROR(memory_.pushv(val1));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_over() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.popv(val1));
    RETURN_IF_ERROR(memory_.topv(val2)); RETURN_IF_ERROR(memory_.pushv(val1)); RETURN_IF_ERROR(memory_.pushv(val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_if(std::istringstream& stream) {
    cell condition = 0; RETURN_IF_ERROR(memory_.popv(condition));
    return ErrorCode::Ok;
}

/* MATH OPERATIONS */
ErrorCode Interpreter::forth_plus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(val1 + val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_minus() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(val1 - val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_multiply() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(val1 * val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_divide() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    if (val2 == 0) return ErrorCode::DivisionByZero;
    RETURN_IF_ERROR(memory_.pushv(val1 / val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_mod() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    if (val2 == 0) return ErrorCode::DivisionByZero;
    RETURN_IF_ERROR(memory_.pushv(val1 % val2));
    return ErrorCode::Ok;
}

/* LOGICAL OPERATIONS */
ErrorCode Interpreter::forth_equal() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(CONVERT_TO_FORTH_BOOL(val1 == val2)));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_negate() {
    cell val1 = 0; RETURN_IF_ERROR(memory_.popv(val1));
    RETURN_IF_ERROR(memory_.pushv(-1 * val1));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_less() {
     cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(CONVERT_TO_FORTH_BOOL(val1 < val2)));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_greater() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(CONVERT_TO_FORTH_BOOL(val1 > val2)));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_or() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(val1 | val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_and() {
    cell val1 = 0, val2 = 0; RETURN_IF_ERROR(memory_.pop2v(val1, val2));
    RETURN_IF_ERROR(memory_.pushv(val1 & val2));
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_invert() {
    cell val1 = 0; RETURN_IF_ERROR(memory_.popv(val1));
    RETURN_IF_ERROR(memory_.pushv(~val1));
    return ErrorCode::Ok;
}

/* MEMORY METHODS */
ErrorCode Interpreter::forth_variable(std::istringstream& stream) {
    std::string buffer{};
    if(!(stream >> buffer)) return ErrorCode::WordNameNotSet;
    std::size_t address = memory_.here();
    RETURN_IF_ERROR(memory_.reserveMemory(sizeof(cell)));
    words_.emplace_back(buffer, [this, address]() {
        RETURN_IF_ERROR(memory_.pushv(address));
        return ErrorCode::Ok;
    } );
    return ErrorCode::Ok; 
}

ErrorCode Interpreter::forth_here() {
    if (memory_.isStackFull()) return ErrorCode::StackOutOfMemory;
    RETURN_IF_ERROR(memory_.pushv(memory_.here())); 
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_cell() {
    cell n = 0; RETURN_IF_ERROR(memory_.popv(n));
    RETURN_IF_ERROR(memory_.pushv(n * sizeof(cell)));
    return ErrorCode::Ok; 
}

ErrorCode Interpreter::forth_cell_plus() {
     cell n = 0; RETURN_IF_ERROR(memory_.popv(n));
    RETURN_IF_ERROR(memory_.pushv(n + sizeof(cell)));
    return ErrorCode::Ok; 
}

/* OUTPUT METHODS */
ErrorCode Interpreter::forth_dot() { 
    cell val1 = 0; RETURN_IF_ERROR(memory_.popv(val1));
    std::cout << val1 << " ";
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_dotquote(std::istringstream& stream) {
    std::string buffer = getStringFromQuotes(stream);
    if (is_compiling_) {
        current_token_.data.emplace_back(".\"", [buffer = std::move(buffer)](){
            std::cout << buffer;
            return ErrorCode::Ok;
        } );
    } else std::cout << buffer;
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_emit() { 
    cell val1 = 0; RETURN_IF_ERROR(memory_.popv(val1));
    std::cout << static_cast<char>(val1);
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_cr() { 
    std::cout << "\n"; 
    return ErrorCode::Ok;
}

/* COMPILE METHODS */
ErrorCode Interpreter::forth_colon(std::istringstream& stream) {
    if (is_compiling_) return ErrorCode::AlreadyInCompilingMode;
    std::string buffer{};
    if (!(stream >> buffer)) return ErrorCode::WordNameNotSet;
    current_token_.name = std::move(buffer);
    current_token_.data.clear();
    current_token_.is_immediate = false;
    is_compiling_ = true;
    return ErrorCode::Ok;
}

ErrorCode Interpreter::forth_semicolon() {
    if (!is_compiling_) return ErrorCode::NotInCompilingMode;
    auto body = std::move(current_token_.data);
    current_token_.code = std::move([body = std::move(body)]() -> ErrorCode {
        for(const auto& token: body) 
            RETURN_IF_ERROR(token.code());
        return ErrorCode::Ok;
    });
    is_compiling_ = false;
    words_.push_back(std::move(current_token_));
    return ErrorCode::Ok; 
}
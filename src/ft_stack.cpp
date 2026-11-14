#include "../include/ft_stack.hpp"

FtError FtStack::topv(cell& value) { 
    if (stack_.empty()) return FtError{ ErrorCode::EmptyStack };
    value = stack_.top();
    return FtError{ ErrorCode::Ok };
}

FtError FtStack::pushv(cell value) {
    if (isStackFull()) return FtError{ ErrorCode::StackOutOfMemory };
    stack_.push(value);
    return FtError{ ErrorCode::Ok };
}

FtError FtStack::pop() {
    if (stack_.empty()) return FtError{ ErrorCode::EmptyStack };
    stack_.pop();
    return FtError{ ErrorCode::Ok };
}

FtError FtStack::popv(cell& val1) {
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return FtError{ ErrorCode::Ok };
}

FtError FtStack::pop2v(cell& val1, cell& val2) {
    RETURN_IF_ERROR(topv(val2)); stack_.pop();
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return FtError{ ErrorCode::Ok };
}

FtError FtStack::pop3v(cell& val1, cell& val2, cell& val3) {
    RETURN_IF_ERROR(topv(val3)); stack_.pop();
    RETURN_IF_ERROR(topv(val2)); stack_.pop();
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return FtError{ ErrorCode::Ok };
}

bool FtStack::isStackFull() const { return stack_.size() >= STACK_SIZE; }
bool FtStack::isStackEmpty() const { return stack_.empty(); }
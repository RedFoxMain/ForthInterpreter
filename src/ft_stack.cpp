#include "../include/ft_stack.hpp"

FtError FtStack::topv(cell& value) { 
    if (stack_.empty()) return FtError{ FtErrorCode::EmptyStack };
    value = stack_.top();
    return FtError{ FtErrorCode::Ok };
}

FtError FtStack::pushv(cell value) {
    if (isStackFull()) return FtError{ FtErrorCode::StackOutOfMemory };
    stack_.push(value);
    return FtError{ FtErrorCode::Ok };
}

FtError FtStack::pop() {
    if (stack_.empty()) return FtError{ FtErrorCode::EmptyStack };
    stack_.pop();
    return FtError{ FtErrorCode::Ok };
}

FtError FtStack::popv(cell& val1) {
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return FtError{ FtErrorCode::Ok };
}

FtError FtStack::pop2v(cell& val1, cell& val2) {
    RETURN_IF_ERROR(topv(val2)); stack_.pop();
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return FtError{ FtErrorCode::Ok };
}

FtError FtStack::pop3v(cell& val1, cell& val2, cell& val3) {
    RETURN_IF_ERROR(topv(val3)); stack_.pop();
    RETURN_IF_ERROR(topv(val2)); stack_.pop();
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return FtError{ FtErrorCode::Ok };
}

bool FtStack::isStackFull() const { return stack_.size() >= STACK_SIZE; }
bool FtStack::isStackEmpty() const { return stack_.empty(); }
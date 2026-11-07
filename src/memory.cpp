#include "../include/memory.hpp"

ErrorCode Memory::allot() {
    cell bytes_cnt = 0; RETURN_IF_ERROR(popv(bytes_cnt));
    if (bytes_cnt < 0) return ErrorCode::BadAddress;
    if (static_cast<std::size_t>(bytes_cnt) > MEMORY_SIZE - memory_ptr_)
        return ErrorCode::MemoryOverflow;
    memory_ptr_ += bytes_cnt;
    return ErrorCode::Ok;
}

ErrorCode Memory::fetch() {
    cell address = 0; RETURN_IF_ERROR(popv(address));
    if (!isValidAddress(address)) return ErrorCode::BadAddress;
    cell value = 0;
    std::memcpy(&value, memory_.get() + address, sizeof(cell));
    RETURN_IF_ERROR(pushv(value));
    return ErrorCode::Ok;
}

ErrorCode Memory::store() {
    cell value = 0, address = 0; RETURN_IF_ERROR(pop2v(value, address));
    if (!isValidAddress(address)) return ErrorCode::BadAddress;
    std::memcpy(memory_.get() + address, &value, sizeof(cell));
    return ErrorCode::Ok;
}

std::size_t Memory::here() { return memory_ptr_; }

ErrorCode Memory::reserveMemory(std::size_t bytes) {
    if (memory_ptr_ + bytes > MEMORY_SIZE) return ErrorCode::MemoryOverflow;
    memory_ptr_ += bytes;
    return ErrorCode::Ok;
}

ErrorCode Memory::topv(cell& value) { 
    if (isStackEmpty()) return ErrorCode::EmptyStack;
    value = stack_.top();
    return ErrorCode::Ok;
}

ErrorCode Memory::pushv(const cell& value) {
    if (isStackFull()) return ErrorCode::StackOutOfMemory;
    stack_.push(value);
    return ErrorCode::Ok;
}

ErrorCode Memory::pop() {
    if (stack_.empty()) return ErrorCode::EmptyStack;
    stack_.pop();
    return ErrorCode::Ok;
}

ErrorCode Memory::popv(cell& val1) {
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return ErrorCode::Ok;
}

ErrorCode Memory::pop2v(cell& val1, cell& val2) {
    if (stack_.size() < 2) return ErrorCode::ParametersExpected;
    RETURN_IF_ERROR(topv(val2)); stack_.pop();
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return ErrorCode::Ok;
}

ErrorCode Memory::pop3v(cell& val1, cell& val2, cell& val3) {
    if (stack_.size() < 3) return ErrorCode::ParametersExpected;
    RETURN_IF_ERROR(topv(val3)); stack_.pop();
    RETURN_IF_ERROR(topv(val2)); stack_.pop();
    RETURN_IF_ERROR(topv(val1)); stack_.pop();
    return ErrorCode::Ok;
}

bool Memory::isValidAddress(cell address) const {
    return (address >= 0) && (address + sizeof(cell) <= MEMORY_SIZE);
}

const bool Memory::isStackFull() const { return stack_.size() >= STACK_SIZE; }
const bool Memory::isStackEmpty() const { return stack_.empty(); }
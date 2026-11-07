#ifndef _FORTH_MEMORY_HPP_
#define _FORTH_MEMORY_HPP_

#include <stack>
#include <vector>
#include <memory>
#include "error_codes.hpp"

#define MEMORY_SIZE 4 * 1024
#define STACK_SIZE 256

using cell = long long;

class Memory {
public:
    Memory()
        : memory_(std::make_unique<std::byte[]>(MEMORY_SIZE)), memory_ptr_(0) {}
    ~Memory() = default;
    ErrorCode allot();
    ErrorCode fetch();
    ErrorCode store();
    ErrorCode reserveMemory(std::size_t bytes);
    std::size_t here();

    ErrorCode topv(cell& val); 
    ErrorCode pushv(const cell& value);
    ErrorCode pop();
    ErrorCode popv(cell& val1);
    ErrorCode pop2v(cell& val1, cell& val2);
    ErrorCode pop3v(cell& val1, cell& val2, cell& val3);

    const bool isStackFull() const;
    const bool isStackEmpty() const;
    
private:
    bool isValidAddress(cell address) const;

private:
    std::stack<cell> stack_;
    std::size_t memory_ptr_;
    std::unique_ptr<std::byte[]> memory_;
};

#endif // !_FORTH_MEMORY_HPP_
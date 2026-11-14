#ifndef _FORTH_MEMORY_HPP_
#define _FORTH_MEMORY_HPP_

#include <vector>
#include <memory>
#include "error_codes.hpp"

class FtMemory {
public:
    FtMemory()
        : memory_(std::make_unique<std::byte[]>(MEMORY_SIZE)), memory_ptr_(0) {}
    ~FtMemory() = default;
    FtError allot(std::size_t bytes);
    FtError fetch(std::size_t address, cell& value);
    FtError store(std::size_t address, cell value);
    FtError reserveMemory(std::size_t bytes);
    std::size_t here();
    
private:
    bool isValidAddress(cell address) const;

private:
    std::size_t memory_ptr_;
    std::unique_ptr<std::byte[]> memory_;
};

#endif // !_FORTH_MEMORY_HPP_
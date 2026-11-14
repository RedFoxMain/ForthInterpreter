#include "../include/ft_memory.hpp"

FtError FtMemory::allot(std::size_t bytes) {
    if (bytes < 0) return FtError{ ErrorCode::BadAddress };
    if (bytes > MEMORY_SIZE - memory_ptr_)
        return FtError{ ErrorCode::MemoryOverflow };
    memory_ptr_ += bytes;
    return FtError{ ErrorCode::Ok };
}

FtError FtMemory::fetch(std::size_t address, cell& value) {
    if (!isValidAddress(address)) 
        return FtError{ ErrorCode::BadAddress, std::to_string(address) };
    std::memcpy(&value, memory_.get() + address, sizeof(cell));
    return FtError{ ErrorCode::Ok };
}

FtError FtMemory::store(std::size_t address, cell value) {
    if (!isValidAddress(address)) 
        return FtError{ ErrorCode::BadAddress, std::to_string(address) };
    std::memcpy(memory_.get() + address, &value, sizeof(cell));
    return FtError{ ErrorCode::Ok };
}

std::size_t FtMemory::here() { return memory_ptr_; }

FtError FtMemory::reserveMemory(std::size_t bytes) {
    if (bytes > MEMORY_SIZE - memory_ptr_) 
        return FtError{ ErrorCode::MemoryOverflow };
    memory_ptr_ += bytes;
    return FtError{ ErrorCode::Ok };
}

bool FtMemory::isValidAddress(cell address) const {
    return (address >= 0) && (address + sizeof(cell) <= MEMORY_SIZE);
}
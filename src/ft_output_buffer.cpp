#include "../include/ft_output_buffer.hpp"

void FtOutputBuffer::reset() {
    stream_.str("");
    stream_.clear();
    is_buffer_empty_ = true;
}

void FtOutputBuffer::writeStringToBuffer(const std::string& str) {
    if (str.empty()) return;
    stream_ << (!is_buffer_empty_ ? " " : "") << str; 
    is_buffer_empty_ = false;
}

void FtOutputBuffer::writeCharToBuffer(char symbol) {
    stream_ << (!is_buffer_empty_ ? " " : "") << symbol;
    is_buffer_empty_ = false;
}

void FtOutputBuffer::writeNumberToBuffer(long long value) {
    stream_ << (!is_buffer_empty_ ? " " : "") << value;
    is_buffer_empty_ = false;
}

void FtOutputBuffer::writeNewLineToBuffer() { stream_ << "\n"; is_buffer_empty_ = true; }
const std::string FtOutputBuffer::take() { 
    auto temp = stream_.str(); 
    reset();
    return temp; 
}
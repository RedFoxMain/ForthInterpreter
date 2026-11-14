#include "../include/ft_output_buffer.hpp"

void FtOutputBuffer::writeStringToBuffer(const std::string& str, FtPrintState state) { stream_ << str << checkState(state); }
void FtOutputBuffer::writeCharToBuffer(char ch, FtPrintState state) { stream_ << ch << checkState(state); }
void FtOutputBuffer::writeNumberToBuffer(long long number, FtPrintState state) { stream_ << number << checkState(state); }
void FtOutputBuffer::writeNewLineToBuffer() { stream_ << "\n"; }
const std::string FtOutputBuffer::take() { auto temp = stream_.str(); stream_.str(""); stream_.clear(); return temp; }

const std::string FtOutputBuffer::checkState(FtPrintState state) const {
    switch(state) {
        case FtPrintState::WithSpace: return " ";
        case FtPrintState::NewLine: return "\n";
        default: return "";
    }
}
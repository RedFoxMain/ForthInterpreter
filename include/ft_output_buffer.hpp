#ifndef _FORTH_OUTPUT_BUFFER_HPP_
#define _FORTH_OUTPUT_BUFFER_HPP_

#include <sstream>

enum FtPrintState { Ordinary, WithSpace, NewLine };

class FtOutputBuffer {
public:
    FtOutputBuffer() = default;
    ~FtOutputBuffer() = default;

    void writeStringToBuffer(const std::string& str, FtPrintState state = FtPrintState::Ordinary);
    void writeCharToBuffer(char ch, FtPrintState state = FtPrintState::Ordinary);
    void writeNumberToBuffer(long long number, FtPrintState state = FtPrintState::Ordinary);
    void writeNewLineToBuffer();
    const std::string take(); 

private:
    const std::string checkState(FtPrintState state) const;

private:
    std::ostringstream stream_;
};

#endif // !_FORTH_OUTPUT_BUFFER_HPP_
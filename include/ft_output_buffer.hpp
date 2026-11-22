#ifndef _FORTH_OUTPUT_BUFFER_HPP_
#define _FORTH_OUTPUT_BUFFER_HPP_

#include <sstream>

class FtOutputBuffer {
public:
    FtOutputBuffer()
        : is_buffer_empty_(true) {}
    ~FtOutputBuffer() = default;

    void writeStringToBuffer(const std::string& str);
    void writeCharToBuffer(char symbol);
    void writeNumberToBuffer(long long value);
    void writeNewLineToBuffer();
    const std::string take(); 

private:
    bool is_buffer_empty_;
    std::ostringstream stream_;
};

#endif // !_FORTH_OUTPUT_BUFFER_HPP_
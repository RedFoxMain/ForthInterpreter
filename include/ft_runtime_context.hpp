#ifndef _FORTH_RUNTIME_CONTEXT_HPP_
#define _FORTH_RUNTIME_CONTEXT_HPP_

#include <vector>
#include "ft_stack.hpp"
#include "ft_memory.hpp"
#include "ft_control_flow.hpp"
#include "ft_output_buffer.hpp"

class FtRuntimeContext {
public:
    FtRuntimeContext()
        : stack(), memory(), output_buffer() { reset(); }
    ~FtRuntimeContext() = default;

    void reset();

public:
    FtStack stack;
    FtMemory memory;
    FtOutputBuffer output_buffer;
    std::vector<FtCtrlFlow> ctrl_flow;
};

#endif // !_FORTH_RUNTIME_CONTEXT_HPP_
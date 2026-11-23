#ifndef _FORTH_COMPILE_CONTEXT_HPP_
#define _FORTH_COMPILE_CONTEXT_HPP_

#include <vector>
#include "ft_control_flow.hpp"

struct FtCompileContext {
    bool is_compiling;
    int last_definition_at;
    std::vector<FtCtrlFlow> ctrl_flow;
    
    explicit FtCompileContext() { reset(); }
    ~FtCompileContext() = default;

    void reset();
};

#endif // !_FORTH_COMPILE_CONTEXT_HPP_
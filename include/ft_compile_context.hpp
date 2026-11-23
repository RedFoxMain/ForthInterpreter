#ifndef _FORTH_COMPILE_CONTEXT_HPP_
#define _FORTH_COMPILE_CONTEXT_HPP_

#include <vector>
#include "ft_control_flow.hpp"

struct FtCompileContext {
    bool is_compiling;
    int last_definition_at;
    std::vector<FtLoopFrame> loop_frames;
    
    explicit FtCompileContext() { reset(); }
    ~FtCompileContext() = default;

    void reset() {
        is_compiling = false;
        last_definition_at = -1;
        loop_frames.clear();
    }
};

#endif // !_FORTH_COMPILE_CONTEXT_HPP_
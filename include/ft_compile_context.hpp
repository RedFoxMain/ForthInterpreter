#ifndef _FORTH_COMPILE_CONTEXT_HPP_
#define _FORTH_COMPILE_CONTEXT_HPP_

#include <memory>
#include <vector>
#include "ft_word.hpp"
#include "ft_control_flow.hpp"

struct FtCompileContext {
    bool is_compiling;
    std::vector<FtCtrlFlow> ctrl_flow;
    std::shared_ptr<FtWord> current_word;
    
    explicit FtCompileContext() { reset(); }
    ~FtCompileContext() = default;

    void reset();
};

#endif // !_FORTH_COMPILE_CONTEXT_HPP_
#include "../include/ft_compile_context.hpp"

void FtCompileContext::reset() {
    current_word.reset();
    is_compiling = false;
    ctrl_flow.clear();
}
#include "../include/ft_compile_context.hpp"

void FtCompileContext::reset() {
    last_definition_at = -1;
    is_compiling = false;
    ctrl_flow.clear();
}
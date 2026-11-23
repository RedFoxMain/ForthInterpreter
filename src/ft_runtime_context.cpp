#include "../include/ft_runtime_context.hpp"

void FtRuntimeContext::reset() {
    ctrl_flow.clear();
    output_buffer.reset();
}
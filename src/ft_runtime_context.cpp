#include "../include/ft_runtime_context.hpp"

void FtRuntimeContext::reset() {
    loop_frames.clear();
    output_buffer.reset();
}
#ifndef _FT_CONTROL_FLOW_HPP_
#define _FT_CONTROL_FLOW_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <sstream>
#include <functional>
#include "ft_error_codes.hpp"

using Func = std::function<FtError()>;
using FuncWithIp = std::function<FtError(std::size_t&)>;
using ImmediateFunc = std::function<FtError(std::istringstream&)>;

struct FtLoopFrame {
    std::size_t index, limit;
    explicit FtLoopFrame(std::size_t index, std::size_t limit)
        : index(index), limit(limit) {}
};

struct FtCtrlFlow {
    enum Kind { 
        If = 0, Else = 1, Do = 2, Loop = 3, 
        Begin = 4, Until = 5, While = 6, Repeat = 7
    } kind;
    std::size_t position;
    std::vector<std::size_t> leave_positions;
    explicit FtCtrlFlow(Kind kind, std::size_t position)
        : kind(kind), position(position) {}
};

#endif // !_FT_CONTROL_FLOW_HPP_
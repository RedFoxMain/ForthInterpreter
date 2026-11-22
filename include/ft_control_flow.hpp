#ifndef _FT_CONTROL_FLOW_HPP_
#define _FT_CONTROL_FLOW_HPP_

#include <string>
#include <vector>
#include <cstddef>
#include <unordered_map>

using Func = std::function<FtError()>;
using FuncWithIp = std::function<FtError(std::size_t&)>;
using ImmediateFunc = std::function<FtError(std::istringstream&)>;

struct FtLoopFrame {
    std::size_t index, limit;
    std::unordered_map<std::string, FuncWithIp> local_varibales;
    explicit FtLoopFrame(std::size_t index, std::size_t limit)
        : index(index), limit(limit) {}
};

struct FtCtrlFlow {
    enum Kind { 
        IfKind = 0, ElseKind = 1, DoKind = 2, LoopKind = 3, 
        BeginKind = 4, UntilKind = 5, WhileKind = 6, RepeatKind = 7
    } kind;
    std::size_t position;
    explicit FtCtrlFlow(Kind kind, std::size_t position)
        : kind(kind), position(position) {}
};

#endif // !_FT_CONTROL_FLOW_HPP_
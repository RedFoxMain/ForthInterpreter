#ifndef _FORTH_EXECUTION_TOKEN_HPP_
#define _FORTH_EXECUTION_TOKEN_HPP_

#include <string>
#include <sstream>
#include <vector>
#include <functional>
#include "error_codes.hpp"

using CodeFunc = std::function<ErrorCode()>;
using ImmediateFunc = std::function<ErrorCode(std::istringstream&)>;

struct ExecutionToken {
    bool is_immediate;
    std::string name{};
    CodeFunc code = nullptr;
    ImmediateFunc immediate = nullptr;
    std::vector<ExecutionToken> data{};
    ExecutionToken() = default;
    ExecutionToken(std::string name, CodeFunc code)
        : name(std::move(name)), code(std::move(code)), is_immediate(false) {}
    ExecutionToken(std::string name, ImmediateFunc immediate)
        : name(std::move(name)), immediate(std::move(immediate)), is_immediate(true) {}
};

#endif // !_FORTH_EXECUTION_TOKEN_HPP_
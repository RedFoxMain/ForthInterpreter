#ifndef _FORTH_EXECUTION_TOKEN_HPP_
#define _FORTH_EXECUTION_TOKEN_HPP_

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "ft_control_flow.hpp"
#include "ft_error_codes.hpp"

struct FtWord {
    bool is_builtin;
    bool is_immediate;
    bool is_hidden;
    std::string name{};
    std::vector<FuncWithIp> data{};
    Func code = nullptr;
    ImmediateFunc immediate = nullptr;

    FtWord()
        : is_builtin(false), is_immediate(false), is_hidden(true), code(nullptr), immediate(nullptr) {}
    FtWord(std::string name, Func code, bool is_immediate, bool is_hidden = false, bool is_builtin = false)
        : name(std::move(name)), code(std::move(code)), is_builtin(is_builtin), is_immediate(is_immediate), is_hidden(is_hidden) {}
    FtWord(std::string name, ImmediateFunc immediate, bool is_immediate, bool is_hidden = false, bool is_builtin = false)
        : name(std::move(name)), immediate(std::move(immediate)), is_builtin(is_builtin), is_immediate(is_immediate), is_hidden(is_hidden) {}
    ~FtWord() = default;
};

#endif // !_FORTH_EXECUTION_TOKEN_HPP_
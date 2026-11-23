#ifndef _FORTH_EXECUTION_TOKEN_HPP_
#define _FORTH_EXECUTION_TOKEN_HPP_

#include <string>
#include <memory>
#include <vector>
#include <functional>

#include "ft_utils.hpp"
#include "ft_error_codes.hpp"
#include "ft_compile_context.hpp"

struct FtWord {
    bool is_immediate;
    bool is_hidden;
    std::string name{};
    std::vector<FuncWithIp> data{};
    Func code = nullptr;
    ImmediateFunc immediate = nullptr;

    FtWord() = default;
    FtWord(std::string name, Func code, bool is_immediate, bool is_hidden = false)
        : name(std::move(name)), code(std::move(code)), is_immediate(is_immediate), is_hidden(is_hidden) {}
    FtWord(std::string name, ImmediateFunc immediate, bool is_immediate, bool is_hidden = false)
        : name(std::move(name)), immediate(std::move(immediate)), is_immediate(is_immediate), is_hidden(is_hidden) {}
    ~FtWord() = default;
};

class FtWordDictionary {
public:
    FtWordDictionary() = default;
    ~FtWordDictionary() = default;

    void pop();
    void erase(std::size_t index);
    void createEmptyWord();

    void addWord(const std::string& name, Func func, bool is_immediate = false);
    void addWord(const std::string& name, ImmediateFunc imm_func, bool is_immediate = true);
    void compileCall(const FtCompileContext& ctx, FtWord* word);
    void compileCall(const FtCompileContext& ctx, FuncWithIp func);
    
    FtWord* getWordAt(int index);
    int findWord(const std::string& word);
    std::size_t size();

private:
    std::vector<std::unique_ptr<FtWord>> words_;
};

#endif // !_FORTH_EXECUTION_TOKEN_HPP_
#ifndef _FORTH_WORD_DICTIONARY_HPP_
#define _FORTH_WORD_DICTIONARY_HPP_

#include <string>
#include <vector>
#include <memory>
#include "ft_word.hpp"
#include "ft_compile_context.hpp"

class FtWordDictionary {
public:
    FtWordDictionary() = default;
    ~FtWordDictionary() = default;

    void pop();
    void erase(std::shared_ptr<FtWord> word);

    void addWord(std::shared_ptr<FtWord> word);
    void addWord(const std::string& name, Func func, bool is_immediate = false, bool is_builtin = false);
    void addWord(const std::string& name, ImmediateFunc imm_func, bool is_immediate = true, bool is_builtin = false);
    void compileCall(const FtCompileContext& ctx, std::weak_ptr<FtWord> word);
    void compileCall(const FtCompileContext& ctx, FuncWithIp func);
    
    std::weak_ptr<FtWord> getWordAt(int index);
    int findWord(const std::string& word);
    std::size_t size();

private:
    std::vector<std::shared_ptr<FtWord>> words_;
};

#endif // !_FORTH_WORD_DICTIONARY_HPP_
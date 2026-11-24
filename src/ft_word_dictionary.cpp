#include "../include/ft_word_dictionary.hpp"

void FtWordDictionary::pop() {
    words_.pop_back();
}

void FtWordDictionary::erase(std::shared_ptr<FtWord> word) {
    auto it = std::find(words_.begin(), words_.end(), word);
    if (it != words_.end()) words_.erase(it, words_.end());
}

void FtWordDictionary::addWord(std::shared_ptr<FtWord> word) {
    words_.push_back(std::move(word));
}

void FtWordDictionary::addWord(const std::string& name, Func func, bool is_immediate, bool is_builtin) {
    words_.emplace_back(std::make_unique<FtWord>(name, func, is_immediate, false, is_builtin));
}

void FtWordDictionary::addWord(const std::string& name, ImmediateFunc imm_func, bool is_immediate, bool is_builtin) {
    words_.emplace_back(std::make_unique<FtWord>(name, imm_func, is_immediate, false, is_builtin));
}

void FtWordDictionary::compileCall(const FtCompileContext& ctx, std::weak_ptr<FtWord> word) {
    if (auto ptr = word.lock())
        ctx.current_word->data.emplace_back([ptr](std::size_t&) -> FtError { 
            return ptr->code();
        });
}

void FtWordDictionary::compileCall(const FtCompileContext& ctx, FuncWithIp func) {
    if (ctx.current_word != nullptr)
        ctx.current_word->data.emplace_back(func);
}

int FtWordDictionary::findWord(const std::string& word) {
    const auto it = std::find_if(words_.rbegin(), words_.rend(), [&](const std::shared_ptr<FtWord>& token) {
        return token->name == word && !token->is_hidden;
    } );
    return (it != words_.rend()) ? static_cast<int>(words_.size() - 1 - std::distance(words_.rbegin(), it)) : -1;
}

std::size_t FtWordDictionary::size() { return words_.size(); }
std::weak_ptr<FtWord> FtWordDictionary::getWordAt(int index) { return words_.at(index); }
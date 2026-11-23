#include "../include/ft_word.hpp"

void FtWordDictionary::createEmptyWord() {
    words_.emplace_back(std::make_unique<FtWord>());
    words_.back()->is_hidden = true;
}

void FtWordDictionary::pop() {
    words_.pop_back();
}

void FtWordDictionary::erase(std::size_t index) {
    words_.erase(words_.begin() + index, words_.end());
}

void FtWordDictionary::addWord(const std::string& name, Func func, bool is_immediate) {
    words_.emplace_back(std::make_unique<FtWord>(name, func, is_immediate));
}

void FtWordDictionary::addWord(const std::string& name, ImmediateFunc imm_func, bool is_immediate) {
    words_.emplace_back(std::make_unique<FtWord>(name, imm_func, is_immediate));
}

void FtWordDictionary::compileCall(const FtCompileContext& ctx, FtWord* word) {
    if (ctx.last_definition_at == -1) 
        return;
    words_.at(ctx.last_definition_at)->data.emplace_back([word](std::size_t&) -> FtError { 
        return word->code();
    });
}

void FtWordDictionary::compileCall(const FtCompileContext& ctx, FuncWithIp func) {
    if (ctx.last_definition_at == -1)
        return;
    words_.at(ctx.last_definition_at)->data.emplace_back(func);
}

int FtWordDictionary::findWord(const std::string& word) {
    const auto index = std::find_if(words_.begin(), words_.end(), [&](const std::unique_ptr<FtWord>& token) {
        return token->name == word && !token->is_hidden;
    } );
    return (index != words_.end()) ? std::distance(words_.begin(), index) : -1;
}

std::size_t FtWordDictionary::size() { return words_.size(); }
FtWord* FtWordDictionary::getWordAt(int index) { return words_.at(index).get(); }
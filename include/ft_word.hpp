#ifndef _FORTH_EXECUTION_TOKEN_HPP_
#define _FORTH_EXECUTION_TOKEN_HPP_

#include <string>
#include <optional>
#include <memory>
#include <vector>
#include <functional>
#include <numeric>

#include "error_codes.hpp"
#include "ft_memory.hpp"
#include "ft_stack.hpp"
#include "ft_state.hpp"
#include "ft_utils.hpp"
#include "ft_output_buffer.hpp"

using Func = std::function<FtError()>;
using ImmediateFunc = std::function<FtError(std::istringstream&)>;

struct FtWord {
    bool is_immediate;
    bool is_hidden;
    std::string name{};
    std::vector<Func> data{};
    Func code = nullptr;
    ImmediateFunc immediate = nullptr;

    FtWord() = default;
    FtWord(std::string name, Func code, bool is_immediate, bool is_hidden = false)
        : name(std::move(name)), code(std::move(code)), is_immediate(is_immediate), is_hidden(is_hidden) {}
    FtWord(std::string name, ImmediateFunc immediate, bool is_immediate, bool is_hidden = false)
        : name(std::move(name)), immediate(std::move(immediate)), is_immediate(is_immediate), is_hidden(is_hidden) {}
};

class FtWordDictionary {
public:
    FtWordDictionary()
        : last_definition_at_(-1) {}
    ~FtWordDictionary() = default;

    void initDictionary();
    void createEmptyWord();
    void deleteLastDefinition();
    void compileNewWord(const std::string& name, Func func, bool is_immediate = false);
    void compileNewWord(const std::string& name, ImmediateFunc imm_func, bool is_immediate = true);
    int findWord(const std::string& word);
    FtError evaluate(const std::string& code);
    FtWord* getWordAt(int index);
    FtWord* getLastDefinition();

    FtMemory& getMemory();
    const FtMemory& getMemory() const;
    FtStack& getStack();
    const FtStack& getStack() const; 
    FtOutputBuffer& getOutputBuffer();
    const FtOutputBuffer& getOutputBuffer() const;

    FtError forth_drop();
    FtError forth_dup();
    FtError forth_swap();
    FtError forth_rot();
    FtError forth_over();

    FtError forth_if(std::istringstream& stream);

    FtError forth_plus();
    FtError forth_minus();
    FtError forth_multiply();
    FtError forth_divide();
    FtError forth_mod();

    FtError forth_equal();
    FtError forth_less();
    FtError forth_greater();
    FtError forth_negate();
    FtError forth_invert();
    FtError forth_or();
    FtError forth_and();

    FtError forth_dot();
    FtError forth_emit();
    FtError forth_cr();

    FtError forth_variable(std::istringstream& stream);
    FtError forth_here();
    FtError forth_cell();
    FtError forth_cell_plus();
    FtError forth_fetch();
    FtError forth_store();

    FtError forth_dotquote(std::istringstream& stream);
    FtError forth_colon(std::istringstream& stream);
    FtError forth_semicolon();

private:
    FtStack stack_;
    FtMemory memory_;
    int last_definition_at_;
    FtOutputBuffer output_buffer_;
    std::vector<std::unique_ptr<FtWord>> words_;
};

#endif // !_FORTH_EXECUTION_TOKEN_HPP_
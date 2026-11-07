#ifndef _FORTH_INTERPRETER_HPP_
#define _FORTH_INTERPRETER_HPP_

#include <iostream>
#include <algorithm>
#include "execution_token.hpp"
#include "memory.hpp"


class Interpreter {
public:
    Interpreter()
        : is_compiling_(false) {}
    ~Interpreter() = default;

    void run();
    
private:
    void registerWords();
    ErrorCode interpretWord(const std::string& code);
    ExecutionToken* isWordBuiltIn(const std::string& word);
    std::string getStringFromQuotes(std::istringstream& stream) const;
    ErrorCode getNumber(const std::string& word, cell& value);
    std::string getError(ErrorCode error_code) const;
    bool isDigit(const std::string& word) const;

    ErrorCode forth_drop();
    ErrorCode forth_dup();
    ErrorCode forth_swap();
    ErrorCode forth_rot();
    ErrorCode forth_over();

    ErrorCode forth_if(std::istringstream& stream);

    ErrorCode forth_plus();
    ErrorCode forth_minus();
    ErrorCode forth_multiply();
    ErrorCode forth_divide();
    ErrorCode forth_mod();

    ErrorCode forth_equal();
    ErrorCode forth_less();
    ErrorCode forth_greater();
    ErrorCode forth_negate();
    ErrorCode forth_invert();
    ErrorCode forth_or();
    ErrorCode forth_and();

    ErrorCode forth_dot();
    ErrorCode forth_emit();
    ErrorCode forth_cr();

    ErrorCode forth_variable(std::istringstream& stream);
    ErrorCode forth_here();
    ErrorCode forth_cell();
    ErrorCode forth_cell_plus();

    ErrorCode forth_dotquote(std::istringstream& stream);
    ErrorCode forth_colon(std::istringstream& stream);
    ErrorCode forth_semicolon();
    
private:
    bool is_compiling_;
    Memory memory_;
    ExecutionToken current_token_;
    std::vector<ExecutionToken> words_;
};

#endif // !_FORTH_INTERPRETER_HPP_
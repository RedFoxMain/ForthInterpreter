#ifndef _FORTH_INTERPRETER_HPP_
#define _FORTH_INTERPRETER_HPP_

#include "ft_word.hpp"

class Interpreter {
public:
    Interpreter() = default;
    ~Interpreter() = default;
    void run();
    
private:
    FtWordDictionary word_dictionary_;
};

#endif // !_FORTH_INTERPRETER_HPP_
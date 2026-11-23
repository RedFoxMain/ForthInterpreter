#ifndef _FORTH_INTERPRETER_HPP_
#define _FORTH_INTERPRETER_HPP_

#include "ft_word.hpp"
#include "ft_compile_context.hpp"
#include "ft_runtime_context.hpp"

class FtInterpreter {
public:
    FtInterpreter() = default;
    ~FtInterpreter() = default;
    void run();

private:
    void initDictionary();
    void createEmptyWord();
    void abortCompile();
    void setLastDefinitionIndex(int index);
    FtError evaluate(const std::string& code);
    FtWord* getLastDefinedWord();
    
    FtError forth_drop();
    FtError forth_dup();
    FtError forth_swap();
    FtError forth_rot();
    FtError forth_over();

    FtError forth_if(std::istringstream& stream);
    FtError forth_else(std::istringstream& stream);
    FtError forth_then(std::istringstream& stream);

    FtError forth_do(std::istringstream& stream);
    FtError forth_loop(std::istringstream& stream);
    FtError forth_begin(std::istringstream& stream);
    FtError forth_until(std::istringstream& stream);
    FtError forth_while(std::istringstream& stream);
    FtError forth_repeat(std::istringstream& stream);
    FtError forth_leave();

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
    FtError beginDefinition(std::istringstream& stream);
    FtError endDefinition();
    
private:
    FtWordDictionary dictionary_;
    FtRuntimeContext runtime_ctx_;
    FtCompileContext compile_ctx_;
};

#endif // !_FORTH_INTERPRETER_HPP_
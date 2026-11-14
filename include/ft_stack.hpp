#ifndef _FORTH_STACK_HPP_
#define _FORTH_STACK_HPP_

#include <stack>
#include "error_codes.hpp"

class FtStack {
public:
    FtStack() = default;
    ~FtStack() = default;

    FtError topv(cell& val); 
    FtError pushv(cell value);
    FtError pop();
    FtError popv(cell& val1);
    FtError pop2v(cell& val1, cell& val2);
    FtError pop3v(cell& val1, cell& val2, cell& val3);

    bool isStackFull() const;
    bool isStackEmpty() const;

private:
    std::stack<cell> stack_;
};

#endif // !_FORTH_STACK_HPP_
#ifndef _FORTH_STATE_HPP_
#define _FORTH_STATE_HPP_

enum FtState { Inline = 0, Compiling };

class FtStateManager {
public:
    FtStateManager() = default;
    ~FtStateManager() = default;

    static void setState(FtState state);
    static FtState getState();

private:
    static FtState current_state_;
};

#endif // !_FORTH_STATE_HPP_
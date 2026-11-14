#include "../include/ft_state.hpp"

FtState FtStateManager::current_state_ = FtState::Inline;

void FtStateManager::setState(FtState state) { current_state_ = state; }
FtState FtStateManager::getState() { return current_state_; }
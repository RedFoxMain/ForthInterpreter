#include <iostream>
#include "../include/interpreter.hpp"
#include "../include/ft_state.hpp"
#include "../include/ft_utils.hpp"

void Interpreter::run() {
    word_dictionary_.initDictionary();
    std::string buffer{};
    std::cout << ">> ";
    while (std::getline(std::cin, buffer) && buffer != "bye") {
        if (buffer.empty()) continue;
        FtError err = word_dictionary_.evaluate(buffer);
        if (FtStateManager::getState() != FtState::Compiling || err.error_code != ErrorCode::Ok) {
            word_dictionary_.getOutputBuffer().writeStringToBuffer(FtUtils::getError(err), FtPrintState::NewLine);
            std::cout << word_dictionary_.getOutputBuffer().take();
        } else if (FtStateManager::getState() == FtState::Compiling) {
            word_dictionary_.deleteLastDefinition();
            FtStateManager::setState(FtState::Inline);
        }
        std::cout << ">> ";
    }
}
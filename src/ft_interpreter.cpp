#include <iostream>
#include "../include/ft_interpreter.hpp"
#include "../include/ft_state.hpp"
#include "../include/ft_utils.hpp"

void Interpreter::run() {
    word_dictionary_.initDictionary();
    std::string buffer{};
    std::cout << ">> ";
    while (std::getline(std::cin, buffer) && buffer != "bye") {
        if (buffer.empty()) continue;
        FtError err = word_dictionary_.evaluate(buffer);
        switch(FtStateManager::getState()) {
            case FtState::Inline:
                word_dictionary_.getOutputBuffer().writeStringToBuffer(FtUtils::getError(err));
                std::cout << word_dictionary_.getOutputBuffer().take() << "\n";
                break;
            case FtState::Compiling:
                if (err.error_code != FtErrorCode::Ok) {
                    word_dictionary_.deleteLastDefinition();
                    word_dictionary_.getOutputBuffer().writeStringToBuffer(FtUtils::getError(err));
                    std::cout << word_dictionary_.getOutputBuffer().take() << "\n";
                }
                break;
        }
        std::cout << ">> ";
    }
}
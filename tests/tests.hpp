#ifndef _FORTH_TESTS_HPP_
#define _FORTH_TESTS_HPP_

#include <iostream>
#include <sstream>
#include <string>

#include "../include/ft_interpreter.hpp"

struct TestCase {
    const char* name;
    const char* code;
    FtErrorCode expected = FtErrorCode::Ok;
};

class TestRunner {
public:
    void expectError(const std::string& name, const std::string& code, FtErrorCode expected) {
        FtInterpreter interpreter;
        const FtError error = interpreter.evaluate(code);
        if (error.error_code != expected) {
            std::cerr << "Test failed: " << name << " (code: " << code << ")\n"
                      << "Expected error code: " << static_cast<int>(expected)
                      << ", got: " << static_cast<int>(error.error_code) << "\n";
            ++failures_;
        } else recordPass(name);
    }

    void expectOutputContains(const std::string& name, const std::string& input, const std::string& expected) {
        FtInterpreter interpreter;
        std::istringstream input_stream(input + "\nbye\n");
        std::ostringstream output_stream;
        std::streambuf* previous_input = std::cin.rdbuf(input_stream.rdbuf());
        std::streambuf* previous_output = std::cout.rdbuf(output_stream.rdbuf());

        interpreter.run();

        std::cin.rdbuf(previous_input);
        std::cout.rdbuf(previous_output);

        if (output_stream.str().find(expected) == std::string::npos) {
            std::cerr << "Test failed: " << name << " (input: " << input << ")\n"
                      << "Expected output to contain: " << expected
                      << "\nActual output: " << output_stream.str() << "\n";
            ++failures_;
        } else {
            recordPass(name);
        }
    }

    int failures() const { return failures_; }
    int total() const { return passed_ + failures_; }

private:
    void recordPass(const std::string& name) {
        std::cout << "Test passed: " << name << '\n';
        ++passed_;
    }

private:
    int passed_ = 0;
    int failures_ = 0;
};

#endif

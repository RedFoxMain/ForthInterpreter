cmake_minimum_required(VERSION 3.25)
project("ForthInterpreter")

set( SRC_FILES
    "src/ft_interpreter.cpp"
    "src/ft_word_impl.cpp"
    "src/ft_memory.cpp"
    "src/ft_stack.cpp"
    "src/ft_utils.cpp"
    "src/ft_output_buffer.cpp"
    "src/ft_runtime_context.cpp"
    "src/ft_compile_context.cpp"
    "src/ft_word_dictionary.cpp"
)

set( HEADER_FILES
    "include/ft_interpreter.hpp"
    "include/ft_word.hpp"
    "include/ft_memory.hpp"
    "include/ft_stack.hpp"
    "include/ft_utils.hpp"
    "include/ft_output_buffer.hpp"
    "include/ft_control_flow.hpp"
    "include/ft_error_codes.hpp"
    "include/ft_runtime_context.hpp"
    "include/ft_compile_context.hpp"
    "include/ft_word_dictionary.hpp"
)

add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
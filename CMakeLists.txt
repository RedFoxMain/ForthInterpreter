cmake_minimum_required(VERSION 3.25)
project("ForthInterpreter")

set( SRC_FILES
    "src/interpreter.cpp"
    "src/word_impl.cpp"
    "src/utils.cpp"
    "src/memory.cpp"
)

set( HEADER_FILES
    "include/interpreter.hpp"
    "include/execution_token.hpp"
    "include/memory.hpp"
    "include/error_codes.hpp"
)

add_executable(${PROJECT_NAME} "src/main.cpp" ${SRC_FILES} ${HEADER_FILES})
target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_FILES})
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
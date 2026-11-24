#include <cstdlib>
#include "tests.hpp"

int main() {
    TestRunner tests;
    const TestCase successful_words[] = {
        {"drop", "1 drop"},
        {"dup", "1 dup drop drop"},
        {"swap", "1 2 swap drop drop"},
        {"rot", "1 2 3 rot drop drop drop"},
        {"over", "1 2 over drop drop drop"},
        {"if / else / then", ": if-test 0 if 2 else 3 then ; if-test drop"},
        {"do / loop", ": loop-test 2 0 do loop ; loop-test"},
        {"begin / until", ": until-test begin 1 1 = until ; until-test"},
        {"begin / while / repeat", ": repeat-test 0 begin dup 3 < while 1 + repeat ; repeat-test drop"},
        {"leave", ": leave-test 1 0 do leave loop ; leave-test"},
        {"I", ": i-test 2 0 do I drop loop ; i-test"},
        {"J", ": j-test 2 0 do 2 0 do J drop I drop loop loop ; j-test"},
        {"+", "2 3 + drop"},
        {"-", "5 2 - drop"},
        {"*", "2 3 * drop"},
        {"/", "6 2 / drop"},
        {"mod", "7 3 mod drop"},
        {"<", "2 3 < drop"},
        {">", "3 2 > drop"},
        {"=", "3 3 = drop"},
        {"negate", "3 negate drop"},
        {"invert", "1 invert drop"},
        {"and", "3 1 and drop"},
        {"or", "2 1 or drop"},
        {".", "42 ."},
        {"emit", "65 emit"},
        {"cr", "cr"},
        {".\"", ".\" hello\""},
        {"variable", "variable value"},
        {"allot", "8 allot"},
        {"here", "here drop"},
        {"cells", "2 cells drop"},
        {"cell+", "0 cell+ drop"},
        {"@", "0 @ drop"},
        {"!", "42 0 ! 0 @ drop"},
        {": / ;", ": constant-word 7 ; constant-word drop"},
        {";", ": semicolon-word 1 ;"},
    };

    for (const TestCase& test : successful_words)
        tests.expectError(test.name, test.code, FtErrorCode::Ok);

    const TestCase invalid_words[] = {
        {"drop on empty stack", "drop", FtErrorCode::EmptyStack},
        {"dup on empty stack", "dup", FtErrorCode::EmptyStack},
        {"swap with one value", "1 swap", FtErrorCode::EmptyStack},
        {"rot with two values", "1 2 rot", FtErrorCode::EmptyStack},
        {"over with one value", "1 over", FtErrorCode::EmptyStack},
        {"if outside compilation", "if", FtErrorCode::NotInCompilingMode},
        {"else outside compilation", "else", FtErrorCode::NotInCompilingMode},
        {"then outside compilation", "then", FtErrorCode::NotInCompilingMode},
        {"do outside compilation", "do", FtErrorCode::NotInCompilingMode},
        {"loop outside compilation", "loop", FtErrorCode::NotInCompilingMode},
        {"begin outside compilation", "begin", FtErrorCode::NotInCompilingMode},
        {"until outside compilation", "until", FtErrorCode::NotInCompilingMode},
        {"while outside compilation", "while", FtErrorCode::NotInCompilingMode},
        {"repeat outside compilation", "repeat", FtErrorCode::NotInCompilingMode},
        {"leave outside compilation", "leave", FtErrorCode::NotInCompilingMode},
        {"I outside a loop", "I", FtErrorCode::NotInLoop},
        {"J outside nested loops", "J", FtErrorCode::NotInLoop},
        {"+ with insufficient values", "1 +", FtErrorCode::EmptyStack},
        {"- with insufficient values", "1 -", FtErrorCode::EmptyStack},
        {"* with insufficient values", "1 *", FtErrorCode::EmptyStack},
        {"/ by zero", "1 0 /", FtErrorCode::DivisionByZero},
        {"/ with insufficient values", "1 /", FtErrorCode::EmptyStack},
        {"mod by zero", "1 0 mod", FtErrorCode::DivisionByZero},
        {"mod with insufficient values", "1 mod", FtErrorCode::EmptyStack},
        {"< with insufficient values", "1 <", FtErrorCode::EmptyStack},
        {"> with insufficient values", "1 >", FtErrorCode::EmptyStack},
        {"= with insufficient values", "1 =", FtErrorCode::EmptyStack},
        {"negate on empty stack", "negate", FtErrorCode::EmptyStack},
        {"invert on empty stack", "invert", FtErrorCode::EmptyStack},
        {"and with insufficient values", "1 and", FtErrorCode::EmptyStack},
        {"or with insufficient values", "1 or", FtErrorCode::EmptyStack},
        {"dot on empty stack", ".", FtErrorCode::EmptyStack},
        {"emit on empty stack", "emit", FtErrorCode::EmptyStack},
        {"variable without a name", "variable", FtErrorCode::WordNameNotSet},
        {"variable in a definition", ": invalid-variable variable item ;", FtErrorCode::WordNotSupportedInCompileMode},
        {"allot on empty stack", "allot", FtErrorCode::EmptyStack},
        {"allot beyond memory", "4097 allot", FtErrorCode::MemoryOverflow},
        {"cells on empty stack", "cells", FtErrorCode::EmptyStack},
        {"cell+ on empty stack", "cell+", FtErrorCode::EmptyStack},
        {"fetch from invalid address", "4096 @", FtErrorCode::BadAddress},
        {"store to invalid address", "42 4096 !", FtErrorCode::BadAddress},
        {": without a name", ":", FtErrorCode::WordNameNotSet},
        {"redefine a built-in word", ": I ;", FtErrorCode::CannotRedefineBuiltinWord},
        {"; outside compilation", ";", FtErrorCode::NotInCompilingMode},
        {"unknown word", "unknown_word", FtErrorCode::UnknownWord},
        {"number too large", "999999999999999999999999999999", FtErrorCode::NumberTooBig},
    };

    for (const TestCase& test : invalid_words)
        tests.expectError(test.name, test.code, test.expected);

    tests.expectError("else without if", ": invalid-else else ;", FtErrorCode::UnterminatedIf);
    tests.expectError("then without if", ": invalid-then then ;", FtErrorCode::UnexpectedThen);
    tests.expectError("loop without do", ": invalid-loop loop ;", FtErrorCode::UnexpectedLoop);
    tests.expectError("until without begin", ": invalid-until until ;", FtErrorCode::UnexpectedUntil);
    tests.expectError("while without begin", ": invalid-while while ;", FtErrorCode::UnexpectedWhile);
    tests.expectError("repeat without while", ": invalid-repeat repeat ;", FtErrorCode::UnexpectedRepeat);
    tests.expectError("leave outside a loop", ": invalid-leave leave ;", FtErrorCode::NotInLoop);
    tests.expectError("definition with unfinished control flow", ": unfinished begin ;", FtErrorCode::UnterminatedIf);
    tests.expectError("definition with unfinished loop", ": unfinished-loop do ;", FtErrorCode::UnterminatedIf);

    tests.expectOutputContains("addition result", "2 3 + .", "5 ok");
    tests.expectOutputContains("stack word results", "1 2 swap . . 1 2 3 rot . . . 1 2 over . . .", "1 2 1 3 2 1 2 1 ok");
    tests.expectOutputContains("arithmetic and comparison results",
                               "5 2 - . 2 3 * . 7 3 mod . 2 3 < . 3 2 > . 3 3 = .",
                               "3 6 1 -1 -1 -1 ok");
    tests.expectOutputContains("bitwise and unary results",
                               "3 negate . 0 invert . 3 1 and . 2 1 or .",
                               "-3 -1 1 3 ok");
    tests.expectOutputContains("conditional false branch", ": choose 0 if 2 else 3 then ; choose .", "3 ok");
    tests.expectOutputContains("conditional true branch", ": choose 1 0 > if 2 else 3 then ; choose .", "2 ok");
    tests.expectOutputContains("loop index", ": count 3 0 do I . loop ; count", "0 1 2");
    tests.expectOutputContains("variable store and fetch", "variable counter\n42 counter !\ncounter @ .", "42 ok");
    tests.expectOutputContains("memory address and size words", "here . 2 cells . 0 cell+ .", "0 16 8 ok");
    tests.expectOutputContains("dot-quote output", ".\" hello, forth!\"", "hello, forth! ok");
    tests.expectOutputContains("character output", "65 emit", "A ok");
    tests.expectOutputContains("newline output", "cr", ">> \nok");

    std::string full_stack;
    for (std::size_t i = 0; i < STACK_SIZE; ++i) {
        if (i != 0) full_stack += ' ';
        full_stack += '1';
    }
    full_stack += " here";
    tests.expectError("here when the stack is full", full_stack, FtErrorCode::StackOutOfMemory);

    std::cout << "Total tests: " << tests.total()
              << ", passed: " << tests.total() - tests.failures()
              << ", failures: " << tests.failures() << std::endl;
    return tests.failures() == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

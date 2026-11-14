#ifndef _FORTH_ERROR_CODES_HPP_
#define _FORTH_ERROR_CODES_HPP_

#include <string>

#define MEMORY_SIZE 4 * 1024
#define STACK_SIZE 256

using cell = long long;

#define RETURN_IF_ERROR(result)                                                    \
    do {                                                                           \
        if (FtError _func_res = (result); _func_res.error_code != ErrorCode::Ok) { \
            return FtError{ _func_res.error_code, _func_res.descr };               \
        }                                                                          \
    } while (0)

#define CONVERT_TO_FORTH_BOOL(condition) ((condition) ? -1 : 0)
#define CONVERT_FORTH_TO_BOOL(condition) ((condition == -1) ? 1 : 0)

enum ErrorCode { 
    Ok = 0, 
    BadNumber, 
    UnknownWord,
    DivisionByZero,
    EmptyStack,
    StackOutOfMemory,
    ParametersExpected,
    WordNameNotSet,
    AlreadyInCompilingMode,
    NotInCompilingMode,
    MemoryOverflow,
    BadAddress
};

struct FtError {
    ErrorCode error_code;
    std::string descr;
    explicit FtError(ErrorCode error_code, std::string descr = "")
        : error_code(error_code), descr(std::move(descr)) {}
};

#endif // !_FORTH_ERROR_CODES_HPP_
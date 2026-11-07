#ifndef _FORTH_ERROR_CODES_HPP_
#define _FORTH_ERROR_CODES_HPP_

#define RETURN_IF_ERROR(func)                                            \
    do {                                                                 \
        if (ErrorCode __err_code = (func); __err_code != ErrorCode::Ok)  \
            return __err_code;                                           \
    } while (0)
#define CONVERT_TO_FORTH_BOOL(condition) ((condition) ? -1 : 0)

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

#endif // !_FORTH_ERROR_CODES_HPP_
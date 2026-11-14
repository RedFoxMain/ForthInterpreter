#ifndef _FORTH_UTILS_HPP_
#define _FORTH_UTILS_HPP_

#include <string>
#include <sstream>
#include "ft_word.hpp"
#include "error_codes.hpp"

class FtUtils {
public:
    FtUtils() = default;
    ~FtUtils() = default;
    static std::string joinVec(const std::vector<std::string>& vec);
    static std::string getStringFromQuotes(std::istringstream& stream);
    static FtError getNumber(const std::string& word, cell& value);
    static std::string getError(FtError error);
    static bool isDigit(const std::string& word);
};

#endif // !_FORTH_UTILS_HPP_
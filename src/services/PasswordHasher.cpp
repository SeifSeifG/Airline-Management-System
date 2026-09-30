#include "services/picosha2.h" // Or "utils/picosha2.h"
#include "services/PasswordHasher.hpp"

std::string PasswordHasher::hashPassword(const std::string& str) {
    if (str.empty()) return "";
    return picosha2::hash256_hex_string(str);
}


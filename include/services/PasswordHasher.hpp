#include <string>

class PasswordHasher{
public:
    static std::string hashPassword(const std::string& str);
    static std::string deHashPassword(const std::string& str);
};
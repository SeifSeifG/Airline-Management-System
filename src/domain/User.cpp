#include "domain/User.hpp"
#include <utility>

namespace airline {

User::User(std::string id, std::string name, std::string contactInfo,
           std::string username, std::string hashedPassword, Role role)
    : Person(std::move(id), std::move(name), std::move(contactInfo)),
      username_(std::move(username)),
      hashedPassword_(std::move(hashedPassword)),
      role_(role) {}

const std::string& User::getUsername() const { return username_; }
Role User::getRole() const { return role_; }

bool User::checkPassword(const std::string& hashedAttempt) const {
    return hashedAttempt == hashedPassword_;
}

}  // namespace airline

#include "domain/User.hpp"
#include <utility>

namespace airline {

User::User(PersonId_t id, std::string name, contactInfo contactInfo,
           std::string username, std::string hashedPassword, Role role)
    : Person(std::move(id), std::move(name), std::move(contactInfo)),
      username_(std::move(username)),
      hashedPassword_(std::move(hashedPassword)),
      role_(role) {}

const std::string& User::getUsername() const { return username_; }
const std::string& User::getHashedPassword() const {return hashedPassword_; }

bool User::checkPassword(std::string_view hashedAttempt) const {
    return hashedAttempt == hashedPassword_; // Returns true if identical, false otherwise}
}

}  // namespace airline

#pragma once
#include "persistence/Repository.hpp"
#include "domain/User.hpp"
#include <type_traits>
#include <memory>
#include <vector>
#include <string>

namespace airline {

template <typename T>
class UserRepository : public Repository<T> {
    // C++17 compile-time check: T must be User or inherit from User
    static_assert(std::is_base_of_v<User, T>, "UserRepository error: Template parameter T must derive from airline::User!");

public:
    // Generic username lookup
    std::shared_ptr<T> findByUsername(const std::string& username) const {
        auto matches = this->findIf([&username](const T& user) {
            return user.getUsername() == username;
        });
        
        return matches;
    }
};

} // namespace airline
#pragma once
#include <memory>
#include <string>
#include <vector>
#include <type_traits>

#include "domain/User.hpp"
#include "persistence/UserRepo.hpp"

namespace airline {

template <typename UserT>
class UserService {
private:
    UserRepository<UserT>& userRepo_;

public:
    explicit UserService(UserRepository<UserT>& userRepo)
        : userRepo_(userRepo) {
        static_assert(
            std::is_base_of_v<User, UserT>,
            "UserService template parameter UserT must be derived from User."
        );
    }

    void initializeNextId() {
        int maxId = 0;
        const auto& users = userRepo_.getAll(); // Get all entities from repository

        for (const auto& user : users) {
            if (!user) continue;

            std::string idStr = user->getId();
            
            // Extract all numeric digits from the ID string (e.g., "P002" -> 2, "BA015" -> 15)
            int numericId = 0;
            for (char c : idStr) {
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    numericId = numericId * 10 + (c - '0');
                }
            }

            if (numericId > maxId) {
                maxId = numericId;
            }
        }

        // Set the static counter for class T to resume sequence
        UserT::setNextId(maxId + 1);
    }

    bool addUser(const std::shared_ptr<UserT>& user) {
        if (!user) return false;

        // Check if user with same username already exists
        if (userRepo_.findByUsername(user->getUsername()) != nullptr) {
            return false;
        }

        // Repository::add requires (key, item)
        userRepo_.add(user->getId(), user);
        return true;
    }

    bool removeUser(const std::string& username) {
        auto user = userRepo_.findByUsername(username);
        if (!user) return false;

        return userRepo_.remove(user->getId());
    }

    bool updateUserPassword(const std::string& username, const std::string& newPassword) {
        auto user = userRepo_.findByUsername(username);
        if (!user) return false;

        user->setPassword(newPassword);
        return true;
    }

    bool updateUserName(const std::string& username, const std::string& newName) {
        auto user = userRepo_.findByUsername(username);
        if (!user) return false;

        user->setName(newName);
        return true;
    }

    std::shared_ptr<UserT> getUserByUsername(const std::string& username) const {
        return userRepo_.findByUsername(username);
    }

    std::shared_ptr<UserT> getUserById(const std::string& id) const {
        return userRepo_.get(id);
    }

    std::vector<std::shared_ptr<UserT>> getAllUsers() const {
        return userRepo_.getAll();
    }
};

} // namespace airline
#pragma once
#include "domain/User.hpp"

namespace airline {

class Administrator : public User {
public:
    using User::User; // Inherit constructors from User
    void displayMenu() const override;
    Role getRole() const override;
};

}  // namespace airline

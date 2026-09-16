#pragma once
#include "domain/User.hpp"

namespace airline {

class Administrator : public User {
public:
    using User::User;
    void displayMenu() const override;
};

}  // namespace airline

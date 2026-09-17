#pragma once
#include "domain/User.hpp"

namespace airline {

class BookingAgent : public User {
public:
    using User::User;
    void displayMenu() const override;
    Role getRole() const override;
};

}  // namespace airline

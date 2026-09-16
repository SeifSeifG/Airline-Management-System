#pragma once
#include "domain/User.hpp"
#include <memory>

namespace airline {

class LoyaltyAccount;

class Passenger : public User {
public:
    Passenger(std::string id, std::string name, std::string contactInfo,
              std::string username, std::string hashedPassword);
    ~Passenger() override;

    void displayMenu() const override;

    LoyaltyAccount& getLoyaltyAccount();

private:
    std::unique_ptr<LoyaltyAccount> loyaltyAccount_;
};

}  // namespace airline

#pragma once
#include "domain/User.hpp"
#include <memory>

namespace airline {

// Forward declaration
class LoyaltyAccount;

class Passenger : public User {
private:
    // 1-to-1 relationship with LoyaltyAccount, so unique_ptr it is.
    std::unique_ptr<LoyaltyAccount> loyaltyAccount_;

public:
    Passenger(std::string id, std::string name, contactInfo contactInfo,
              std::string username, std::string hashedPassword);
    ~Passenger() override;

    // clean wrappers to LoyaltyAccount
    void earnLoyaltyPoints(int points);
    bool redeemLoyaltyPoints(int points);
    int getLoyaltyBalance() const;
    
    void displayMenu() const override;
    Role getRole() const override;

};

}  // namespace airline

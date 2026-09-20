#pragma once
#include "domain/User.hpp"
#include <memory>
#include <vector>
#include <chrono>



namespace airline {

// Forward declaration
class LoyaltyAccount;

struct TravelHistory {
    std::string flightNumber;
    std::string date;
    std::string origin;
    std::string destination;
    SeatClass seatClass;
}; 

class Passenger : public User {
private:
    // 1-to-1 relationship with LoyaltyAccount, so unique_ptr it is.
    std::unique_ptr<LoyaltyAccount> loyaltyAccount_;
    std::vector<TravelHistory> travelHistory_;  // 1-to-many relationship with TravelHistory

public:
    Passenger(PersonId_t id, std::string name, contactInfo contactInfo,
              std::string username, std::string hashedPassword);
    ~Passenger() override;

    Passenger(const Passenger&) = delete;
    Passenger& operator=(const Passenger&) = delete;
    Passenger(Passenger&&) noexcept;
    Passenger& operator=(Passenger&&) noexcept;

    // clean wrappers to LoyaltyAccount
    void earnLoyaltyPoints(int points);
    bool redeemLoyaltyPoints(int points);
    int getLoyaltyBalance() const;
    
    void displayMenu() const override;
    Role getRole() const override;

};

}  // namespace airline

#include "domain/Passenger.hpp"
#include "domain/LoyaltyAccount.hpp"
#include <iostream>
#include <utility>

namespace airline {

Passenger::Passenger(PersonId_t id, std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword)
    : User(std::move(id), std::move(name), std::move(contactInfo),
           std::move(username), std::move(hashedPassword), Role::Passenger),
      loyaltyAccount_(std::make_unique<LoyaltyAccount>()) {}

// Defined here (not defaulted in the header) because LoyaltyAccount is only
// forward-declared in Passenger.hpp -- the unique_ptr destructor needs the
// complete type, which is only visible in this translation unit.
Passenger::~Passenger() = default;
Passenger::Passenger(Passenger&&) noexcept = default;
Passenger& Passenger::operator=(Passenger&&) noexcept = default;

void Passenger::earnLoyaltyPoints(int points) {
    loyaltyAccount_->earnPoints(points);

}

bool Passenger::redeemLoyaltyPoints(int points) {
    return loyaltyAccount_->redeem(points); // Returns true/false based on success
}

int Passenger::getLoyaltyBalance() const {
    return loyaltyAccount_->getPoints();
}

void Passenger::displayMenu() const {
    std::cout << "[Passenger menu placeholder]\n";
}

Role Passenger::getRole() const {
    return Role::Passenger;
}



}  // namespace airline

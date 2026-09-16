#include "domain/Passenger.hpp"
#include "domain/LoyaltyAccount.hpp"
#include <iostream>
#include <utility>

namespace airline {

Passenger::Passenger(std::string id, std::string name, std::string contactInfo,
                      std::string username, std::string hashedPassword)
    : User(std::move(id), std::move(name), std::move(contactInfo),
           std::move(username), std::move(hashedPassword), Role::Passenger),
      loyaltyAccount_(std::make_unique<LoyaltyAccount>()) {}

// Defined here (not defaulted in the header) because LoyaltyAccount is only
// forward-declared in Passenger.hpp -- the unique_ptr destructor needs the
// complete type, which is only visible in this translation unit.
Passenger::~Passenger() = default;

void Passenger::displayMenu() const {
    std::cout << "[Passenger menu placeholder]\n";
}

LoyaltyAccount& Passenger::getLoyaltyAccount() {
    return *loyaltyAccount_;
}

}  // namespace airline

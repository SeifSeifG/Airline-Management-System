#include "domain/BookingAgent.hpp"
#include <iostream>

namespace airline {

void BookingAgent::displayMenu() const {
    std::cout << "[Booking agent menu placeholder]\n";
}

Role BookingAgent::getRole() const {
    return Role::BookingAgent;
}

}  // namespace airline

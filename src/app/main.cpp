#include "domain/Aircraft.hpp"
#include "domain/Flight.hpp"
#include "domain/Passenger.hpp"
#include "domain/SeatMap.hpp"
#include <iostream>
#include <memory>
#include <vector>

int main() {
    using namespace airline;

    // Smoke test only: proves the domain layer compiles and links.
    // Real entry point (auth, menus, persistence) comes in later steps.
    auto aircraft = std::make_shared<Aircraft>("SU-GEM", "Airbus A320", 180);

    std::vector<Seat> seats;
    seats.emplace_back("1A", SeatClass::Business);
    seats.emplace_back("12C", SeatClass::Economy);
    SeatMap seatMap(std::move(seats));

    Flight flight("MS777", "CAI", "JFK", aircraft, std::move(seatMap));

    Passenger passenger("P001", "Seif Mostafa", contactInfo{"seif@example.com", "0124235235"}, "Nigga1" ,"hashed_placeholder");
    Passenger passenger2("P002", "John Doe", contactInfo{"john@example.com", "0123456789"}, "Nigga2" ,"hashed_placeholder");

    std::cout << "Domain layer scaffolded.\n";
    std::cout << "Flight " << flight.getFlightNumber() << " from " << flight.getOrigin()
              << " to " << flight.getDestination() << "\n";
    std::cout << "Passenger: " << passenger.getName() << " (" << passenger.getUsername() << ")\n";
    std::cout << "Passenger: " << passenger2.getName() << " (" << passenger2.getUsername() << ")\n";

    return 0;
}

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

    Passenger passenger("P001", "Seif Mostafa", "seif@example.com", "seifm", "hashed_placeholder");

    std::cout << "Domain layer scaffolded.\n";
    std::cout << "Flight " << flight.getFlightNumber() << " from " << flight.getOrigin()
              << " to " << flight.getDestination() << "\n";
    std::cout << "Passenger: " << passenger.getName() << " (" << passenger.getUsername() << ")\n";

    return 0;
}

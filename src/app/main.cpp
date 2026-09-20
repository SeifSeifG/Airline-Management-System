#include "domain/Aircraft.hpp"
#include "domain/Flight.hpp"
#include "domain/Passenger.hpp"
#include "domain/SeatLayout.hpp"
#include <iostream>
#include <memory>

int main() {
    using namespace airline;
    
    // Initialize the new map-based SeatLayout and populate seats explicitly
    SeatLayout seatMap;
    seatMap.addSeats(SeatClass::Economy, SeatPosition::Window, 3);
    seatMap.addSeats(SeatClass::Economy, SeatPosition::Middle, 3);
    seatMap.addSeats(SeatClass::Business, SeatPosition::Window, 3);
    seatMap.addSeats(SeatClass::Business, SeatPosition::Middle, 3);

    // Smoke test only: proves the domain layer compiles and links.
    // Real entry point (auth, menus, persistence) comes in later steps.
    auto aircraft = std::make_shared<Aircraft>("SU-GEM", "Airbus A320", 3, std::move(seatMap));

    Flight flight("MS777", "CAI", "JFK", Date{11, 1, 2, 2024}, 3, CrewRegulations{2});
    flight.assignAircraft(aircraft);

    Passenger passenger("P001", "Seif Mostafa", contactInfo{"seif@example.com", "0124235235"}, "Nigga1" ,"hashed_placeholder");
    Passenger passenger2("P002", "John Doe", contactInfo{"john@example.com", "0123456789"}, "Nigga2" ,"hashed_placeholder");

    aircraft->assignSeat(SeatClass::Economy, SeatPosition::Middle, std::make_shared<Passenger>(std::move(passenger)));
    aircraft->assignSeat(SeatClass::Business, SeatPosition::Window, std::make_shared<Passenger>(std::move(passenger2)));

    std::cout << "Domain layer scaffolded.\n";
    std::cout << "Flight " << flight.getFlightNumber() << " from " << flight.getOrigin()
              << " to " << flight.getDestination() << "\n";
    std::cout << "Passenger: " << passenger.getName() << " (" << passenger.getUsername() << ")\n";
    std::cout << "Passenger: " << passenger2.getName() << " (" << passenger2.getUsername() << ")\n";
    std:: cout << aircraft->getBusinessClassCapacity() << std::endl;
    std:: cout << aircraft->getOccupiedBusinessClass() << std::endl;
    std:: cout << aircraft->getOccupiedFirstClass() << std::endl;
    std:: cout << aircraft->getOccupiedEconomyClass() << std::endl;

    return 0;
}
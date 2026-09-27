#include "app/AirlineApplication.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/Aircraft.hpp"
#include "domain/Passenger.hpp"
#include "domain/Pilot.hpp"
#include "domain/FlightAttendant.hpp"
#include "persistence/Loader.hpp"
#include "persistence/Saver.hpp"
#include "services/PasswordHasher.hpp"
#include <iostream>

namespace {
    int nextBookingId_ = 100;
    int nextCheckInId_ = 500;
    // const was added to silence a warning (I dont know why the warning though)
    constexpr const char* defaultSaveFilePath = "data/airline_data.json";
}

namespace airline {

AirlineApplication::AirlineApplication(const std::string& dataFilePath) {
    initialize(dataFilePath);
}

AirlineApplication::~AirlineApplication() {
    try {
        saveToFile(defaultSaveFilePath);
    } catch (...) {
        std::cerr << "Unknown error saving state on shutdown.\n";
    }
}

void AirlineApplication::initialize(const std::string& dataFilePath) {
    Loader::loadFromJson(*this, dataFilePath);
}

void AirlineApplication::saveToFile(const std::string& filePath) const {
    Saver::saveToJson(*this, filePath);
}

bool AirlineApplication::login(const std::string& username, const std::string& password) {
    currentUser_ = authService_.login(username, password);
    return currentUser_ != nullptr;
}

void AirlineApplication::logout() {
    currentUser_.reset();
}

std::shared_ptr<User> AirlineApplication::getCurrentUser() const {
    return currentUser_;
}

bool AirlineApplication::registerNewPassenger(std::string name, contactInfo contact,
                                               std::string username, const std::string& plainPassword) {
    auto passenger = authService_.registerPassenger(std::move(name), std::move(contact),
                                                    std::move(username), plainPassword);
    return passenger != nullptr;
}

std::vector<std::shared_ptr<Flight>> AirlineApplication::searchFlights(const std::string& origin, const std::string& destination) const {
    return bookingService_.searchFlights(origin, destination);
}

std::shared_ptr<BookingRequest> AirlineApplication::createBookingRequest(
    const std::shared_ptr<Passenger>& passenger, 
    const std::shared_ptr<Flight>& flight,
    SeatClass seatClass) 
{
    return bookingService_.createBookingRequest(passenger, flight, seatClass);
}

std::vector<std::shared_ptr<BookingRequest>> AirlineApplication::getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const {
    return bookingService_.getBookingRequestsForPassenger(passenger);
}

std::vector<std::shared_ptr<BookingRequest>> AirlineApplication::getPendingBookingRequests() const {
    return bookingService_.getPendingBookingRequests();
}

std::shared_ptr<CheckInRequest> AirlineApplication::createCheckInRequest(
    const std::shared_ptr<Passenger>& passenger, 
    const std::shared_ptr<Flight>& flight) 
{
    return bookingService_.createCheckInRequest(passenger, flight);
}

std::vector<std::shared_ptr<CheckInRequest>> AirlineApplication::getPendingCheckInRequests() const {
    return bookingService_.getPendingCheckInRequests();
}

std::shared_ptr<CheckInRequest> AirlineApplication::getCheckInRequest(
    const std::shared_ptr<Passenger>& passenger, 
    const std::shared_ptr<Flight>& flight) const 
{
    return bookingService_.getCheckInRequest(passenger, flight);
}

// Helper generator methods
std::string AirlineApplication::generateBookingId() { return "BR-" + std::to_string(nextBookingId_++); }
std::string AirlineApplication::generateCheckInId() { return "CR-" + std::to_string(nextCheckInId_++); }


// --- Flight Scheduling Forwarders ---
bool AirlineApplication::addFlight(const std::string& flightNumber,
                                const std::string& origin,
                                const std::string& destination,
                                const std::string& depTime,
                                std::shared_ptr<Aircraft> ac,
                                float flightDur, 
                                const CrewRegulations& reg) 
{
    return flightService_.addFlight(flightNumber, origin, destination, depTime, ac, flightDur, reg);
}

bool AirlineApplication::assignCrewMember(const std::shared_ptr<Flight>& flight,
                                           const std::shared_ptr<CrewMember>& crewMember) 
{
    return crewService_.assignCrewMember(flight, crewMember);
}

std::shared_ptr<Flight> AirlineApplication::getFlightById(const std::string& flightNumber) const {
    return flightService_.getFlightById(flightNumber);
}

std::vector<std::shared_ptr<Flight>> AirlineApplication::getAllFlights() const {
    return flightService_.getAllFlights();
}

std::vector<std::shared_ptr<Aircraft>> AirlineApplication::getAvailableAircrafts() const {
    return flightService_.getAvailableAircrafts();
}

std::shared_ptr<Aircraft> AirlineApplication::getAircraftByTailNumber(const std::string& tailNumber) const {
    return flightService_.getAircraftByTailNumber(tailNumber);
}

// --- Crew Forwarders ---
std::vector<std::shared_ptr<Pilot>> AirlineApplication::getAvailablePilots() const {
    return crewService_.getAvailablePilots();
}

std::shared_ptr<Pilot> AirlineApplication::getPilotById(const std::string& id) const {
    return crewService_.getPilotById(id);
}

std::vector<std::shared_ptr<FlightAttendant>> AirlineApplication::getAvailableFlightAttendants() const {
    return crewService_.getAvailableFlightAttendants();
}

std::shared_ptr<FlightAttendant> AirlineApplication::getFAById(const std::string& id) const {
    return crewService_.getFAById(id);
}



void AirlineApplication::printSummary() const {
    std::cout << "=== Airline Data Load Summary ===\n\n";

    std::cout << "Aircraft (" << aircraftRepo_.size() << "):\n";
    for (const auto& ac : aircraftRepo_.getAll()) {
        std::cout << "  " << ac->getTailNumber() << " - " << ac->getModel()
                   << " (F:" << ac->getFirstClassCapacity()
                   << " B:" << ac->getBusinessClassCapacity()
                   << " E:" << ac->getEconomyClassCapacity() << ")\n";
    }

    std::cout << "\nPilots (" << pilots_.size() << "):\n";
    for (const auto& p : pilots_.getAll()) {
        std::cout << "  " << p->getName() << " (license: " << p->getLicenseId()
                   << ", hours: " << p->getFlightHours() << ")\n";
    }

    std::cout << "\nFlight Attendants (" << flightAtts_.size() << "):\n";
    for (const auto& fa : flightAtts_.getAll()) {
        std::cout << "  " << fa->getName() << " (license: " << fa->getLicenseId() << ")\n";
    }

    std::cout << "\nAdministrators (" << admins_.size() << "):\n";
    for (const auto& admin : admins_.getAll()) {
        std::cout << "  " << admin->getName() << " (username: " << admin->getUsername() << ")\n";
    }

    std::cout << "\nBooking Agents (" << bookingAgents_.size() << "):\n";
    for (const auto& agent : bookingAgents_.getAll()) {
        std::cout << "  " << agent->getName() << " (username: " << agent->getUsername() << ")\n";
    }

    std::cout << "\nPassengers (" << passengerRepo_.size() << "):\n";
    for (const auto& p : passengerRepo_.getAll()) {
        std::cout << "  " << p->getName() << " (username: " << p->getUsername()
                   << ", loyalty pts: " << p->getLoyaltyBalance() << ")\n";
    }

    std::cout << "\nFlights (" << flightRepo_.size() << "):\n";
    for (const auto& f : flightRepo_.getAll()) {
        std::cout << "  " << f->getFlightNumber() << ": " << f->getOrigin()
                   << " -> " << f->getDestination() << "\n";
    }

    std::cout << "\n===================================\n";
}

}
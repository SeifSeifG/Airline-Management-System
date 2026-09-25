#include "app/AirlineApplication.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/Aircraft.hpp"
#include "domain/Passenger.hpp"
#include "persistence/Loader.hpp"
#include "persistence/Saver.hpp"
#include "services/PasswordHasher.hpp"
#include <iostream>

namespace airline {

AirlineApplication::AirlineApplication() {

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
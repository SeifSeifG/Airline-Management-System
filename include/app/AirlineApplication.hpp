#pragma once
#include "persistence/PassengerRepo.hpp"
#include "persistence/CrewRepo.hpp"
#include "persistence/AircraftRepo.hpp"
#include "persistence/FlightRepo.hpp"
#include "persistence/UserRepo.hpp"
#include "domain/FlightAttendant.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "services/AuthService.hpp"

namespace airline {
    
class AirlineApplication {
    friend class Loader; // Grant Loader direct access to private repositories
    friend class Saver;
private:
    AircraftRepository aircraftRepo_;
    CrewRepository pilots_;
    CrewRepository flightAtts_;

    UserRepository<Administrator> admins_;
    UserRepository<BookingAgent> bookingAgents_;

    PassengerRepository passengerRepo_;
    FlightRepository flightRepo_;
    // BookingRequestRepository bookingRequestRepo_;

    AuthService authService_{passengerRepo_, admins_, bookingAgents_};
    std::shared_ptr<User> currentUser_ = nullptr;

public:
    
    AirlineApplication();

    // Reads the startup data file and populates every repository above.
    // Throws std::runtime_error if the file can't be opened or a
    // required top-level section is missing -- that's treated as a
    // structural problem with the whole file. A malformed individual
    // entry within a section is skipped with a warning to std::cerr,
    // so one bad row doesn't block everything else from loading.
    void initialize(const std::string& dataFilePath);
    void saveToFile(const std::string& filePath) const;

    // authentication service related APIs
    bool login(const std::string& username, const std::string& password);
    void logout();
    std::shared_ptr<User> getCurrentUser() const;
    bool registerNewPassenger(std::string name, contactInfo contact,
                               std::string username, const std::string& plainPassword);


    // Debug/verification helper -- prints what actually loaded into each
    // repository. Not part of the real application flow; remove or gate
    // behind a verbosity flag once services/UI exist and this isn't
    // needed for manual testing anymore.
    void printSummary() const;
};

}
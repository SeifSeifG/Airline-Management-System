#pragma once
#include "persistence/PassengerRepo.hpp"
#include "persistence/CrewRepo.hpp"
#include "persistence/AircraftRepo.hpp"
#include "persistence/FlightRepo.hpp"
#include "persistence/UserRepo.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "domain/BookCheckInRequest.hpp"
#include "services/AuthService.hpp"
#include "services/BookCheckInService.hpp"
#include "services/CrewService.hpp"
#include "services/FlightSchedulingService.hpp"
#include "services/ReportingService.hpp"

namespace airline {

class Loader;
class Saver;
class ConsoleUI; // forward declare 
class Pilot;
class FlightAttendant;
    
class AirlineApplication {
    friend class Loader; // Grant Loader direct access to private repositories
    friend class Saver;
    friend class ConsoleUI;
private:
    AircraftRepository aircraftRepo_;
    CrewRepository pilots_;
    CrewRepository flightAtts_;

    UserRepository<Administrator> admins_;
    UserRepository<BookingAgent> bookingAgents_;

    PassengerRepository passengerRepo_;
    FlightRepository flightRepo_;

    // Request queues
    std::vector<std::shared_ptr<BookingRequest>> bookingRequests_;
    std::vector<std::shared_ptr<CheckInRequest>> checkInRequests_;

    // services
    AuthService authService_{passengerRepo_, admins_, bookingAgents_};
    BookingService bookingService_{flightRepo_, bookingRequests_, checkInRequests_};
    FlightSchedulingService flightService_{flightRepo_, aircraftRepo_};
    CrewService crewService_{pilots_, flightAtts_};
    ReportingService reportingService_{flightRepo_, aircraftRepo_};

    std::shared_ptr<User> currentUser_ = nullptr;

public:
    explicit AirlineApplication(const std::string& dataFilePath);
    ~AirlineApplication();

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
    bool registerNewPassenger(std::string name, contactInfo contact, std::string username, const std::string& plainPassword);

    // Flight search query
    std::vector<std::shared_ptr<Flight>> searchFlights(const std::string& origin, const std::string& destination) const;

    // Booking request functions
    std::shared_ptr<BookingRequest> createBookingRequest(const std::shared_ptr<Passenger>& passenger, 
    const std::shared_ptr<Flight>& flight, SeatClass seatClass, SeatPosition position);
    std::vector<std::shared_ptr<BookingRequest>> getPendingBookingRequests() const;
    std::vector<std::shared_ptr<BookingRequest>> getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const;

    // Check-in request queries
    std::shared_ptr<CheckInRequest> createCheckInRequest(const std::shared_ptr<Passenger>& passenger, const std::shared_ptr<Flight>& flight);
    std::vector<std::shared_ptr<CheckInRequest>> getPendingCheckInRequests() const;
    std::shared_ptr<CheckInRequest> getCheckInRequest(const std::shared_ptr<Passenger>& passenger, const std::shared_ptr<Flight>& flight) const;

    // Helper generator methods
    std::string generateBookingId();
    std::string generateCheckInId();

    // --- Administrator Flight & Crew Management APIs ---
    bool addFlight(const std::string& flightNumber,
                const std::string& origin,
                const std::string& destination,
                const std::string& depTime,
                std::shared_ptr<Aircraft> ac,
                float flightDur,
                const CrewRegulations& reg);
                
    bool assignCrewMember(const std::shared_ptr<Flight>& flight, const std::shared_ptr<CrewMember>& crewMember) ;
    
    std::shared_ptr<Flight> getFlightById(const std::string& flightId) const;
    std::vector<std::shared_ptr<Flight>> getAllFlights() const;

    std::vector<std::shared_ptr<Pilot>> getAvailablePilots() const;
    std::shared_ptr<Pilot> getPilotById(const std::string& id) const;

    std::vector<std::shared_ptr<FlightAttendant>> getAvailableFlightAttendants() const;
    std::shared_ptr<FlightAttendant> getFAById(const std::string& id) const;

    std::vector<std::shared_ptr<Aircraft>> getAvailableAircrafts() const;
    std::shared_ptr<Aircraft> getAircraftByTailNumber(const std::string& tailNumber) const;


    // Debug/verification helper -- prints what actually loaded into each
    // repository. Not part of the real application flow; remove or gate
    // behind a verbosity flag once services/UI exist and this isn't
    // needed for manual testing anymore.
    void printSummary() const;
};

}
#pragma once
#include "persistence/PassengerRepo.hpp"
#include "persistence/CrewRepo.hpp"
#include "persistence/AircraftRepo.hpp"
#include "persistence/FlightRepo.hpp"
#include "persistence/UserRepo.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "domain/BookingRequest.hpp"
#include "services/AuthService.hpp"
#include "services/BookCheckInService.hpp"
#include "services/CrewService.hpp"
#include "services/FlightSchedulingService.hpp"
#include "services/ReportingService.hpp"
#include "services/AircraftManagementService.hpp"
#include "services/UserService.hpp"

namespace airline {

class Loader;
class Saver;
class ConsoleUI; // forward declare 
class Pilot;
class FlightAttendant;

struct BookingRequest;
struct CheckInRequest;

class AirlineApplication {
    friend class Loader; // Grant Loader direct access to private repositories
    friend class Saver;
    friend class ConsoleUI;
private:
    AircraftRepository aircraftRepo_;
    CrewRepository pilots_;
    CrewRepository flightAtts_;

    UserRepository<Administrator> admins_;
    UserRepository<BookingAgent> agentRepo_;

    PassengerRepository passengerRepo_;
    FlightRepository flightRepo_;

    // Request queues
    std::vector<std::shared_ptr<BookingRequest>> bookingRequests_;
    std::vector<std::shared_ptr<CheckInRequest>> checkInRequests_;
    std::vector<std::shared_ptr<FinishedRequest>> finishedRequests_;

    // services
    AuthService authService_{passengerRepo_, admins_, agentRepo_};
    BookingService bookingService_{flightRepo_, bookingRequests_, checkInRequests_, finishedRequests_};
    FlightSchedulingService flightService_{flightRepo_, aircraftRepo_};
    AircraftManagementService aircraftService_{aircraftRepo_};
    CrewService crewService_{pilots_, flightAtts_};
    ReportingService reportingService_{flightRepo_, aircraftRepo_};
    UserService<Passenger> userPassengerService_{passengerRepo_};
    UserService<BookingAgent> userBookingAgentService_{agentRepo_};
    UserService<Administrator> userAdminService_{admins_};

    std::shared_ptr<User> currentUser_ = nullptr;

public:
    explicit AirlineApplication(const std::string& dataFilePath);
    ~AirlineApplication();

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
    bool isDuplicatedBookingReq(const std::shared_ptr<Passenger>& passenger, 
        const std::shared_ptr<Flight>& flight);
    std::shared_ptr<BookingRequest> createBookingRequest(
        const std::shared_ptr<Passenger>& passenger, 
        const std::shared_ptr<Flight>& flight, 
        SeatClass seatClass);

    std::vector<std::shared_ptr<BookingRequest>> getAllBookingRequests() const;
    std::vector<std::shared_ptr<BookingRequest>> getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const;

    // Check-in request queries
    bool confirmBookingRequest(const std::string& bookingRequestId);
    std::shared_ptr<CheckInRequest> createCheckInRequest(
        const std::shared_ptr<Passenger>& passenger, 
        const std::shared_ptr<BookingRequest>& req);

    std::vector<std::shared_ptr<CheckInRequest>> getAllCheckInRequests() const;
    std::vector<std::shared_ptr<CheckInRequest>> getCheckInRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const;
    bool confirmCheckInRequest(const std::string& bookingRequestId); // called to modify the request itself

    const std::vector<std::shared_ptr<FinishedRequest>>& getFinishedRequests() const;

    // Helper generator methods
    std::string generateBookingId(); // the service may or may not be using them
    std::string generateCheckInId(); // I don't recall at this point (I am exhausted)

    // --- Administrator Flight & Crew Management APIs ---
    bool addFlight(const std::string& flightNumber,
                const std::string& origin,
                const std::string& destination,
                const std::string& depTime,
                std::shared_ptr<Aircraft> ac,
                float flightDur,
                const CrewRegulations& reg,
                int basePrice);

    bool removeFlight(const std::string& flightNumber);
    std::shared_ptr<Flight> getFlightById(const std::string& flightId) const;
    std::vector<std::shared_ptr<Flight>> getAllFlights() const;

    // --- Aircraft Management Forwarders ---
    bool addAircraft(const std::string& tailNumber,
                     const std::string& model,
                     const SeatLayout& seatLayout,
                     float maxRunningHours);

    bool removeAircraft(const std::string& tailNumber);
    std::shared_ptr<Aircraft> getAircraftByTailNumber(const std::string& tailNumber) const;
    std::vector<std::shared_ptr<Aircraft>> getAvailableAircrafts() const;
    std::vector<std::shared_ptr<Aircraft>> getAllAircraft() const;

    // --- Crew Management Forwarders ---
    bool assignCrewMember(const std::shared_ptr<Flight>& flight, const std::shared_ptr<CrewMember>& crewMember) ;
    
    std::vector<std::shared_ptr<Pilot>> getAvailablePilots() const;
    std::shared_ptr<Pilot> getPilotById(const std::string& id) const;

    std::vector<std::shared_ptr<FlightAttendant>> getAvailableFlightAttendants() const;
    std::shared_ptr<FlightAttendant> getFAById(const std::string& id) const;

    // --- User Management Delegation Forwarders ---
    bool addUser(Role roleChoice,
                 const std::string& name,
                 const std::string& username,
                 const std::string& password,
                 const std::string& email,
                 const std::string& phone,
                 const int balance = 0); //default parameter for passenger accounts only

    bool removeUser(const std::string& username);

    std::shared_ptr<User> getUserByUsername(const std::string& username) const;
    std::shared_ptr<User> getUserById(const std::string& id) const;
    std::vector<std::shared_ptr<User>> getAllUsers() const;

    // Specific typed lookups
    std::shared_ptr<Passenger> getPassengerByUsername(const std::string& username) const;
    std::shared_ptr<BookingAgent> getAgentByUsername(const std::string& username) const;
    std::shared_ptr<Administrator> getAdminByUsername(const std::string& username) const;

};

}
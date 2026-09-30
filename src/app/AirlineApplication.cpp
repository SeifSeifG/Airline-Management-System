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
    constexpr const char* defaultSaveFilePath = "data/airline_data.json";
}

namespace airline {

AirlineApplication::AirlineApplication(const std::string& dataFilePath) {
    initialize(dataFilePath);
    bookingService_.initializeNextId();
    userAdminService_.initializeNextId();
    userBookingAgentService_.initializeNextId();
    userPassengerService_.initializeNextId();
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

bool AirlineApplication::login(const std::string& username, const std::string& plainPassword) {
    currentUser_ = authService_.login(username, plainPassword);
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

bool AirlineApplication::isDuplicatedBookingReq(const std::shared_ptr<Passenger>& passenger, 
        const std::shared_ptr<Flight>& flight)
{
    return bookingService_.isDuplicatedBookingReq(passenger, flight);
}

std::shared_ptr<BookingRequest> AirlineApplication::createBookingRequest(
    const std::shared_ptr<Passenger>& passenger, 
    const std::shared_ptr<Flight>& flight,
    SeatClass seatClass) 
{
    return bookingService_.createBookingRequest(passenger, flight, seatClass);
    }

std::vector<std::shared_ptr<BookingRequest>> AirlineApplication::getAllBookingRequests() const {
    return bookingService_.getAllBookingRequests();
}

std::vector<std::shared_ptr<BookingRequest>> AirlineApplication::getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const {
    return bookingService_.getBookingRequestsForPassenger(passenger);
}

RequestReply AirlineApplication::confirmBookingRequest(const std::string& bookingRequestId){
    return bookingService_.confirmBookingRequest(bookingRequestId);
}

std::shared_ptr<CheckInRequest> AirlineApplication::createCheckInRequest(
    const std::shared_ptr<Passenger>& passenger, 
    const std::shared_ptr<BookingRequest>& req) 
{
    return bookingService_.createCheckInRequest(passenger, req);
}

std::vector<std::shared_ptr<CheckInRequest>> AirlineApplication::getAllCheckInRequests() const {
    return bookingService_.getAllCheckInRequests();
}

std::vector<std::shared_ptr<CheckInRequest>> AirlineApplication::getCheckInRequestsForPassenger(
    const std::shared_ptr<Passenger>& passenger) const 
{
    return bookingService_.getCheckInRequestsForPassenger(passenger);
}

bool AirlineApplication::confirmCheckInRequest(const std::string& checkInRequestId){
    return bookingService_.confirmCheckInRequest(checkInRequestId);
}

const std::vector<std::shared_ptr<FinishedRequest>>& AirlineApplication::getFinishedRequests() const {
    return finishedRequests_;
}

// Helper generator methods
std::string AirlineApplication::generateBookingId() { return bookingService_.generateBookingId(); }
std::string AirlineApplication::generateCheckInId() { return bookingService_.generateCheckInId(); }

// --- Flight Scheduling Forwarders ---
bool AirlineApplication::addFlight(const std::string& flightNumber,
                                const std::string& origin,
                                const std::string& destination,
                                const std::string& depTime,
                                std::shared_ptr<Aircraft> ac,
                                float flightDur, 
                                const CrewRegulations& reg,
                                int basePrice) 
{
    return flightService_.addFlight(flightNumber, origin, destination, depTime, ac, flightDur, reg, basePrice);
}

bool AirlineApplication::removeFlight(const std::string& flightNumber){
    return flightService_.removeFlight(flightNumber);
}

std::shared_ptr<Flight> AirlineApplication::getFlightById(const std::string& flightNumber) const {
    return flightService_.getFlightById(flightNumber);
}

std::vector<std::shared_ptr<Flight>> AirlineApplication::getAllFlights() const {
    return flightService_.getAllFlights();
}

DepartResult AirlineApplication::departFlight(const std::string& flightNumber){
    return flightService_.departFlight(flightNumber);
}  

// --- Aircraft Management Forwarders ---
bool AirlineApplication::addAircraft(const std::string& tailNumber,
                                      const std::string& model,
                                      const SeatLayout& seatLayout,
                                      float maxRunningHours) 
{
    return aircraftService_.addAircraft(tailNumber, model, seatLayout, maxRunningHours);
}

bool AirlineApplication::removeAircraft(const std::string& tailNumber) {
    return aircraftService_.removeAircraft(tailNumber);
}

std::shared_ptr<Aircraft> AirlineApplication::getAircraftByTailNumber(const std::string& tailNumber) const {
    return aircraftService_.getAircraftByTailNumber(tailNumber);
}

std::vector<std::shared_ptr<Aircraft>> AirlineApplication::getAllAircraft() const {
    return aircraftService_.getAllAircraft();
}

std::vector<std::shared_ptr<Aircraft>> AirlineApplication::getAvailableAircrafts() const {
    return aircraftService_.getAvailableAircrafts();
}

// --- Crew Forwarders ---
bool AirlineApplication::assignCrewMember(const std::shared_ptr<Flight>& flight,
                                           const std::shared_ptr<CrewMember>& crewMember) 
{
    return crewService_.assignCrewMember(flight, crewMember);
}

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

bool AirlineApplication::addUser(Role roleChoice,
                                 const std::string& name,
                                 const std::string& username,
                                 const std::string& password,
                                 const std::string& email,
                                 const std::string& phone,
                                 int balance) {
    contactInfo contact{email, phone};
    std::string hashedPassword = PasswordHasher::hashPassword(password);
    switch (roleChoice) {
        case Role::Administrator: { // Administrator
            auto admin     = std::make_shared<Administrator>(name, contact, username, hashedPassword);
            return userAdminService_.addUser(admin);
        }
        case Role::BookingAgent: { // Booking Agent
            auto agent     = std::make_shared<BookingAgent>(name, contact, username, hashedPassword);
            return userBookingAgentService_.addUser(agent);
        }
        case Role::Passenger: { // Passenger
            auto passenger = std::make_shared<Passenger>(name, contact, username, hashedPassword, balance);
            return userPassengerService_.addUser(passenger);
        }
        default:
            return false; // Invalid role choice
    }
}

bool AirlineApplication::removeUser(const std::string& username) {
    // Attempt removal across each service until found and removed
    if (userPassengerService_.removeUser(username))    return true;
    if (userBookingAgentService_.removeUser(username)) return true;
    if (userAdminService_.removeUser(username))        return true;
    return false;
}

std::shared_ptr<User> AirlineApplication::getUserByUsername(const std::string& username) const {
    if (auto passenger = userPassengerService_.getUserByUsername(username))    return passenger;
    if (auto agent     = userBookingAgentService_.getUserByUsername(username)) return agent;
    if (auto admin     = userAdminService_.getUserByUsername(username))        return admin;
    return nullptr;
}

std::shared_ptr<User> AirlineApplication::getUserById(const std::string& id) const {
    if (auto passenger = userPassengerService_.getUserById(id))    return passenger;
    if (auto agent     = userBookingAgentService_.getUserById(id)) return agent;
    if (auto admin     = userAdminService_.getUserById(id))        return admin;
    return nullptr;
}

std::vector<std::shared_ptr<User>> AirlineApplication::getAllUsers() const {
    auto passengers = userPassengerService_.getAllUsers();
    auto agents     = userBookingAgentService_.getAllUsers();
    auto admins     = userAdminService_.getAllUsers();

    std::vector<std::shared_ptr<User>> allUsers;
    
    // 1. Single memory allocation to eliminate vector reallocations
    allUsers.reserve(passengers.size() + agents.size() + admins.size());

    // 2. Move smart pointers to avoid atomic ref-count increments
    allUsers.insert(allUsers.end(), 
                   std::make_move_iterator(passengers.begin()), 
                   std::make_move_iterator(passengers.end()));

    allUsers.insert(allUsers.end(), 
                   std::make_move_iterator(agents.begin()), 
                   std::make_move_iterator(agents.end()));

    allUsers.insert(allUsers.end(), 
                   std::make_move_iterator(admins.begin()), 
                   std::make_move_iterator(admins.end()));

    return allUsers;
}

std::shared_ptr<Passenger> AirlineApplication::getPassengerByUsername(const std::string& username) const {
    return userPassengerService_.getUserByUsername(username);
}

std::shared_ptr<BookingAgent> AirlineApplication::getAgentByUsername(const std::string& username) const {
    return userBookingAgentService_.getUserByUsername(username);
}

std::shared_ptr<Administrator> AirlineApplication::getAdminByUsername(const std::string& username) const {
    return userAdminService_.getUserByUsername(username);
}

}
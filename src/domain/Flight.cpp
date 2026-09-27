#include "domain/Flight.hpp"
#include "domain/Aircraft.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/CrewMember.hpp"
#include "domain/Passenger.hpp"
#include "domain/Pilot.hpp"
#include "domain/FlightAttendant.hpp"
#include <utility>
#include <algorithm> // Required for std::remove_if
#include <stdexcept> // for the getAircraft function


namespace airline {

Flight::Flight(std::string flightNumber, std::string origin, std::string destination,
               Date date, float duration, CrewRegulations reg)
    : flightNumber_(std::move(flightNumber)),
      origin_(std::move(origin)),
      destination_(std::move(destination)),
      date_(std::move(date)),
      duration_(duration),
      regulations_(std::move(reg)){}

// flight ends, this should be called (if it ever ended LOL)
Flight::~Flight(){
    auto ac = aircraft_.lock();
    if (ac) {
        ac->assignToFlight(false);
    }
        
}

const std::string& Flight::getFlightNumber() const { return flightNumber_; }
const std::string& Flight::getOrigin() const { return origin_; }
const std::string& Flight::getDestination() const { return destination_; }
std::shared_ptr<Aircraft> Flight::getAircraft() const { return getAircraftOrThrow(); }
Date Flight::getDate() const { return date_; }
float Flight::getDuration() const { return duration_; }
FlightStatus Flight::getStatus() const { return status_; }
const CrewRegulations& Flight::getRegulations() const {return regulations_;}
const std::vector<std::weak_ptr<FlightAttendant>> Flight::getFAs() const {return flightAtts_;}
    const std::vector<std::weak_ptr<Pilot>> Flight::getPilots() const {return pilots_;}

void Flight::setOrigin(std::string origin){ origin_ = origin; }
void Flight::setDestination(std::string destination) { destination_ = destination; }
void Flight::setDate(const Date& date) { date_ = date; }
void Flight::setDuration(float duration) { duration_ = duration; }
void Flight::setStatus(FlightStatus status) { status_ = status; }
void Flight::setAircraft(std::shared_ptr<Aircraft> aircraft) { aircraft_ = aircraft; }
bool Flight::setCrewMember(std::shared_ptr<CrewMember> crew){
    if (crew->getRole() == Role::Pilot){
        pilots_.push_back(std::dynamic_pointer_cast<Pilot>(crew)); // Stores weak_ptr internally
        return true;
    } else {
        flightAtts_.push_back(std::dynamic_pointer_cast<FlightAttendant>(crew)); // Stores weak_ptr internally
        return true;
    }

}

bool Flight::assignAircraft(std::shared_ptr<Aircraft> aircraft) {
    if (!aircraft) {
        return false;
    }
    if (aircraft->getMaintenanceStatus() == MaintenanceStatus::InMaintenance) {
        return false; 
    }

    aircraft->addRunningHours(duration_);
    aircraft->assignToFlight(true);
    aircraft_ = aircraft; // Automatically converts shared_ptr to weak_ptr
    return true;
}

bool Flight::isRegulationCompliant(std::shared_ptr<CrewMember> crewMember) {
    if (!crewMember) {
        return false;
    }
    return crewMember->getFlightHours() >= this->regulations_.minFlightHours;
}


bool Flight::assignCrewMember(std::shared_ptr<CrewMember> crew){
    if (!crew || !isRegulationCompliant(crew)) {
        return false;
    }

    if (crew->getRole() == Role::Pilot){
        pilots_.push_back(std::dynamic_pointer_cast<Pilot>(crew)); // Stores weak_ptr internally
        crew->addFlightHours(this->duration_);
        return true;
    } else {
        flightAtts_.push_back(std::dynamic_pointer_cast<FlightAttendant>(crew)); // Stores weak_ptr internally
        crew->addFlightHours(this->duration_);
        return true;
    }

}

bool Flight::removeCrewMember(const std::string& licenseId) {
    auto newEndPilot = std::remove_if(pilots_.begin(), pilots_.end(),
        [&licenseId](const auto& weakMember) {
            auto member = weakMember.lock();
            return member && member->getLicenseId() == licenseId;
        });

    if (newEndPilot == pilots_.end()) {
        return false;
    }

    pilots_.erase(newEndPilot, pilots_.end());


    auto newEndFA = std::remove_if(flightAtts_.begin(), flightAtts_.end(),
        [&licenseId](const auto& weakMember) {
            auto member = weakMember.lock();
            return member && member->getLicenseId() == licenseId;
        });

    if (newEndFA == flightAtts_.end()) {
        return false;
    }

    flightAtts_.erase(newEndFA, flightAtts_.end());

    return true;
}


std::shared_ptr<Aircraft> Flight::getAircraftOrThrow() const {
    auto ac = aircraft_.lock();
    if (!ac) {
        throw std::runtime_error("Flight " + flightNumber_ + " has no aircraft assigned");
    }
    return ac;
}

int Flight::getFirstClassCapacity() const { return getAircraftOrThrow()->getFirstClassCapacity(); }
int Flight::getBusinessClassCapacity() const { return getAircraftOrThrow()->getBusinessClassCapacity(); }
int Flight::getEconomyClassCapacity() const { return getAircraftOrThrow()->getEconomyClassCapacity(); }

int Flight::getOccupiedFirstClass() const { return getAircraftOrThrow()->getOccupiedFirstClass(); }
int Flight::getOccupiedBusinessClass() const { return getAircraftOrThrow()->getOccupiedBusinessClass(); }
int Flight::getOccupiedEconomyClass() const { return getAircraftOrThrow()->getOccupiedEconomyClass(); }

std::tuple<int, int, int> Flight::getAvailableSeatsPerClass() const{
    return getAircraftOrThrow()->getAvailableSeatsPerClass();
}

const std::unordered_map<SeatId_t, SeatData>& Flight::getAllSeats() const{
    return getAircraftOrThrow()->getAllSeats();
}

const std::shared_ptr<SeatId_t> Flight::findSeat(SeatClass seatClass ) const {
    return getAircraftOrThrow()->findSeat(seatClass);
}

bool Flight::assignSeat(SeatClass seatClass, std::shared_ptr<Passenger> passenger) {
    return getAircraftOrThrow()->assignSeat(seatClass, std::move(passenger));
}

bool Flight::assignSeat(const SeatId_t& id, std::shared_ptr<Passenger> passenger){
        return getAircraftOrThrow()->assignSeat(id, std::move(passenger));
}

std::shared_ptr<Passenger> Flight::freeSeat(const SeatId_t& id) {
    return getAircraftOrThrow()->freeSeat(id);
}

}  // namespace airline

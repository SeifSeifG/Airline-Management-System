#include "domain/Flight.hpp"
#include "domain/Aircraft.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/CrewMember.hpp"
#include "domain/Passenger.hpp"
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

const std::string& Flight::getFlightNumber() const { return flightNumber_; }
const std::string& Flight::getOrigin() const { return origin_; }
const std::string& Flight::getDestination() const { return destination_; }
Date Flight::getDate() const { return date_; }
float Flight::getDuration() const { return duration_; }
FlightStatus Flight::getStatus() const { return status_; }


void Flight::setStatus(FlightStatus status) { status_ = status; }

bool Flight::assignAircraft(std::shared_ptr<Aircraft> aircraft) {
    if (!aircraft) {
        return false;
    }
    if (aircraft->getMaintenanceStatus() == MaintenanceStatus::InMaintenance) {
        return false; 
    }
    aircraft->addRunningHours(duration_);
    aircraft_ = aircraft; // Automatically converts shared_ptr to weak_ptr
    return true;
}

bool Flight::isRegulationCompliant(std::shared_ptr<CrewMember> crewMember) {
    if (!crewMember) {
        return false;
    }
    return crewMember->getFlightHours() >= this->regulations_.minFlightHours;
}

bool Flight::assignCrew(std::shared_ptr<CrewMember> crewMember) {
    if (!crewMember || isRegulationCompliant(crewMember)) {
        return false;
    }
    crew_.push_back(crewMember); // Stores weak_ptr internally
    crewMember->addFlightHours(this->duration_);
    return true;
}

bool Flight::removeCrewMember(const std::string& licenseId) {
    auto newEnd = std::remove_if(crew_.begin(), crew_.end(),
        [&licenseId](const auto& weakMember) {
            auto member = weakMember.lock();
            return member && member->getLicenseId() == licenseId;
        });

    if (newEnd == crew_.end()) {
        return false;
    }

    crew_.erase(newEnd, crew_.end());
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

const std::unordered_map<SeatId_t, SeatData>& Flight::getAllSeats() const{
    return getAircraftOrThrow()->getAllSeats();
}

const std::shared_ptr<SeatId_t> Flight::findSeat(SeatClass seatClass, SeatPosition position) const {
    return getAircraftOrThrow()->findSeat(seatClass, position);
}

bool Flight::assignSeat(SeatClass seatClass, SeatPosition position, std::shared_ptr<Passenger> passenger) {
    return getAircraftOrThrow()->assignSeat(seatClass, position, std::move(passenger));
}

std::shared_ptr<Passenger> Flight::freeSeat(const SeatId_t& id) {
    return getAircraftOrThrow()->freeSeat(id);
}

}  // namespace airline

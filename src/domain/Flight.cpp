#include "domain/Flight.hpp"
#include "domain/Aircraft.hpp"
#include "domain/CrewMember.hpp"
#include <utility>

namespace airline {

Flight::Flight(std::string flightNumber, std::string origin, std::string destination,
               std::shared_ptr<Aircraft> aircraft, SeatMap seatMap)
    : flightNumber_(std::move(flightNumber)),
      origin_(std::move(origin)),
      destination_(std::move(destination)),
      aircraft_(std::move(aircraft)),
      seatMap_(std::move(seatMap)) {}

const std::string& Flight::getFlightNumber() const { return flightNumber_; }
const std::string& Flight::getOrigin() const { return origin_; }
const std::string& Flight::getDestination() const { return destination_; }
FlightStatus Flight::getStatus() const { return status_; }
void Flight::setStatus(FlightStatus status) { status_ = status; }

void Flight::assignCrew(std::shared_ptr<CrewMember> crewMember) {
    crew_.push_back(std::move(crewMember));
}

const std::vector<std::shared_ptr<CrewMember>>& Flight::getCrew() const { return crew_; }

SeatMap& Flight::getSeatMap() { return seatMap_; }

}  // namespace airline

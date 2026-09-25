#include "persistence/FlightRepo.hpp"

namespace airline {

std::vector<std::shared_ptr<Flight>> FlightRepository::findAvailableFlights(
    const std::string& origin, const std::string& destination, const Date& requestedDateTime) const {
    return this->findAll([&](const Flight& f) {
        return f.getOrigin() == origin &&
               f.getDestination() == destination &&
               (f.getDate() == requestedDateTime) &&
               f.getStatus() == FlightStatus::Scheduled;  // don't offer departed/cancelled flights
    });
}

}  // namespace airline
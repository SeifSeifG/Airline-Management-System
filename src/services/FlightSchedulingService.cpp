#include "services/FlightSchedulingService.hpp"

namespace airline {

FlightSchedulingService::FlightSchedulingService(FlightRepository& flightRepo,
                                                 AircraftRepository& aircraftRepo)
    : flightRepo_(flightRepo), aircraftRepo_(aircraftRepo) {}

bool FlightSchedulingService::addFlight(const std::string& flightNumber,
                                        const std::string& origin,
                                        const std::string& destination,
                                        const std::string& depTime,
                                        std::shared_ptr<Aircraft> ac,
                                        float duration,
                                        const CrewRegulations& reg,
                                        int basePrice) 
{
    if (!ac || ac->isAssigned()) {
        return false;
    }

    if (flightRepo_.get(flightNumber) != nullptr){
        return false;
    }

    Date depDate{depTime};

    auto flight = std::make_shared<Flight>(
        flightNumber, origin, destination, depDate, duration, basePrice, reg
    );

    if (!flight->assignAircraft(ac)) {
        return false;
    }

    flightRepo_.add(flightNumber, std::move(flight));
    return true;
}

bool FlightSchedulingService::removeFlight(const std::string& flightNumber) {
    auto flight = flightRepo_.get(flightNumber);
    if (!flight) {
        return false;
    }

    // Unassign aircraft before removal to free it
    if (auto ac = flight->getAircraft()) {
        ac->assignToFlight(false);
    }

    return flightRepo_.remove(flightNumber);
}

std::shared_ptr<Flight> FlightSchedulingService::getFlightById(const std::string& flightNumber) const {
    return flightRepo_.get(flightNumber);
}

std::vector<std::shared_ptr<Flight>> FlightSchedulingService::getAllFlights() const {
    return flightRepo_.getAll();
}

} // namespace airline
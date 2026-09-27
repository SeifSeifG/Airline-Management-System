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
                                        const CrewRegulations& reg) 
{
    if (!ac || ac->isAssigned()) {
        return false;
    }

    Date depDate{depTime};

    auto flight = std::make_shared<Flight>(
        flightNumber, origin, destination, depDate, duration, reg
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

std::vector<std::shared_ptr<Aircraft>> FlightSchedulingService::getAvailableAircrafts() const {
    std::vector<std::shared_ptr<Aircraft>> available;
    for (const auto& ac : aircraftRepo_.getAll()) {
        if (ac && !ac->isAssigned()) {
            available.push_back(ac);
        }
    }
    return available;
}

std::shared_ptr<Aircraft> FlightSchedulingService::getAircraftByTailNumber(const std::string& tailNumber) const {
    return aircraftRepo_.get(tailNumber);
}

} // namespace airline
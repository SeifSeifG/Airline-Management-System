#include "services/FlightManagementService.hpp"

namespace airline {

FlightManagementService::FlightManagementService(FlightRepository& flightRepo,
                                                 AircraftRepository& aircraftRepo,
                                                BookingService& bookingService)
    : flightRepo_(flightRepo), aircraftRepo_(aircraftRepo), bookingService_(bookingService) {}

bool FlightManagementService::addFlight(const std::string& flightNumber,
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

bool FlightManagementService::removeFlight(const std::string& flightNumber) {
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

std::shared_ptr<Flight> FlightManagementService::getFlightById(const std::string& flightNumber) const {
    return flightRepo_.get(flightNumber);
}

std::vector<std::shared_ptr<Flight>> FlightManagementService::getAllFlights() const {
    return flightRepo_.getAll();
}

DepartResult FlightManagementService::departFlight(const std::string& flightNumber) {
    // 1. Retrieve Flight
    auto flight = flightRepo_.get(flightNumber);
    if (!flight) {
        return DepartResult::FlightNotFound;
    }
    
    // 2. Check Aircraft Airworthiness
    auto aircraft = flight->getAircraft();
    if (!aircraft) {
        return DepartResult::AircraftNotFound;
    }

    if (aircraft->getMaintenanceStatus() != MaintenanceStatus::Airworthy) {
        return DepartResult::AircraftNotAirworthy;
    }

    // 3. Set Flight Status to Departed
    flight->setStatus(FlightStatus::Departed);

    // 4. Delegate Request Resolution via BookingService API
    bookingService_.resolveRequestsForDepartedFlight(flight);

    return DepartResult::Success;
}
} // namespace airline
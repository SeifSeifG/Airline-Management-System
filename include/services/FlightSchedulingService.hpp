#pragma once
#include <memory>
#include <string>
#include <vector>

#include "domain/Aircraft.hpp"
#include "domain/Flight.hpp"
#include "persistence/AircraftRepo.hpp"
#include "persistence/FlightRepo.hpp"

namespace airline {

class FlightSchedulingService {
private:
    FlightRepository& flightRepo_;
    AircraftRepository& aircraftRepo_;

public:
    FlightSchedulingService(FlightRepository& flightRepo, AircraftRepository& aircraftRepo);

    bool addFlight(const std::string& flightNumber,
                   const std::string& origin,
                   const std::string& destination,
                   const std::string& depTime,
                   std::shared_ptr<Aircraft> ac,
                   float duration,
                   const CrewRegulations& reg);

    bool removeFlight(const std::string& flightNumber);

    std::shared_ptr<Flight> getFlightById(const std::string& flightNumber) const;
    std::vector<std::shared_ptr<Flight>> getAllFlights() const;

    std::vector<std::shared_ptr<Aircraft>> getAvailableAircrafts() const;
    std::shared_ptr<Aircraft> getAircraftByTailNumber(const std::string& tailNumber) const;
};

} // namespace airline
#pragma once
#include <memory>
#include <string>
#include <vector>

#include "domain/Aircraft.hpp"
#include "domain/Flight.hpp"
#include "persistence/AircraftRepo.hpp"
#include "persistence/FlightRepo.hpp"
#include "BookCheckInService.hpp"


namespace airline {

class FlightManagementService {
private:
    FlightRepository& flightRepo_;
    AircraftRepository& aircraftRepo_;
    BookingService& bookingService_;

public:
    FlightManagementService(FlightRepository& flightRepo, AircraftRepository& aircraftRepo,
                        BookingService& bookingService);

    bool addFlight(const std::string& flightNumber,
                const std::string& origin,
                const std::string& destination,
                const std::string& depTime,
                std::shared_ptr<Aircraft> ac,
                float duration,
                const CrewRegulations& reg,
                int basePrice);

    bool removeFlight(const std::string& flightNumber);

    std::shared_ptr<Flight> getFlightById(const std::string& flightNumber) const;
    std::vector<std::shared_ptr<Flight>> getAllFlights() const;
    DepartResult departFlight(const std::string& flightNumber);
};

} // namespace airline
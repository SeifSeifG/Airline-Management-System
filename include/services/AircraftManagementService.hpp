#pragma once
#include <memory>
#include <string>
#include <vector>

#include "domain/Aircraft.hpp"
#include "persistence/AircraftRepo.hpp"

namespace airline {

class AircraftManagementService {
private:
    AircraftRepository& aircraftRepo_;

public:
    explicit AircraftManagementService(AircraftRepository& aircraftRepo);

    bool addAircraft(const std::string& tailNumber,
                     const std::string& model,
                     const SeatLayout& seatLayout,
                     float maxRunningHours);

    bool removeAircraft(const std::string& tailNumber);

    bool updateMaintenanceStatus(const std::string& tailNumber, MaintenanceStatus status);

    std::shared_ptr<Aircraft> getAircraftByTailNumber(const std::string& tailNumber) const;
    std::vector<std::shared_ptr<Aircraft>> getAvailableAircrafts() const;
    std::vector<std::shared_ptr<Aircraft>> getAllAircraft() const;
};

} // namespace airline
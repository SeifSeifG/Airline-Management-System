#include "services/AircraftManagementService.hpp"

namespace airline {

AircraftManagementService::AircraftManagementService(AircraftRepository& aircraftRepo)
    : aircraftRepo_(aircraftRepo) {}

bool AircraftManagementService::addAircraft(const std::string& tailNumber,
                                            const std::string& model,
                                            const SeatLayout& seatLayout,
                                            float maxRunningHours) {
    if (aircraftRepo_.get(tailNumber) != nullptr) {
        return false;
    }

    auto newAircraft = std::make_shared<Aircraft>(
        tailNumber,
        model,
        maxRunningHours,
        seatLayout
    );

    aircraftRepo_.add(tailNumber, std::move(newAircraft));
    return true;
}

bool AircraftManagementService::removeAircraft(const std::string& tailNumber) {
    return aircraftRepo_.remove(tailNumber);
}

bool AircraftManagementService::updateMaintenanceStatus(const std::string& tailNumber, MaintenanceStatus status) {
    auto aircraft = aircraftRepo_.get(tailNumber);
    if (!aircraft) {
        return false;
    }
    aircraft->setMaintenanceStatus(status);
    return true;
}

std::shared_ptr<Aircraft> AircraftManagementService::getAircraftByTailNumber(const std::string& tailNumber) const {
    return aircraftRepo_.get(tailNumber);
}

std::vector<std::shared_ptr<Aircraft>> AircraftManagementService::getAvailableAircrafts() const {
    std::vector<std::shared_ptr<Aircraft>> available;
    for (const auto& ac : aircraftRepo_.getAll()) {
        if (ac && !ac->isAssigned() && ac->getMaintenanceStatus() == MaintenanceStatus::Airworthy) {
            available.push_back(ac);
        }
    }
    return available;
}

std::vector<std::shared_ptr<Aircraft>> AircraftManagementService::getAllAircraft() const {
    return aircraftRepo_.getAll();
}

} // namespace airline
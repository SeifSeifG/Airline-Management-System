#include "persistence/AircraftRepo.hpp"

namespace airline {

std::vector<std::shared_ptr<Aircraft>> AircraftRepository::findByMinFirstClassCapacity(int minCapacity) const {
    return this->findAll([minCapacity](const Aircraft& ac) {
        return ac.getFirstClassCapacity() >= minCapacity;
    });
}

std::vector<std::shared_ptr<Aircraft>> AircraftRepository::findByMinBusinessClassCapacity(int minCapacity) const {
    return this->findAll([minCapacity](const Aircraft& ac) {
        return ac.getBusinessClassCapacity() >= minCapacity;
    });
}

std::vector<std::shared_ptr<Aircraft>> AircraftRepository::findByMinEconomyClassCapacity(int minCapacity) const {
    return this->findAll([minCapacity](const Aircraft& ac) {
        return ac.getEconomyClassCapacity() >= minCapacity;
    });
}

std::vector<std::shared_ptr<Aircraft>> AircraftRepository::findAirworthy() const {
    return this->findAll([](const Aircraft& ac) {
        return ac.getMaintenanceStatus() == MaintenanceStatus::Airworthy;
    });
}

}  // namespace airline
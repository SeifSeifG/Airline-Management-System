#pragma once
#include "persistence/Repository.hpp"
#include "domain/Aircraft.hpp"

namespace airline {

class AircraftRepository : public Repository<Aircraft> {
public:
    std::vector<std::shared_ptr<Aircraft>> findByMinFirstClassCapacity(int minCapacity) const;
    std::vector<std::shared_ptr<Aircraft>> findByMinBusinessClassCapacity(int minCapacity) const;
    std::vector<std::shared_ptr<Aircraft>> findByMinEconomyClassCapacity(int minCapacity) const;
    std::vector<std::shared_ptr<Aircraft>> findAirworthy() const;
};

}  // namespace airline
#pragma once
#include "persistence/Repository.hpp"
#include "domain/CrewMember.hpp"

namespace airline {

class CrewRepository : public Repository<CrewMember> {
public:
    std::vector<std::shared_ptr<CrewMember>> findWithMinFlightHours(float threshold) const;
};

}  // namespace airline
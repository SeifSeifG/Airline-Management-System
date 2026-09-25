#include "persistence/CrewRepo.hpp"

namespace airline {

std::vector<std::shared_ptr<CrewMember>> CrewRepository::findWithMinFlightHours(float threshold) const {
    return this->findAll([threshold](const CrewMember& c) {
        return c.getFlightHours() >= threshold;
    });
}

}  // namespace airline
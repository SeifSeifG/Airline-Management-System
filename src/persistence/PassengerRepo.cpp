#include "persistence/PassengerRepo.hpp"

namespace airline {

std::vector<std::shared_ptr<Passenger>> PassengerRepository::findWithMinLoyaltyPoints(int minPoints) const {
    return findAll([minPoints](const Passenger& p) { return p.getLoyaltyBalance() >= minPoints; });
}

}  // namespace airline
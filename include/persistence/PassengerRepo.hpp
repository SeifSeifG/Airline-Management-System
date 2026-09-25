// persistence/PassengerRepository.hpp
#pragma once
#include "persistence/UserRepo.hpp"
#include "domain/Passenger.hpp"

namespace airline {

class PassengerRepository : public UserRepository<Passenger> {
public:
    std::vector<std::shared_ptr<Passenger>> findWithMinLoyaltyPoints(int minPoints) const;
};

} 
// persistence/FlightRepository.hpp
#pragma once
#include "persistence/Repository.hpp"
#include "domain/Flight.hpp"

namespace airline {

class FlightRepository : public Repository<Flight> {
public:
    std::vector<std::shared_ptr<Flight>> findAvailableFlights(const std::string& origin,
                                                                const std::string& destination,
                                                                const Date& requestedDateTime) const;
};

}  // namespace airline
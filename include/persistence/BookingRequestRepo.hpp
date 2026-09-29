#pragma once
#include "persistence/Repository.hpp"
#include "domain/BookingRequest.hpp"

namespace airline {

class BookingRequestRepository : public Repository<BookingRequest> {
public:
    std::vector<std::shared_ptr<BookingRequest>> findByPassenger(const std::string& passengerId) const;
    std::vector<std::shared_ptr<BookingRequest>> findByFlight(const std::string& flightId) const;
};

}  // namespace airline
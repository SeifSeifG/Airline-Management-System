#include "persistence/BookingRequestRepo.hpp"

namespace airline {

std::vector<std::shared_ptr<BookingRequest>> BookingRequestRepository::findByPassenger(const std::string& passengerId) const {
    return this->findAll([&passengerId](const BookingRequest& r) { return r.passengerId == passengerId; });
}

std::vector<std::shared_ptr<BookingRequest>> BookingRequestRepository::findByFlight(const std::string& flightId) const {
    return this->findAll([&flightId](const BookingRequest& r) { return r.flightId == flightId; });
}

}  // namespace airline
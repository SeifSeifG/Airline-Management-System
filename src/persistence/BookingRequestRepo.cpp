#include "persistence/BookingRequestRepo.hpp"
#include "domain/BookingRequest.hpp"

namespace airline {

std::vector<std::shared_ptr<BookingRequest>> BookingRequestRepository::findByPassenger(const std::string& passengerId) const {
    return this->findAll([&passengerId](const BookingRequest& r) {
        auto p = r.passenger.lock();
        return p && p->getId() == passengerId;
    });
}

std::vector<std::shared_ptr<BookingRequest>> BookingRequestRepository::findByFlight(const std::string& flightId) const {
    return this->findAll([&flightId](const BookingRequest& r) {
        auto f = r.flight.lock();
        return f && (f->getFlightNumber() == flightId);
    });
}

}  // namespace airline
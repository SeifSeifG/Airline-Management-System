#include "domain/Reservation.hpp"
#include "domain/Payment.hpp"
#include <utility>

namespace airline {

Reservation::Reservation(std::string id, std::shared_ptr<Flight> flight,
                          std::shared_ptr<Passenger> passenger)
    : id_(std::move(id)), flight_(std::move(flight)), passenger_(std::move(passenger)) {}

Reservation::~Reservation() = default;

const std::string& Reservation::getId() const { return id_; }
ReservationStatus Reservation::getStatus() const { return status_; }

void Reservation::cancel() { status_ = ReservationStatus::Cancelled; }
void Reservation::confirm() { status_ = ReservationStatus::Confirmed; }

void Reservation::attachPayment(std::unique_ptr<Payment> payment) {
    payment_ = std::move(payment);
}

}  // namespace airline

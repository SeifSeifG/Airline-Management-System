#pragma once
#include "domain/Enums.hpp"
#include <memory>
#include <string>
#include <vector>

namespace airline {

class Flight;
class Passenger;
class Payment;

class Reservation {
public:
    Reservation(std::string id, std::shared_ptr<Flight> flight,
                std::shared_ptr<Passenger> passenger);

    // Declared explicitly (not defaulted here) because Payment is only
    // forward-declared -- the unique_ptr member needs the complete type,
    // which is visible where this is defined, in the .cpp.
    ~Reservation();

    const std::string& getId() const;
    ReservationStatus getStatus() const;

    void cancel();
    void confirm();

    // Composition: a Reservation owns its Payment outright.
    void attachPayment(std::unique_ptr<Payment> payment);

private:
    std::string id_;
    std::shared_ptr<Flight> flight_;
    std::shared_ptr<Passenger> passenger_;
    std::vector<std::size_t> seatIndices_;
    std::unique_ptr<Payment> payment_;
    ReservationStatus status_ = ReservationStatus::Pending;
};

}  // namespace airline

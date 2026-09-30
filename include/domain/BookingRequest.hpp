// domain/BookingRequest.hpp
#pragma once
#include "domain/Defs.hpp"
#include "domain/Passenger.hpp"
#include "domain/Flight.hpp"
#include <string>

namespace airline {

struct BookingRequest {
    std::string id;
    std::weak_ptr<Passenger> passenger;
    std::weak_ptr<Flight> flight;
    SeatClass seatClass{SeatClass::Economy};
    int price{0};
    ReservationStatus status{ReservationStatus::PendingCheckIn};
};

inline std::ostream& operator<<(std::ostream& os, const BookingRequest& req) {
    auto p = req.passenger.lock();
    auto f = req.flight.lock();

    os << "Request ID: " << req.id
       << " | Name: " << (p ? p->getName() : "N/A")
       << " | Flight: " << (f ? f->getFlightNumber() : "N/A")
       << " | Class: " << toString(req.seatClass)
       << " | Price: $" << req.price
       << " | Status: " << toString(req.status);
    return os;
}

struct CheckInRequest {
    std::string id;
    std::weak_ptr<Passenger> passenger;
    std::weak_ptr<Flight> flight;
    SeatClass seatClass{SeatClass::Economy};
    int price{0};
    ReservationStatus status{ReservationStatus::PendingBook};
};

inline std::ostream& operator<<(std::ostream& os, const CheckInRequest& req) {
    auto p = req.passenger.lock();
    auto f = req.flight.lock();

    os << "CheckIn ID: " << req.id
       << " | Name: " << (p ? p->getName() : "N/A")
       << " | Flight: " << (f ? f->getFlightNumber() : "N/A")
       << " | Class: " << toString(req.seatClass)
       << " | Price: $" << req.price
       << " | Status: " << toString(req.status);
    return os;
}

struct FinishedRequest {
    std::string checkInId;     // Original Check-In ID (e.g., CR-1)

    // Historical Snapshot Data (for immutable operational reporting)
    std::string passengerId;
    std::string passengerName;
    std::string flightNumber;
    std::string origin;
    std::string destination;
    std::string departureDate;
    SeatClass seatClass{SeatClass::Economy};
    int price{0};

    ReservationStatus reservationStatus{ReservationStatus::Confirmed};
    PaymentStatus paymentStatus{PaymentStatus::Completed};
};

inline std::ostream& operator<<(std::ostream& os, const FinishedRequest& req) {
    os << "CheckIn ID: " << req.checkInId
       << " | Passenger: " << req.passengerName << " (" << req.passengerId << ")"
       << " | Flight: " << req.flightNumber << " (" << req.origin << " -> " << req.destination << ")"
       << " | Departure: " << req.departureDate
       << " | Class: " << toString(req.seatClass)
       << " | Price: $" << req.price
       << " | Status: " << toString(req.reservationStatus);
    return os;
}

struct RequestReply{
    bool idFound = true;
    PaymentStatus payStatus = PaymentStatus::Completed;
};

}  // namespace airline
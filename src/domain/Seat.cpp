#include "domain/Seat.hpp"
#include <utility>

namespace airline {

Seat::Seat(std::string seatId, SeatClass seatClass)
    : seatId_(std::move(seatId)), seatClass_(seatClass) {}

const std::string& Seat::getSeatId() const { return seatId_; }
SeatClass Seat::getSeatClass() const { return seatClass_; }
bool Seat::isOccupied() const { return occupied_; }
void Seat::setOccupied(bool occupied) { occupied_ = occupied; }

}  // namespace airline

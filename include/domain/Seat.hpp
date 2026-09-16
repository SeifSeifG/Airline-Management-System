#pragma once
#include "domain/Enums.hpp"
#include <string>

namespace airline {

class Seat {
public:
    Seat(std::string seatId, SeatClass seatClass);

    const std::string& getSeatId() const;
    SeatClass getSeatClass() const;
    bool isOccupied() const;
    void setOccupied(bool occupied);

private:
    std::string seatId_;
    SeatClass seatClass_;
    bool occupied_ = false;
};

}  // namespace airline

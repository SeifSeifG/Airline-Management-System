// this file DOESN't have a .cpp
#pragma once
#include "domain/Defs.hpp"
#include <string>

namespace airline {

struct BookingRequest {
    std::string passengerId;
    std::string origin;
    std::string destination;
    Date requestedDate;
    SeatClass desiredClass;
    SeatPosition desiredPosition;
};

}  // namespace airline
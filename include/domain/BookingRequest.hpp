// domain/BookingRequest.hpp
#pragma once
#include "domain/Defs.hpp"
#include <string>

namespace airline {

struct BookingRequest {
    std::string requestId;
    std::string passengerId;
    std::string flightId;        // Flight::flightNumber_ IS the id -- no new field needed on Flight
    SeatClass desiredClass;
    SeatPosition desiredPosition;
};

}  // namespace airline
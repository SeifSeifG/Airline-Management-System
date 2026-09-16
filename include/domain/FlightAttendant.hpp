#pragma once
#include "domain/CrewMember.hpp"

namespace airline {

class FlightAttendant : public CrewMember {
public:
    using CrewMember::CrewMember;
    float maxFlightHours() const override;
};

}  // namespace airline

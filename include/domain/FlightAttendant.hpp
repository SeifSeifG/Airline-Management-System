#pragma once
#include "domain/CrewMember.hpp"

namespace airline {

class FlightAttendant : public CrewMember {
public:
    using CrewMember::CrewMember;

    Role getRole() const override;
};

}  // namespace airline

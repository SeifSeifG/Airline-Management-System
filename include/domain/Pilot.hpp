#pragma once
#include "domain/CrewMember.hpp"

namespace airline {

class Pilot : public CrewMember {
public:
    using CrewMember::CrewMember;

    Role getRole() const override;
};

}  // namespace airline

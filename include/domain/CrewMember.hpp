#pragma once
#include "domain/Person.hpp"

namespace airline {

// Separate hierarchy from User: crew have an identity and flight-hour limits
// to enforce, but (per this spec) don't authenticate into the system.
class CrewMember : public Person {
public:
    CrewMember(std::string id, std::string name, std::string contactInfo,
               std::string licenseId);
    ~CrewMember() override = default;

    const std::string& getLicenseId() const;
    float getFlightHours() const;
    void addFlightHours(float hours);

    // Regulatory ceiling differs by role -- see Pilot / FlightAttendant.
    virtual float maxFlightHours() const = 0;

protected:
    std::string licenseId_;
    float flightHours_ = 0.0f;
};

}  // namespace airline

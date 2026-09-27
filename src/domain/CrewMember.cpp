#include "domain/CrewMember.hpp"
#include <utility>

namespace airline {

CrewMember::CrewMember(std::string id, std::string name, contactInfo contactInfo,
                        std::string licenseId, float minFlightHrs)
    : Person(std::move(id), std::move(name), std::move(contactInfo)),
      licenseId_(std::move(licenseId)),
      flightHours_(std::move(minFlightHrs)) {}

const std::string& CrewMember::getLicenseId() const { return licenseId_; }
float CrewMember::getFlightHours() const { return flightHours_; }

void CrewMember::addFlightHours(float hours) {
    flightHours_ += hours;
}

}  // namespace airline

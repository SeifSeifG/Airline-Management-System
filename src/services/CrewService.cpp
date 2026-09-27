#include "services/CrewService.hpp"

namespace airline {

CrewService::CrewService(CrewRepository& pilots, CrewRepository& flightAtts)
    : pilots_(pilots), flightAtts_(flightAtts) {}

std::vector<std::shared_ptr<Pilot>> CrewService::getAvailablePilots() const {
    std::vector<std::shared_ptr<Pilot>> available;
    for (const auto& member : pilots_.getAll()) {
        if (auto pilot = std::dynamic_pointer_cast<Pilot>(member)) {
            available.push_back(pilot);
        }
    }
    return available;
}

std::shared_ptr<Pilot> CrewService::getPilotById(const std::string& id) const {
    return std::dynamic_pointer_cast<Pilot>(pilots_.get(id));
}

std::vector<std::shared_ptr<FlightAttendant>> CrewService::getAvailableFlightAttendants() const {
    std::vector<std::shared_ptr<FlightAttendant>> available;
    for (const auto& member : flightAtts_.getAll()) {
        if (auto fa = std::dynamic_pointer_cast<FlightAttendant>(member)) {
            available.push_back(fa);
        }
    }
    return available;
}

std::shared_ptr<FlightAttendant> CrewService::getFAById(const std::string& id) const {
    return std::dynamic_pointer_cast<FlightAttendant>(flightAtts_.get(id));
}

bool CrewService::assignCrewMember(const std::shared_ptr<Flight>& flight,
                                   const std::shared_ptr<CrewMember>& crewMember) 
{
    if (!flight || !crewMember) {
        return false;
    }
    return flight->assignCrewMember(crewMember);
}

} // namespace airline
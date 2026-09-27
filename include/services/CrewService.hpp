#pragma once
#include <memory>
#include <string>
#include <vector>

#include "domain/Flight.hpp"
#include "domain/FlightAttendant.hpp"
#include "domain/Pilot.hpp"
#include "persistence/CrewRepo.hpp"

namespace airline {

class CrewService {
private:
    CrewRepository& pilots_;
    CrewRepository& flightAtts_;

public:
    CrewService(CrewRepository& pilots, CrewRepository& flightAtts);

    std::vector<std::shared_ptr<Pilot>> getAvailablePilots() const;
    std::shared_ptr<Pilot> getPilotById(const std::string& id) const;

    std::vector<std::shared_ptr<FlightAttendant>> getAvailableFlightAttendants() const;
    std::shared_ptr<FlightAttendant> getFAById(const std::string& id) const;

    bool assignCrewMember(const std::shared_ptr<Flight>& flight,
                          const std::shared_ptr<CrewMember>& crewMember);
};

} // namespace airline
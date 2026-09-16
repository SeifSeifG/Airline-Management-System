#pragma once
#include "domain/Enums.hpp"
#include "domain/SeatMap.hpp"
#include <memory>
#include <string>
#include <vector>

namespace airline {

class Aircraft;
class CrewMember;

class Flight {
public:
    Flight(std::string flightNumber, std::string origin, std::string destination,
           std::shared_ptr<Aircraft> aircraft, SeatMap seatMap);

    const std::string& getFlightNumber() const;
    const std::string& getOrigin() const;
    const std::string& getDestination() const;
    FlightStatus getStatus() const;
    void setStatus(FlightStatus status);

    // Aggregation: the crew member is not owned by the flight, just
    // referenced -- they get reassigned to other flights independently.
    void assignCrew(std::shared_ptr<CrewMember> crewMember);
    const std::vector<std::shared_ptr<CrewMember>>& getCrew() const;

    SeatMap& getSeatMap();

private:
    std::string flightNumber_;
    std::string origin_;
    std::string destination_;
    std::shared_ptr<Aircraft> aircraft_;
    SeatMap seatMap_;
    std::vector<std::shared_ptr<CrewMember>> crew_;
    FlightStatus status_ = FlightStatus::Scheduled;
};

}  // namespace airline

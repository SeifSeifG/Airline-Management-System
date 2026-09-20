#pragma once
#include "domain/Aircraft.hpp"
#include <memory>
#include <string>
#include <vector>

namespace airline {

struct CrewRegulations{
    float minFlightHours; 
    CrewRegulations(float m) : minFlightHours(m) {} 
};

class Aircraft;
class CrewMember;
class SeatLayout;
class Passenger;

class Flight {
private:
    std::string flightNumber_;
    std::string origin_;
    std::string destination_;
    Date date_;
    float duration_; // Duration in hours
    
    // Non-owning weak references
    std::weak_ptr<Aircraft> aircraft_;
    std::vector<std::weak_ptr<CrewMember>> crew_;
    
    FlightStatus status_ = FlightStatus::Scheduled;
    CrewRegulations regulations_;

    bool isRegulationCompliant(std::shared_ptr<CrewMember> crewMember);
    std::shared_ptr<Aircraft> getAircraftOrThrow() const;
public:
    Flight(std::string flightNumber, std::string origin, std::string destination,
           Date date, float duration, CrewRegulations reg);

    const std::string& getFlightNumber() const;
    const std::string& getOrigin() const;
    const std::string& getDestination() const;
    Date getDate() const;
    float getDuration() const;
    FlightStatus getStatus() const;
    void setStatus(FlightStatus status);

    // Aggregation: Input shared_ptrs are stored internally as weak_ptrs
    bool assignAircraft(std::shared_ptr<Aircraft> aircraft);
    bool assignCrew(std::shared_ptr<CrewMember> crewMember);
    bool removeCrewMember(const std::string& licenseId);

    // Tier capacity getters
    int getFirstClassCapacity() const;
    int getBusinessClassCapacity() const;
    int getEconomyClassCapacity() const;

    // Occupied count getters
    int getOccupiedFirstClass() const;
    int getOccupiedBusinessClass() const;
    int getOccupiedEconomyClass() const;

    // SeatLayout wrappers
    const std::unordered_map<SeatId_t, SeatData>& getAllSeats() const;
    const std::shared_ptr<SeatId_t> findSeat(SeatClass seatClass, SeatPosition position) const;
    bool assignSeat(SeatClass seatClass, SeatPosition position, std::shared_ptr<Passenger> passenger);
    std::shared_ptr<Passenger> freeSeat(const SeatId_t& id );
};

}  // namespace airline

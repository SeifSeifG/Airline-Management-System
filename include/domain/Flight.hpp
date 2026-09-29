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
class FlightAttendant;
class Pilot;
class SeatLayout;
class Passenger;

class Flight {
private:
    std::string flightNumber_;
    std::string origin_;
    std::string destination_;
    Date date_;
    float duration_; // Duration in hours
    int basePrice_; // price per economy class, others are multiples of it
    
    // Non-owning weak references
    std::weak_ptr<Aircraft> aircraft_;
    std::vector<std::weak_ptr<FlightAttendant>> flightAtts_;
    std::vector<std::weak_ptr<Pilot>> pilots_;
    
    CrewRegulations regulations_;
    FlightStatus status_ = FlightStatus::Scheduled;

    bool isRegulationCompliant(std::shared_ptr<CrewMember> crewMember);
    std::shared_ptr<Aircraft> getAircraftOrThrow() const;
public:
    Flight(std::string flightNumber, std::string origin, std::string destination,
           Date date, float duration, int basePrice, CrewRegulations reg);
    ~Flight();

    const std::string& getFlightNumber() const;
    const std::string& getOrigin() const;
    const std::string& getDestination() const;
    std::shared_ptr<Aircraft> getAircraft() const;
    Date getDate() const;
    float getDuration() const;
    FlightStatus getStatus() const;
    const CrewRegulations& getRegulations() const;
    const std::vector<std::weak_ptr<FlightAttendant>> getFAs() const;
    const std::vector<std::weak_ptr<Pilot>> getPilots() const;

    void setOrigin(std::string origin);
    void setDestination(std::string destination);
    void setDate(const Date& date);
    void setDuration(float duration);
    void setStatus(FlightStatus status);
    void setBasePrice(int price);
    void setAircraft(std::shared_ptr<Aircraft> aircraft); // used in load
    bool setCrewMember(std::shared_ptr<CrewMember> crew); // used in load


    // Aggregation: Input shared_ptrs are stored internally as weak_ptrs
    bool assignAircraft(std::shared_ptr<Aircraft> aircraft);
    bool assignCrewMember(std::shared_ptr<CrewMember> crew);
    bool removeCrewMember(const std::string& licenseId);

    // Tier capacity getters
    int getFirstClassCapacity() const;
    int getBusinessClassCapacity() const;
    int getEconomyClassCapacity() const;

    // Occupied count getters
    int getOccupiedFirstClass() const;
    int getOccupiedBusinessClass() const;
    int getOccupiedEconomyClass() const;

    // price getter
    int getPriceByClass(SeatClass seatClass) const ;

    // available seats in all classes
    std::tuple<int, int, int> getAvailableSeatsPerClass() const;

    // SeatLayout wrappers
    const std::unordered_map<SeatId_t, SeatData>& getAllSeats() const;
    const std::shared_ptr<SeatId_t> findSeat(SeatClass seatClass) const;
    bool assignSeat(SeatClass seatClass, std::shared_ptr<Passenger> passenger);
    bool assignSeat(const SeatId_t& id, std::shared_ptr<Passenger> passenger);
    std::shared_ptr<Passenger> freeSeat(const SeatId_t& id );
};

}  // namespace airline

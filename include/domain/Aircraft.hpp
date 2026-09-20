#pragma once
#include "domain/SeatLayout.hpp"
#include <string>
#include <vector>

namespace airline {

class Aircraft {
private:
    std::string tailNumber_;
    std::string model_;
    SeatLayout seatLayout_;
    float maxRunningHours_ ;
    
    float runningHours_ = 0.0f; // Total flight hours since last maintenance
    MaintenanceStatus status_ = MaintenanceStatus::Airworthy;
public:
    Aircraft(std::string tailNumber, std::string model, float maxRunningHours, SeatLayout seatMap);

    const std::string& getTailNumber() const;
    const std::string& getModel() const;

    // Tier capacity getters
    int getFirstClassCapacity() const;
    int getBusinessClassCapacity() const;
    int getEconomyClassCapacity() const;

    // Occupied count getters
    int getOccupiedFirstClass() const;
    int getOccupiedBusinessClass() const;
    int getOccupiedEconomyClass() const;

    // maintenance related functions
    MaintenanceStatus getMaintenanceStatus() const;
    float getRunningHours() const ;
    float getMaxRunningHours() const ;
    void addRunningHours(float hours);

    // SeatLayout wrappers
    const std::unordered_map<SeatId_t, SeatData>& getAllSeats() const;
    const std::shared_ptr<SeatId_t> findSeat(SeatClass seatClass, SeatPosition position) const;
    bool assignSeat(SeatClass seatClass, SeatPosition position, std::shared_ptr<Passenger> passenger);
    std::shared_ptr<Passenger> freeSeat(const SeatId_t& id );
};

}  // namespace airline

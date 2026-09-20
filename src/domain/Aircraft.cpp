#include "domain/Aircraft.hpp"
#include <utility>

namespace airline {


Aircraft::Aircraft(std::string tailNumber, std::string model, float maxRunningHours, SeatLayout seatMap) :
    tailNumber_(std::move(tailNumber)),
    model_(std::move(model)),
    maxRunningHours_(maxRunningHours),
    seatLayout_(std::move(seatMap)) {}


const std::string& Aircraft::getTailNumber() const { return tailNumber_; }
const std::string& Aircraft::getModel() const { return model_; }

// Tier capacity getters
int Aircraft::getFirstClassCapacity() const{ return seatLayout_.getFirstClassCapacity(); }
int Aircraft::getBusinessClassCapacity() const{ return seatLayout_.getBusinessClassCapacity(); }
int Aircraft::getEconomyClassCapacity() const{ return seatLayout_.getEconomyClassCapacity(); }

// Occupied count getters
int Aircraft::getOccupiedFirstClass() const{ return seatLayout_.getOccupiedFirstClass(); }
int Aircraft::getOccupiedBusinessClass() const{ return seatLayout_.getOccupiedBusinessClass(); }
int Aircraft::getOccupiedEconomyClass() const{ return seatLayout_.getOccupiedEconomyClass(); }

MaintenanceStatus Aircraft::getMaintenanceStatus() const { return runningHours_ >= maxRunningHours_ ? 
    MaintenanceStatus::InMaintenance : MaintenanceStatus::Airworthy; }
float Aircraft::getRunningHours() const { return runningHours_; }
float Aircraft::getMaxRunningHours() const { return maxRunningHours_; }
void Aircraft::addRunningHours(float hours) { runningHours_ += hours; }

const std::unordered_map<SeatId_t, SeatData>& Aircraft::getAllSeats() const {
    return seatLayout_.getAllSeats();
}

const std::shared_ptr<SeatId_t> Aircraft::findSeat(SeatClass seatClass, SeatPosition position) const {
    return seatLayout_.findSeat(seatClass, position);
}

bool Aircraft::assignSeat(SeatClass seatClass, SeatPosition position, std::shared_ptr<Passenger> passenger) {
    auto status = seatLayout_.assignSeat(seatClass, position, std::move(passenger));
    if (status.has_value()) {
        return true;
    } else{
        return false;
    }
}

std::shared_ptr<Passenger> Aircraft::freeSeat(const SeatId_t& id) {
    return seatLayout_.freeSeat(id);
}

}  // namespace airline

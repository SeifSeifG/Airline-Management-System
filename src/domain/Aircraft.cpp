#include "domain/Aircraft.hpp"
#include <utility>

namespace airline {


Aircraft::Aircraft(std::string tailNumber, std::string model, float maxRunningHours, SeatLayout seatMap) :
    tailNumber_(std::move(tailNumber)),
    model_(std::move(model)),
    seatLayout_(std::move(seatMap)),
    maxRunningHours_(maxRunningHours),
    runningHours_(0) {}

const std::string& Aircraft::getTailNumber() const { return tailNumber_; }
const std::string& Aircraft::getModel() const { return model_; }

// Tier capacity getters
int Aircraft::getFirstClassCapacity() const{ return seatLayout_.getFirstClassCapacity(); }
int Aircraft::getBusinessClassCapacity() const{ return seatLayout_.getBusinessClassCapacity(); }
int Aircraft::getEconomyClassCapacity() const{ return seatLayout_.getEconomyClassCapacity(); }
int Aircraft::getTotalCapacity() const{ 
    return  this->getFirstClassCapacity() + 
            this-> getBusinessClassCapacity() + 
            this-> getEconomyClassCapacity();
}


// Occupied count getters
int Aircraft::getOccupiedFirstClass() const{ return seatLayout_.getOccupiedFirstClass(); }
int Aircraft::getOccupiedBusinessClass() const{ return seatLayout_.getOccupiedBusinessClass(); }
int Aircraft::getOccupiedEconomyClass() const{ return seatLayout_.getOccupiedEconomyClass(); }

std::tuple<int, int, int> Aircraft::getAvailableSeatsPerClass() const{
    return seatLayout_.getAvailableSeatsPerClass();
}



MaintenanceStatus Aircraft::getMaintenanceStatus() const { return runningHours_ >= maxRunningHours_ ? 
    MaintenanceStatus::InMaintenance : MaintenanceStatus::Airworthy; }
float Aircraft::getRunningHours() const { return runningHours_; }
float Aircraft::getMaxRunningHours() const { return maxRunningHours_; }

bool Aircraft::isAssigned() const{ return assigned;}
void Aircraft::assignToFlight(bool as){ assigned = as;}
void Aircraft::addRunningHours(float hours) { runningHours_ += hours; }
void Aircraft::setMaintenanceStatus(MaintenanceStatus s) {status_ = s;}

const std::unordered_map<SeatId_t, SeatData>& Aircraft::getAllSeats() const {
    return seatLayout_.getAllSeats();
}

const std::shared_ptr<SeatId_t> Aircraft::findSeat(SeatClass seatClass) const {
    return seatLayout_.findSeat(seatClass);
}

bool Aircraft::assignSeat(SeatClass seatClass, std::shared_ptr<Passenger> passenger) {
    auto status = seatLayout_.assignSeat(seatClass, std::move(passenger));
    if (status.has_value()) {
        return true;
    } else{
        return false;
    }
}

bool Aircraft::assignSeat(const SeatId_t& id, std::shared_ptr<Passenger> passenger){
    auto status = seatLayout_.assignSeat(id, std::move(passenger));
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

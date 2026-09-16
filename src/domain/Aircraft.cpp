#include "domain/Aircraft.hpp"
#include <utility>

namespace airline {

Aircraft::Aircraft(std::string tailNumber, std::string model, int capacity)
    : tailNumber_(std::move(tailNumber)), model_(std::move(model)), capacity_(capacity) {}

const std::string& Aircraft::getTailNumber() const { return tailNumber_; }
const std::string& Aircraft::getModel() const { return model_; }
int Aircraft::getCapacity() const { return capacity_; }
MaintenanceStatus Aircraft::getStatus() const { return status_; }
void Aircraft::setStatus(MaintenanceStatus status) { status_ = status; }

}  // namespace airline

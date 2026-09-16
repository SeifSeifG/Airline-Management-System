#include "domain/MaintenanceRecord.hpp"
#include <utility>

namespace airline {

MaintenanceRecord::MaintenanceRecord(std::string date, std::string description, std::string technician)
    : date_(std::move(date)), description_(std::move(description)), technician_(std::move(technician)) {}

const std::string& MaintenanceRecord::getDate() const { return date_; }
const std::string& MaintenanceRecord::getDescription() const { return description_; }
const std::string& MaintenanceRecord::getTechnician() const { return technician_; }

}  // namespace airline

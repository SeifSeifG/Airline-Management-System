#pragma once
#include "domain/Enums.hpp"
#include <string>

namespace airline {

class Aircraft {
public:
    Aircraft(std::string tailNumber, std::string model, int capacity);

    const std::string& getTailNumber() const;
    const std::string& getModel() const;
    int getCapacity() const;
    MaintenanceStatus getStatus() const;
    void setStatus(MaintenanceStatus status);

private:
    std::string tailNumber_;
    std::string model_;
    int capacity_;
    MaintenanceStatus status_ = MaintenanceStatus::Airworthy;
};

}  // namespace airline

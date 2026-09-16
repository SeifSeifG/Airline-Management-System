#pragma once
#include <string>

namespace airline {

class MaintenanceRecord {
public:
    MaintenanceRecord(std::string date, std::string description, std::string technician);

    const std::string& getDate() const;
    const std::string& getDescription() const;
    const std::string& getTechnician() const;

private:
    std::string date_;
    std::string description_;
    std::string technician_;
};

}  // namespace airline

#ifndef REPORTING_SERVICE_HPP
#define REPORTING_SERVICE_HPP

#include <string>
#include <vector>
#include "persistence/AircraftRepo.hpp"
#include "persistence/FlightRepo.hpp"

namespace airline {

struct FlightPerformance {
    std::string flightId;
    std::string status;
    int bookingsCount{0};
    double revenue{0.0};
};

struct OperationalReportData {
    int totalScheduled{0};
    int completed{0};
    int delayed{0};
    int canceled{0};
    int totalReservations{0};
    double totalRevenue{0.0};
    std::vector<FlightPerformance> flightSummaries;
};

class ReportingService {
private:
    const FlightRepository& flightRepo_;
    const AircraftRepository& aircraftRepo_;

public:
    ReportingService(const FlightRepository& flightRepo, const AircraftRepository& aircraftRepo);

    OperationalReportData generateOperationalReport(const std::string& monthYear) const;
    void generateMaintenanceReport() const;
    void generateUserActivityReport() const;
};

} // namespace airline

#endif // REPORTING_SERVICE_HPP
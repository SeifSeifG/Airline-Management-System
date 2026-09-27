#include "services/ReportingService.hpp"

namespace airline {

// Signature MUST match the header exactly
ReportingService::ReportingService(const FlightRepository& flightRepo,
                                   const AircraftRepository& aircraftRepo)
    : flightRepo_(flightRepo), aircraftRepo_(aircraftRepo) {}

} // namespace airline
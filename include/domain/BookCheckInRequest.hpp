#pragma once
#include <memory>
#include <string>

namespace airline {

class Passenger;
class Flight;

enum class RequestStatus { Pending, Confirmed, Rejected };

struct BookingRequest {
    std::string id;
    std::shared_ptr<Passenger> passenger;
    std::shared_ptr<Flight> flight;
    RequestStatus status = RequestStatus::Pending;
};

struct CheckInRequest {
    std::string id;
    std::shared_ptr<Passenger> passenger;
    std::shared_ptr<Flight> flight;
    RequestStatus status = RequestStatus::Pending;
};

// Helper for status formatting
inline std::string statusToString(RequestStatus status) {
    switch (status) {
        case RequestStatus::Pending:   return "Pending";
        case RequestStatus::Confirmed: return "Confirmed";
        case RequestStatus::Rejected:  return "Rejected";
    }
    return "Unknown";
}

} // namespace airline
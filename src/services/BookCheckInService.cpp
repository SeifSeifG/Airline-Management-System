#include "services/BookCheckInService.hpp"

namespace airline {

BookingService::BookingService(FlightRepository& flightRepo,
                               std::vector<std::shared_ptr<BookingRequest>>& bookingRequests,
                               std::vector<std::shared_ptr<CheckInRequest>>& checkInRequests)
    : flightRepo_(flightRepo),
      bookingRequests_(bookingRequests),
      checkInRequests_(checkInRequests) {}

std::string BookingService::generateBookingId() {
    return "BR-" + std::to_string(nextBookingId_++);
}

std::string BookingService::generateCheckInId() {
    return "CR-" + std::to_string(nextCheckInId_++);
}

std::vector<std::shared_ptr<Flight>> BookingService::searchFlights(const std::string& origin, const std::string& destination) const {
    std::vector<std::shared_ptr<Flight>> matchingFlights;
    const auto& allFlights = flightRepo_.getAll();

    for (const auto& flight : allFlights) {
        if (flight->getOrigin().find(origin) != std::string::npos &&
            flight->getDestination().find(destination) != std::string::npos) {
            matchingFlights.push_back(flight);
        }
    }
    return matchingFlights;
}

std::shared_ptr<BookingRequest> BookingService::createBookingRequest(
    const std::shared_ptr<Passenger>& passenger,
    const std::shared_ptr<Flight>& flight,
    SeatClass seatClass) 
{
    if (!passenger || !flight) {
        return nullptr;
    }

    auto seatId = flight->findSeat(seatClass);
    if (!seatId) {
        return nullptr;
    }

    if (!flight->assignSeat(*seatId, passenger)) {
        return nullptr;
    }

    auto req = std::make_shared<BookingRequest>();
    req->id = generateBookingId();
    req->passenger = passenger;
    req->flight = flight;
    req->status = RequestStatus::Pending;

    bookingRequests_.push_back(req);
    return req;
}

std::vector<std::shared_ptr<BookingRequest>> BookingService::getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const {
    std::vector<std::shared_ptr<BookingRequest>> results;
    for (const auto& req : bookingRequests_) {
        if (req->passenger == passenger) {
            results.push_back(req);
        }
    }
    return results;
}

std::vector<std::shared_ptr<BookingRequest>> BookingService::getPendingBookingRequests() const {
    std::vector<std::shared_ptr<BookingRequest>> pending;
    for (const auto& req : bookingRequests_) {
        if (req->status == RequestStatus::Pending) {
            pending.push_back(req);
        }
    }
    return pending;
}

std::shared_ptr<CheckInRequest> BookingService::createCheckInRequest(
    const std::shared_ptr<Passenger>& passenger,
    const std::shared_ptr<Flight>& flight) 
{
    if (!passenger || !flight) {
        return nullptr;
    }

    auto req = std::make_shared<CheckInRequest>();
    req->id = generateCheckInId();
    req->passenger = passenger;
    req->flight = flight;
    req->status = RequestStatus::Pending;

    checkInRequests_.push_back(req);
    return req;
}

std::vector<std::shared_ptr<CheckInRequest>> BookingService::getPendingCheckInRequests() const {
    std::vector<std::shared_ptr<CheckInRequest>> pending;
    for (const auto& req : checkInRequests_) {
        if (req->status == RequestStatus::Pending) {
            pending.push_back(req);
        }
    }
    return pending;
}

std::shared_ptr<CheckInRequest> BookingService::getCheckInRequest(
    const std::shared_ptr<Passenger>& passenger,
    const std::shared_ptr<Flight>& flight) const 
{
    for (const auto& ci : checkInRequests_) {
        if (ci->passenger == passenger && ci->flight == flight) {
            return ci;
        }
    }
    return nullptr;
}

} // namespace airline
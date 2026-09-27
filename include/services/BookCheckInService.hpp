#ifndef BOOKING_SERVICE_HPP
#define BOOKING_SERVICE_HPP

#include <memory>
#include <string>
#include <vector>

#include "domain/BookCheckInRequest.hpp"
#include "domain/Flight.hpp"
#include "domain/Passenger.hpp"
#include "domain/SeatLayout.hpp"
#include "persistence/FlightRepo.hpp"

namespace airline {

class BookingService {
private:
    FlightRepository& flightRepo_;
    std::vector<std::shared_ptr<BookingRequest>>& bookingRequests_;
    std::vector<std::shared_ptr<CheckInRequest>>& checkInRequests_;

    int nextBookingId_{1};
    int nextCheckInId_{1};

    std::string generateBookingId();
    std::string generateCheckInId();

public:
    BookingService(FlightRepository& flightRepo,
                   std::vector<std::shared_ptr<BookingRequest>>& bookingRequests,
                   std::vector<std::shared_ptr<CheckInRequest>>& checkInRequests);

    std::vector<std::shared_ptr<Flight>> searchFlights(const std::string& origin, const std::string& destination) const;

    std::shared_ptr<BookingRequest> createBookingRequest(
        const std::shared_ptr<Passenger>& passenger,
        const std::shared_ptr<Flight>& flight,
        SeatClass seatClass,
        SeatPosition position);

    std::vector<std::shared_ptr<BookingRequest>> getPendingBookingRequests() const;
    std::vector<std::shared_ptr<BookingRequest>> getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const;

    std::shared_ptr<CheckInRequest> createCheckInRequest(
        const std::shared_ptr<Passenger>& passenger,
        const std::shared_ptr<Flight>& flight);

    std::vector<std::shared_ptr<CheckInRequest>> getPendingCheckInRequests() const;
    std::shared_ptr<CheckInRequest> getCheckInRequest(
        const std::shared_ptr<Passenger>& passenger,
        const std::shared_ptr<Flight>& flight) const;
};

} // namespace airline

#endif // BOOKING_SERVICE_HPP
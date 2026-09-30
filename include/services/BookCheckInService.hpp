#ifndef BOOKING_SERVICE_HPP
#define BOOKING_SERVICE_HPP

#include <memory>
#include <string>
#include <vector>

#include "domain/Flight.hpp"
#include "domain/Passenger.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/BookingRequest.hpp"
#include "persistence/FlightRepo.hpp"

namespace airline {

class BookingService {
private:
    FlightRepository& flightRepo_;
    std::vector<std::shared_ptr<BookingRequest>>& bookingRequests_;
    std::vector<std::shared_ptr<CheckInRequest>>& checkInRequests_;
    std::vector<std::shared_ptr<FinishedRequest>>& finishedRequests_;

    static int nextBookingId_;  // Added trailing underscore
    static int nextCheckInId_;  // Added trailing underscore
public:
    BookingService(FlightRepository& flightRepo,
                std::vector<std::shared_ptr<BookingRequest>>& bookingRequests,
                std::vector<std::shared_ptr<CheckInRequest>>& checkInRequests,
                std::vector<std::shared_ptr<FinishedRequest>>& finishedRequests);

    std::vector<std::shared_ptr<Flight>> searchFlights(const std::string& origin, const std::string& destination) const;

    bool isDuplicatedBookingReq(const std::shared_ptr<Passenger>& passenger, const std::shared_ptr<Flight>& flight);
    std::shared_ptr<BookingRequest> createBookingRequest( //called by passenger & the app
        const std::shared_ptr<Passenger>& passenger,
        const std::shared_ptr<Flight>& flight,
        SeatClass seatClass);

    std::vector<std::shared_ptr<BookingRequest>> getAllBookingRequests() const;
    std::vector<std::shared_ptr<BookingRequest>> getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const;
    PaymentStatus processPayment(const std::shared_ptr<BookingRequest>& bookingReq);
    void refundPassenger(std::shared_ptr<Passenger> p, int seatPrice);
    RequestReply confirmBookingRequest(const std::string& bookingRequestId); // called to modify the request itself

    std::shared_ptr<CheckInRequest> createCheckInRequest( // called to modify passenger & the app
        const std::shared_ptr<Passenger>& passenger,
        const std::shared_ptr<BookingRequest>& flight);
        
    std::vector<std::shared_ptr<CheckInRequest>> getAllCheckInRequests() const;
    std::vector<std::shared_ptr<CheckInRequest>> getCheckInRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const;
    bool confirmCheckInRequest(const std::string& bookingRequestId); // called to modify the request itself

    void resolveRequestsForDepartedFlight(const std::shared_ptr<Flight>& flight);

    std::string generateBookingId();
    std::string generateCheckInId();

    void initializeNextId();
};

} // namespace airline

#endif // BOOKING_SERVICE_HPP
#include "services/BookCheckInService.hpp"
#include <algorithm>

namespace airline {

int BookingService::nextBookingId_ = 1;
int BookingService::nextCheckInId_ = 1;

BookingService::BookingService(FlightRepository& flightRepo,
                               std::vector<std::shared_ptr<BookingRequest>>& bookingRequests,
                               std::vector<std::shared_ptr<CheckInRequest>>& checkInRequests,
                               std::vector<std::shared_ptr<FinishedRequest>>& finishedRequests)
    : flightRepo_(flightRepo),
      bookingRequests_(bookingRequests),
      checkInRequests_(checkInRequests),
      finishedRequests_(finishedRequests) {}

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


bool BookingService::isDuplicatedBookingReq(const std::shared_ptr<Passenger>& passenger,
    const std::shared_ptr<Flight>& flight)
{
    if (!passenger || !flight) {
        return false;
    }

    for (const auto& existingReq : bookingRequests_) {
        if (!existingReq) continue;

        // Safely convert weak_ptr to shared_ptr before accessing members
        auto reqPassenger = existingReq->passenger.lock();
        auto reqFlight    = existingReq->flight.lock();

        if (reqPassenger && reqFlight) {
            if (reqPassenger->getId() == passenger->getId() && 
                reqFlight->getFlightNumber() == flight->getFlightNumber() &&
                existingReq->status != ReservationStatus::Cancelled) 
            {
                return true; // YES, a duplicate active request exists!
            }
        }
    }
    return false; // NO duplicate found
}

std::shared_ptr<BookingRequest> BookingService::createBookingRequest(
    const std::shared_ptr<Passenger>& passenger,
    const std::shared_ptr<Flight>& flight,
    SeatClass seatClass) 
{
    if (!passenger || !flight) {
        return nullptr;
    }

    // 1. Find and assign seat
    auto seatId = flight->findSeat(seatClass);
    if (!seatId) {
        return nullptr;
    }

    // 2. Create new booking request
    auto req = std::make_shared<BookingRequest>(BookingRequest{
        .id = generateBookingId(),
        .passenger = passenger,
        .flight = flight,
        .seatClass = seatClass,
        .price = flight->getPriceByClass(seatClass),
        .status = ReservationStatus::PendingBook 
    });
    
    passenger->addBookingReq(req);
    bookingRequests_.push_back(req);
    return req;
}

std::vector<std::shared_ptr<BookingRequest>> BookingService::getAllBookingRequests() const {
    return this->bookingRequests_;
}

std::vector<std::shared_ptr<BookingRequest>> BookingService::getBookingRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const {
    std::vector<std::shared_ptr<BookingRequest>> results;
    for (const auto& req : bookingRequests_) {
        auto p = req->passenger.lock();
        if (p == passenger) {
            results.push_back(req);
        }
    }
    return results;
}


PaymentStatus BookingService::processPayment(const std::shared_ptr<BookingRequest>& bookingReq){
    auto passenger = bookingReq->passenger.lock();
    auto flight = bookingReq->flight.lock();

    if( passenger->makePayment(flight->getPriceByClass(bookingReq->seatClass))){
        return PaymentStatus::Completed;
    } else {
        return PaymentStatus::Failed;
    }

}

bool BookingService::confirmBookingRequest(const std::string& bookingRequestId) {
    // 1. Locate the requested BookingRequest by ID
    auto it = std::find_if(bookingRequests_.begin(), bookingRequests_.end(),
        [&bookingRequestId](const std::shared_ptr<BookingRequest>& req) {
            return req && req->id == bookingRequestId;
    });

    if (it == bookingRequests_.end()) {
        return false; // Request ID not found
    }

    auto bookingReq = *it;

    auto passenger = bookingReq->passenger.lock();
    auto flight    = bookingReq->flight.lock();
    auto seatId = flight->findSeat(bookingReq->seatClass);

    // 2. Process Payment (or seats no longer available at confirmation time)
    if (processPayment(bookingReq) != PaymentStatus::Completed || !seatId) {
        bookingReq->status = ReservationStatus::Cancelled;

        // Record failed request in Finished Requests
        auto finishedRecord = std::make_shared<FinishedRequest>(FinishedRequest{
            .checkInId    = bookingReq->id,
            .passengerId  = passenger ? passenger->getId() : "",
            .passengerName= passenger ? passenger->getName() : "",
            .flightNumber = flight ? flight->getFlightNumber() : "",
            .origin       = flight ? flight->getOrigin() : "",
            .destination  = flight ? flight->getDestination() : "",
            .departureDate= flight ? toString(flight->getDate()) : "",
            .seatClass    = bookingReq->seatClass,
            .price        = bookingReq->price,
            .reservationStatus  = ReservationStatus::Confirmed,
            .paymentStatus = seatId ? PaymentStatus::Failed : PaymentStatus::Pending // mark the difference
        });

        finishedRequests_.push_back(finishedRecord);
        return false;
    }

    // 3. Assign seat after payment succeeds
    flight->assignSeat(*seatId, passenger);

    // 4. Mark booking request as confirmed
    bookingReq->status = ReservationStatus::ConfirmedBook; 
    return true;
}

std::shared_ptr<CheckInRequest> BookingService::createCheckInRequest(
    const std::shared_ptr<Passenger>& passenger,
    const std::shared_ptr<BookingRequest>& req) 
{
    if (!passenger || !req) {
        return nullptr;
    }

    if (req->status != ReservationStatus::ConfirmedBook){
        return nullptr;
    }

    auto checkInReq = std::make_shared<CheckInRequest>(CheckInRequest{
        .id = generateCheckInId(),
        .passenger = req->passenger,
        .flight = req->flight,
        .seatClass = req->seatClass,
        .price = req->price,
        .status = ReservationStatus::PendingCheckIn
    });

    checkInRequests_.push_back(checkInReq);

    // 1. Remove req from the passenger's booking requests list
    passenger->removeBookingReq(req);

    // 2. Remove req from BookingService's internal booking list (if maintained)
    bookingRequests_.erase(
        std::remove(bookingRequests_.begin(), bookingRequests_.end(), req),
        bookingRequests_.end()
    );

    // 3. add the check in request to the passenger 
    passenger->addCheckInReq(checkInReq);

    return checkInReq;
}

std::vector<std::shared_ptr<CheckInRequest>> BookingService::getAllCheckInRequests() const {
    return this->checkInRequests_;
}

std::vector<std::shared_ptr<CheckInRequest>> BookingService::getCheckInRequestsForPassenger(const std::shared_ptr<Passenger>& passenger) const 
{
    std::vector<std::shared_ptr<CheckInRequest>> results;
    for (const auto& req : checkInRequests_) {
        auto p = req->passenger.lock();
        if (p == passenger) {
            results.push_back(req);
        }
    }
    return results;
}

bool BookingService::confirmCheckInRequest(const std::string& checkInRequestId) {
    // 1. Locate the requested CheckInRequest by ID
    auto it = std::find_if(checkInRequests_.begin(), checkInRequests_.end(),
        [&checkInRequestId](const std::shared_ptr<CheckInRequest>& req) {
            return req && req->id == checkInRequestId;
    });

    if (it == checkInRequests_.end()) {
        return false;
    }

    auto checkInReq = *it;
    checkInReq->status = ReservationStatus::Confirmed;
    auto passenger = checkInReq->passenger.lock();
    auto flight    = checkInReq->flight.lock();

    // 2. Add to Passenger's Travel History
    if (passenger && flight) {
        TravelHistory history;
        history.flightNumber = flight->getFlightNumber();
        history.date         = toString(flight->getDate());
        history.origin       = flight->getOrigin();
        history.destination  = flight->getDestination();
        history.seatClass    = checkInReq->seatClass;
        history.price        = checkInReq->price;

        passenger->addTravelHistory(history);
        passenger->earnLoyaltyPoints(flight->getPriceByClass(checkInReq->seatClass));
    }

    // 3. Create FinishedRequest snapshot for reporting
    auto finishedRecord = std::make_shared<FinishedRequest>(FinishedRequest{
            .checkInId    = checkInReq->id,
            .passengerId  = passenger ? passenger->getId() : "",
            .passengerName= passenger ? passenger->getName() : "",
            .flightNumber = flight ? flight->getFlightNumber() : "",
            .origin       = flight ? flight->getOrigin() : "",
            .destination  = flight ? flight->getDestination() : "",
            .departureDate= flight ? toString(flight->getDate()) : "",
            .seatClass    = checkInReq->seatClass,
            .price        = checkInReq->price,
            .reservationStatus  = ReservationStatus::Confirmed
    });


    finishedRequests_.push_back(finishedRecord);

    // 4. Remove processed check-in request from active vector
    checkInRequests_.erase(it);

    return true;
}

void BookingService::initializeNextId() {
    int maxId = 0;
    for (const auto& booking : this->bookingRequests_) {
        // Extract numeric part from "BR-15" -> 15
        std::string idStr = booking->id;
        if (idStr.find("BR-") == 0) {
            int numericId = std::stoi(idStr.substr(3));
            if (numericId > maxId) {
                maxId = numericId;
            }
        }
    }
    nextBookingId_ = maxId + 1;

    maxId = 0;
    for (const auto& checkIn : this->checkInRequests_) {
        // Extract numeric part from "CR-15" -> 15
        std::string idStr = checkIn->id;
        if (idStr.find("CR-") == 0) {
            int numericId = std::stoi(idStr.substr(3));
            if (numericId > maxId) {
                maxId = numericId;
            }
        }
    }
    nextCheckInId_ = maxId + 1;
}

};  // namespace airline
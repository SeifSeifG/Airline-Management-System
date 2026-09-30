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

void BookingService::refundPassenger(std::shared_ptr<Passenger> passnger, int seatPrice){
    passnger->rechargeBalance(seatPrice);
}

RequestReply BookingService::confirmBookingRequest(const std::string& bookingRequestId) {
    RequestReply reply{
        .idFound = true,
        .payStatus = PaymentStatus::Completed
    };

    // 1. Check if the request exists in bookingRequests_ and is already confirmed
    auto it = std::find_if(bookingRequests_.begin(), bookingRequests_.end(),
        [&bookingRequestId](const std::shared_ptr<BookingRequest>& req) {
            return req && req->id == bookingRequestId;
    });

    // check if it exists
    if (it == bookingRequests_.end()){
        reply.idFound = false; // Request ID not found
        return reply;
    }

    auto bookingReq = *it;

    // check if this request is already confirmed
    if (bookingReq->status == ReservationStatus::ConfirmedBook) {
        RequestReply reply;
        reply.idFound = true;
        reply.payStatus = PaymentStatus::Completed;
        return reply;
    }
    
    auto passenger  = bookingReq->passenger.lock();
    auto flight     = bookingReq->flight.lock();

    // Helper lambda: safely extracts details and inserts into finishedRequests_ (if need be) without duplicates
    auto recordFinishedRequest = [&](ReservationStatus resStatus, PaymentStatus payStatus) {
        std::string pId       = passenger ? passenger->getId()   : "N/A";
        std::string pName     = passenger ? passenger->getName() : "Unknown Passenger";
        std::string flightNum = flight    ? flight->getFlightNumber() : "N/A";
        std::string origin    = flight    ? flight->getOrigin()       : "N/A";
        std::string dest      = flight    ? flight->getDestination()  : "N/A";
        std::string depDate   = flight    ? toString(flight->getDate()) : "N/A";

        // Remove any existing record for this request ID to avoid duplicates
        finishedRequests_.erase(
            std::remove_if(finishedRequests_.begin(), finishedRequests_.end(),
                [&bookingReq](const std::shared_ptr<FinishedRequest>& rec) {
                    return rec && rec->checkInId == bookingReq->id;
                }),
            finishedRequests_.end()
        );

        // Record the updated outcome
        finishedRequests_.push_back(std::make_shared<FinishedRequest>(FinishedRequest{
            .checkInId         = bookingReq->id,
            .passengerId       = pId,
            .passengerName     = pName,
            .flightNumber      = flightNum,
            .origin            = origin,
            .destination       = dest,
            .departureDate     = depDate,
            .seatClass         = bookingReq->seatClass,
            .price             = bookingReq->price,
            .reservationStatus = resStatus,
            .paymentStatus     = payStatus
        }));
    };

    // Check seat availability
    auto seatId = flight->findSeat(bookingReq->seatClass);

    // 1. No seats available for required seat class
    if (!seatId) {
        bookingReq->status = ReservationStatus::Cancelled;
        
        refundPassenger(passenger, flight->getPriceByClass(bookingReq->seatClass));

        reply.payStatus = PaymentStatus::Pending;
        recordFinishedRequest(ReservationStatus::Cancelled, PaymentStatus::Pending);
        return reply;
    }

    // 2. Process Payment
    if (processPayment(bookingReq) != PaymentStatus::Completed) {
        bookingReq->status = ReservationStatus::PendingBook;

        reply.payStatus = PaymentStatus::Failed;
        recordFinishedRequest(ReservationStatus::PendingBook, PaymentStatus::Failed);
        return reply;
    }

    // 3. Payment succeeded: Assign seat
    flight->assignSeat(*seatId, passenger);

    // 4. Mark booking request as confirmed
    bookingReq->status = ReservationStatus::ConfirmedBook;

    // Remove any previous failed attempt record now that confirmation succeeded
    finishedRequests_.erase(
        std::remove_if(finishedRequests_.begin(), finishedRequests_.end(),
            [&bookingReq](const std::shared_ptr<FinishedRequest>& rec) {
                return rec && rec->checkInId == bookingReq->id;
            }),
        finishedRequests_.end()
    );

    return reply;
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

    // 2. Remove req from BookingService's internal booking list
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

    // 2. Update check-in request status
    auto checkInReq = *it;
    checkInReq->status = ReservationStatus::Confirmed;

    return true;
}

void BookingService::resolveRequestsForDepartedFlight(const std::shared_ptr<Flight>& flight) {
    if (!flight) return;

    const std::string flightNum = flight->getFlightNumber();

    // ---------------------------------------------------------------------
    // 1. Resolve Booking Requests (Pending / ConfirmedBook -> Confirmed or Missed)
    // ---------------------------------------------------------------------
    for (auto& req : bookingRequests_) {
        if (!req) continue;

        auto reqFlight = req->flight.lock();
        if (reqFlight->getFlightNumber() != flightNum) {
            continue;
        }

        auto passenger = req->passenger.lock();
        std::string passengerId   = passenger->getId();
        std::string passengerName = passenger->getName();

        // Remove existing finished record with matching ID if re-processing
        auto eraseIt = std::remove_if(
            finishedRequests_.begin(),
            finishedRequests_.end(),
            [&req](const std::shared_ptr<FinishedRequest>& rec) {
                return rec && rec->checkInId == req->id;
        });

        finishedRequests_.erase(eraseIt, finishedRequests_.end());

        // Confirmed / Paid requests -> Move to Finished Requests as Confirmed
        if (req->status == ReservationStatus::Confirmed) {
            auto finishedRecord = std::make_shared<FinishedRequest>(FinishedRequest{
                req->id,
                passengerId,
                passengerName,
                flight->getFlightNumber(),
                flight->getOrigin(),
                flight->getDestination(),
                toString(flight->getDate()),
                req->seatClass,
                req->price,
                ReservationStatus::Confirmed,
                PaymentStatus::Completed
            });
            finishedRequests_.push_back(finishedRecord);
        }  else if (req->status != ReservationStatus::Cancelled) {  // Incomplete / Pending requests at departure time -> Mark as Missed
            req->status = ReservationStatus::Missed;

            auto missedRecord = std::make_shared<FinishedRequest>(FinishedRequest{
                req->id,
                passengerId,
                passengerName,
                flight->getFlightNumber(),
                flight->getOrigin(),
                flight->getDestination(),
                toString(flight->getDate()),
                req->seatClass,
                req->price,
                ReservationStatus::Missed,
                PaymentStatus::Pending
            });
            finishedRequests_.push_back(missedRecord);
        } else { // they cancelled it on their own
            req->status = ReservationStatus::Cancelled;

            auto missedRecord = std::make_shared<FinishedRequest>(FinishedRequest{
                req->id,
                passengerId,
                passengerName,
                flight->getFlightNumber(),
                flight->getOrigin(),
                flight->getDestination(),
                toString(flight->getDate()),
                req->seatClass,
                req->price,
                ReservationStatus::Cancelled,
                PaymentStatus::Refunded
            });
            finishedRequests_.push_back(missedRecord);
        }
    }

    // ---------------------------------------------------------------------
    // 2. Resolve Check-In Requests -> Travel History & Finished Registry
    // ---------------------------------------------------------------------
    for (auto it = checkInRequests_.begin(); it != checkInRequests_.end(); ) {
        auto checkInReq = *it;
        if (!checkInReq) {
            ++it;
            continue;
        }

        auto reqFlight = checkInReq->flight.lock();
        if (!reqFlight || reqFlight->getFlightNumber() != flightNum) {
            ++it;
            continue;
        }

        auto passenger = checkInReq->passenger.lock();
        std::string passengerId   = passenger ? passenger->getId()   : "N/A";
        std::string passengerName = passenger ? passenger->getName() : "Unknown Passenger";

        if (checkInReq->status == ReservationStatus::Confirmed) {
            // A) Update Passenger's Travel History and Loyalty Points
            if (passenger) {
                TravelHistory history;
                history.flightNumber = flight->getFlightNumber();
                history.date         = toString(flight->getDate());
                history.origin       = flight->getOrigin();
                history.destination  = flight->getDestination();
                history.seatClass    = checkInReq->seatClass;
                history.price        = checkInReq->price;

                passenger->removeCheckInReq(checkInReq);
                passenger->addTravelHistory(history);
                passenger->earnLoyaltyPoints(flight->getPriceByClass(checkInReq->seatClass));
            }

            // B) Upsert into finishedRequests_
            auto eraseIt = std::remove_if(
                finishedRequests_.begin(),
                finishedRequests_.end(),
                [&checkInReq](const std::shared_ptr<FinishedRequest>& rec) {
                    return rec && rec->checkInId == checkInReq->id;
                }
            );
            finishedRequests_.erase(eraseIt, finishedRequests_.end());

            auto finishedRecord = std::make_shared<FinishedRequest>(FinishedRequest{
                checkInReq->id,
                passengerId,
                passengerName,
                flight->getFlightNumber(),
                flight->getOrigin(),
                flight->getDestination(),
                toString(flight->getDate()),
                checkInReq->seatClass,
                checkInReq->price,
                ReservationStatus::Confirmed,
                PaymentStatus::Completed
            });
            finishedRequests_.push_back(finishedRecord);
        }

        // Erase processed check-in request from active vector
        it = checkInRequests_.erase(it);
    }
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
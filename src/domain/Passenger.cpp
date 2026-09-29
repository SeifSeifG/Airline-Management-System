#include "domain/Passenger.hpp"
#include "domain/LoyaltyAccount.hpp"
#include "domain/BookingRequest.hpp"
#include <iostream>
#include <iomanip>
#include <utility>
#include <algorithm>

namespace airline {

int Passenger::nextId = 1;

Passenger::Passenger(PersonId_t id, std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword, int balance)
    : User(std::move(id), std::move(name), std::move(contactInfo),
           std::move(username), std::move(hashedPassword), Role::Passenger),
      loyaltyAccount_(std::make_unique<LoyaltyAccount>()), balance_(balance) {}

Passenger::Passenger(std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword, int balance)
    : User(this->generateId(), std::move(name), std::move(contactInfo),
           std::move(username), std::move(hashedPassword), Role::Passenger),
      loyaltyAccount_(std::make_unique<LoyaltyAccount>()), balance_(balance) {}

// Defined here (not defaulted in the header) because LoyaltyAccount is only
// forward-declared in Passenger.hpp -- the unique_ptr destructor needs the
// complete type, which is only visible in this translation unit.
Passenger::~Passenger() = default;
Passenger::Passenger(Passenger&&) noexcept = default;
Passenger& Passenger::operator=(Passenger&&) noexcept = default;

Role Passenger::getRole() const {
    return Role::Passenger;
}

void Passenger::earnLoyaltyPoints(int points) {
    loyaltyAccount_->earnPoints(points);

}

bool Passenger::redeemLoyaltyPoints(int points) {
    return loyaltyAccount_->redeem(points); // Returns true/false based on success
}

int Passenger::getLoyaltyBalance() const {
    return loyaltyAccount_->getPoints();
}

void Passenger::rechargeBalance(int delta){
    balance_ += delta;
}

int Passenger::getBalance() const{
    return balance_;
}

bool Passenger::makePayment(int deduction){
    if (deduction > balance_){
        return false;
    }
    balance_ -= deduction;
    return true;
}

void Passenger::addBookingReq(std::shared_ptr<BookingRequest> request) {
    if (request) {
        bookingRequests_.push_back(std::move(request));
    }
}

void Passenger::removeBookingReq(std::shared_ptr<BookingRequest> request) {
    if (!request) return;

    bookingRequests_.erase(
        std::remove_if(bookingRequests_.begin(), bookingRequests_.end(),
            [&request](const std::shared_ptr<BookingRequest>& req) {
                // Match by pointer comparison or by Request ID
                return req && (req == request);
            }),
        bookingRequests_.end()
    );
}

std::vector<std::shared_ptr<BookingRequest>>& Passenger::getBookingRequests() {
    return bookingRequests_;
}

const std::vector<std::shared_ptr<BookingRequest>>& Passenger::getBookingRequests() const {
    return bookingRequests_;
}

std::shared_ptr<BookingRequest> Passenger::getBookingRequestById(const std::string& requestId) const {
    for (const auto& req : bookingRequests_) {
        if (req && req->id == requestId) { // Change getId() to getRequestId() if needed
            return req;
        }
    }
    return nullptr;
}

void Passenger::addCheckInReq(std::shared_ptr<CheckInRequest> request) {
    if (request) {
        checkInRequests_.push_back(std::move(request));
    }
}

std::vector<std::shared_ptr<CheckInRequest>>& Passenger::getCheckInRequests() {
    return checkInRequests_;
}

const std::vector<std::shared_ptr<CheckInRequest>>& Passenger::getCheckInRequests() const {
    return checkInRequests_;
}

std::shared_ptr<CheckInRequest> Passenger::getCheckInRequestById(const std::string& requestId) const {
    for (const auto& req : checkInRequests_) {
        if (req && req->id == requestId) { // Change getId() to getRequestId() if needed
            return req;
        }
    }
    return nullptr;
}

// Add travel history entry
void Passenger::addTravelHistory(const TravelHistory& history) {
    travelHistory_.push_back(history);
}

void Passenger::addTravelHistory(const std::shared_ptr<FinishedRequest>& finishedReq) {
    if (!finishedReq) return;

    TravelHistory history{
        .flightNumber = finishedReq->flightNumber,
        .date         = finishedReq->departureDate, // Maps departureDate to date
        .origin       = finishedReq->origin,
        .destination  = finishedReq->destination,
        .seatClass    = finishedReq->seatClass,
        .price        = finishedReq->price
    };

    travelHistory_.push_back(history);
}

// Getter for travel history
const std::vector<TravelHistory>& Passenger::getTravelHistory() const {
    return travelHistory_;
}

std::string Passenger::generateId() {
    std::ostringstream oss;
    oss << "P" << std::setw(3) << std::setfill('0') << nextId++;
    return oss.str();
}

void Passenger::setNextId(int id){
    nextId = id;
}


}  // namespace airline

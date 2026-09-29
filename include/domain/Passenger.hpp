#pragma once
#include "domain/User.hpp"
#include <memory>
#include <vector>
#include <chrono>

namespace airline {

// Forward declaration
class LoyaltyAccount;
class Flight;

struct BookingRequest;
struct CheckInRequest;
struct TravelHistory;
struct FinishedRequest;

class Passenger : public User {
private:
    // 1-to-1 relationship with LoyaltyAccount, so unique_ptr it is.
    std::unique_ptr<LoyaltyAccount> loyaltyAccount_;
    std::vector<TravelHistory> travelHistory_;  // 1-to-many relationship with TravelHistory
    std::vector<std::shared_ptr<BookingRequest>> bookingRequests_;
    std::vector<std::shared_ptr<CheckInRequest>> checkInRequests_;
    int balance_;
    static int nextId;
    std::string generateId() override;
public:
    Passenger(PersonId_t id, std::string name, contactInfo contactInfo,
              std::string username, std::string hashedPassword, int balance);
    Passenger(std::string name, contactInfo contactInfo,
              std::string username, std::string hashedPassword, int balance);
    ~Passenger() override;

    Passenger(const Passenger&) = delete;
    Passenger& operator=(const Passenger&) = delete;
    Passenger(Passenger&&) noexcept;
    Passenger& operator=(Passenger&&) noexcept;

    Role getRole() const override;

    // clean wrappers to LoyaltyAccount
    void earnLoyaltyPoints(int points);
    bool redeemLoyaltyPoints(int points);
    int getLoyaltyBalance() const;

    void rechargeBalance(int delta);
    int getBalance() const;
    bool makePayment(int deduction);

    // booking setters and getters
    void addBookingReq(std::shared_ptr<BookingRequest> request);
    void removeBookingReq(std::shared_ptr<BookingRequest> request);
    std::vector<std::shared_ptr<BookingRequest>>& getBookingRequests();
    const std::vector<std::shared_ptr<BookingRequest>>& getBookingRequests() const;
    std::shared_ptr<BookingRequest> getBookingRequestById(const std::string& requestId) const;
    
    void addCheckInReq(std::shared_ptr<CheckInRequest> request);
    void removeCheckInReq(std::shared_ptr<CheckInRequest> request);
    std::vector<std::shared_ptr<CheckInRequest>>& getCheckInRequests();
    const std::vector<std::shared_ptr<CheckInRequest>>& getCheckInRequests() const;
    std::shared_ptr<CheckInRequest> getCheckInRequestById(const std::string& requestId) const;

    // Add travel history entry
    void addTravelHistory(const TravelHistory& history);
    void addTravelHistory(const std::shared_ptr<FinishedRequest>& finishedReq);
    const std::vector<TravelHistory>& getTravelHistory() const;

    static void setNextId(int id);
};

struct TravelHistory {
    std::string flightNumber;
    std::string date;
    std::string origin;
    std::string destination;
    SeatClass seatClass{SeatClass::Economy};
    int price{0}; // Added price
};
    
} // namespace airline

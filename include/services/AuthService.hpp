#pragma once
#include "persistence/PassengerRepo.hpp"
#include "persistence/UserRepo.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "domain/Passenger.hpp"
#include <memory>
#include <string>

namespace airline {

class AuthService {
private:
    PassengerRepository& passengerRepo_;
    UserRepository<Administrator>& adminRepo_;
    UserRepository<BookingAgent>& agentRepo_;

    // True if `username` already belongs to ANY account, staff or
    // passenger. Login has to search all three repos, so registration
    // needs to check all three too -- otherwise a passenger could
    // self-register with the same username as an existing staff
    // account, and login's first-match-wins order would silently hide
    // one of them.
    bool usernameExists(const std::string& username) const;

public:
    AuthService(PassengerRepository& passengerRepo,
                UserRepository<Administrator>& adminRepo,
                UserRepository<BookingAgent>& agentRepo);

    // Checks username/password against passengers, then administrators,
    // then booking agents, in that order. Returns nullptr on any failure
    // (unknown username OR wrong password) -- deliberately not
    // distinguishing the two in the return value, so a caller can't be
    // used to enumerate valid usernames by checking which failure mode
    // came back.
    std::shared_ptr<User> login(const std::string& username, const std::string& password) const;

    // Self-registration -- passengers only, per the spec (staff are
    // provisioned via the data file, not through this). Returns nullptr
    // if the username is already taken by anyone. `plainPassword` is
    // hashed internally before storage; the caller never needs to call
    // hashPassword() itself.
    std::shared_ptr<Passenger> registerPassenger(std::string name, contactInfo contact,
                                                   std::string username, const std::string& plainPassword);
};

}  // namespace airline
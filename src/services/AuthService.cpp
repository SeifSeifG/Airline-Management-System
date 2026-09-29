#include "services/AuthService.hpp"
#include "services/PasswordHasher.hpp"
#include <utility>

namespace airline {

namespace {

// NOTE: sequential ID generation, seeded only from what's currently in
// the repository. This is a known simplification, not a finished
// answer: if a passenger is ever removed, or if the data file's loaded
// IDs don't follow this exact "P" + number scheme, this can collide
// with an existing ID. Fine for now (nothing removes passengers yet),
// but flagging it as the first thing to revisit if IDs start colliding
// once deletion or a different ID scheme exists.
std::string generatePassengerId(const PassengerRepository& repo) {
    return "P" + std::to_string(repo.size() + 1);
}

}  // namespace

AuthService::AuthService(PassengerRepository& passengerRepo,
                          UserRepository<Administrator>& adminRepo,
                          UserRepository<BookingAgent>& agentRepo)
    : passengerRepo_(passengerRepo), adminRepo_(adminRepo), agentRepo_(agentRepo) {}

bool AuthService::usernameExists(const std::string& username) const {
    return passengerRepo_.findByUsername(username) != nullptr ||
           adminRepo_.findByUsername(username) != nullptr ||
           agentRepo_.findByUsername(username) != nullptr;
}

std::shared_ptr<User> AuthService::login(const std::string& username, const std::string& password) const {
    std::string attemptHash = PasswordHasher::hashPassword(password);

    if (auto passenger = passengerRepo_.findByUsername(username)) {
        return passenger->checkPassword(attemptHash) ? passenger : nullptr;
    }
    if (auto admin = adminRepo_.findByUsername(username)) {
        return admin->checkPassword(attemptHash) ? admin : nullptr;
    }
    if (auto agent = agentRepo_.findByUsername(username)) {
        return agent->checkPassword(attemptHash) ? agent : nullptr;
    }
    return nullptr;
}

std::shared_ptr<Passenger> AuthService::registerPassenger(std::string name, contactInfo contact,
                                                            std::string username, const std::string& plainPassword) {
    if (usernameExists(username)) {
        return nullptr;
    }

    std::string id = generatePassengerId(passengerRepo_);
    auto passenger = std::make_shared<Passenger>(id, std::move(name), 
    std::move(contact),username,  
    PasswordHasher::hashPassword(plainPassword), 0); // initial balance to zero

    passengerRepo_.add(passenger->getId(), passenger);
    return passenger;
}

}  // namespace airline
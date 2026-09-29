#include "domain/BookingAgent.hpp"
#include <iostream>
#include <iomanip>

namespace airline {

int BookingAgent::nextId = 1;

BookingAgent::BookingAgent(PersonId_t id, std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword) 
        :User(std::move(id), std::move(name), std::move(contactInfo),
        std::move(username), std::move(hashedPassword), Role::BookingAgent) {}
BookingAgent::BookingAgent(std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword) 
        :User(this->generateId(), std::move(name), std::move(contactInfo),
        std::move(username), std::move(hashedPassword), Role::BookingAgent) {}

Role BookingAgent::getRole() const {
    return Role::BookingAgent;
}

// Overridden ID generator ("AGxxx")
std::string BookingAgent::generateId() {
    std::ostringstream oss;
    oss << "AG" << std::setw(3) << std::setfill('0') << nextId++;
    return oss.str();
}

void BookingAgent::setNextId(int id){
    nextId = id;
}

};

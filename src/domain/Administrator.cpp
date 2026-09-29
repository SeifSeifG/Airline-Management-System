#include "domain/Administrator.hpp"
#include <iostream>
#include <iomanip>

namespace airline {

// Initialize static ID counter
int Administrator::nextId = 1;

Administrator::Administrator(PersonId_t id, std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword)
    : User(std::move(id), std::move(name), std::move(contactInfo),
           std::move(username), std::move(hashedPassword), Role::Administrator) {}


Administrator::Administrator(std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword)
    : User(this->generateId(), std::move(name), std::move(contactInfo),
           std::move(username), std::move(hashedPassword), Role::Administrator) {}


Role Administrator::getRole() const {
    return Role::Administrator;
}

std::string Administrator::generateId() {
        std::ostringstream oss;
        oss << "AD" << std::setw(3) << std::setfill('0') << nextId++;
        return oss.str();
}

void Administrator::setNextId(int id){
    nextId = id;
}

}  // namespace airline

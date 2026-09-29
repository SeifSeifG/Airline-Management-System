#pragma once
#include "domain/User.hpp"

namespace airline {

class Administrator : public User {
private:
    static int nextId;
    std::string generateId() override ;
public:
    Administrator(PersonId_t id, std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword);
    Administrator(std::string name, contactInfo contactInfo,
                      std::string username, std::string hashedPassword);
    Role getRole() const override;
    static void setNextId(int id);

};

}  // namespace airline

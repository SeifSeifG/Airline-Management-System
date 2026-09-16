#pragma once
#include "domain/Person.hpp"
#include "domain/Enums.hpp"

namespace airline {

// A Person who can authenticate and act through the system.
class User : public Person {
public:
    User(std::string id, std::string name, std::string contactInfo,
         std::string username, std::string hashedPassword, Role role);
    ~User() override = default;

    const std::string& getUsername() const;
    Role getRole() const;

    bool checkPassword(const std::string& hashedAttempt) const;

    // Each role presents a different console menu.
    virtual void displayMenu() const = 0;

protected:
    std::string username_;
    std::string hashedPassword_;
    Role role_;
};

}  // namespace airline

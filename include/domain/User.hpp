#pragma once
#include "domain/Person.hpp"
#include "domain/Enums.hpp"

namespace airline {

// A Person who can authenticate and act through the system.
class User : public Person {
protected:
    std::string username_;
    std::string hashedPassword_;
    Role role_;
public:
User(
    std::string id,                     // sink parameter: caller's string is
                                        // either moved in (temporary/std::move)
                                        // or copied in (lvalue caller still needs)
    std::string name,                   // same pattern as id
    contactInfo contactInfo,     // read-only borrow: User never takes
                                        // ownership here, just reads it to
                                        // copy into contactInfo_ below
    std::string username,               // sink parameter, same as id/name
    std::string hashedPassword,         // sink parameter, same as id/name
    Role role                           // Role is an enum -- trivially cheap
                                        // to copy, no ownership question at all
);
    ~User() override = default;

    const std::string& getUsername() const;
    
    bool checkPassword(std::string_view hashedAttempt) const;
    
    // Each role presents a different console menu.
    virtual void displayMenu() const = 0;
    virtual Role getRole() const = 0;


};

}  // namespace airline

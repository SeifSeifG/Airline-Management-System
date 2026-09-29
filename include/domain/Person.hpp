#pragma once
#include <string>
#include "domain/Defs.hpp"

namespace airline {

struct contactInfo{
    std::string email;
    std::string phone;
};

// Common identity for anyone in the domain, whether or not they log in.
class Person {
    protected:
        std::string id_;
        std::string name_;
        contactInfo contactInfo_;
public:
Person(
        PersonId_t id,                     // sink parameter: caller's string is
                                            // either moved in (temporary/std::move)
                                            // or copied in (lvalue caller still needs)
        std::string name,                    // same pattern as id
        contactInfo contactInfo             // sink parameter: caller's struct is
    );    
    virtual ~Person() = default;

    const std::string& getId() const;
    const std::string& getName() const;
    const contactInfo& getContactInfo() const;

    void setContactInfo(const contactInfo& contactInfo);
    void setName(const std::string& name);
};

}  // namespace airline

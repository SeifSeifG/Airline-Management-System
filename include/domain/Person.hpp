#pragma once
#include <string>

namespace airline {

// Common identity for anyone in the domain, whether or not they log in.
class Person {
public:
    Person(std::string id, std::string name, std::string contactInfo);
    virtual ~Person() = default;

    const std::string& getId() const;
    const std::string& getName() const;
    const std::string& getContactInfo() const;

    void setContactInfo(std::string contactInfo);

protected:
    std::string id_;
    std::string name_;
    std::string contactInfo_;
};

}  // namespace airline

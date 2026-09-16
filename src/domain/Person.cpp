#include "domain/Person.hpp"
#include <utility>

namespace airline {

Person::Person(std::string id, std::string name, std::string contactInfo)
    : id_(std::move(id)), name_(std::move(name)), contactInfo_(std::move(contactInfo)) {}

const std::string& Person::getId() const { return id_; }
const std::string& Person::getName() const { return name_; }
const std::string& Person::getContactInfo() const { return contactInfo_; }

void Person::setContactInfo(std::string contactInfo) {
    contactInfo_ = std::move(contactInfo);
}

}  // namespace airline

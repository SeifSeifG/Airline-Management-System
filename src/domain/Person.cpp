#include "domain/Person.hpp"
#include <utility>

namespace airline {

Person::Person(PersonId_t id, std::string name, contactInfo contactInfo)
    : id_(std::move(id)), name_(std::move(name)), contactInfo_(std::move(contactInfo)) {}

const std::string& Person::getId() const { return id_; }
const std::string& Person::getName() const { return name_; }
const contactInfo& Person::getContactInfo() const { return contactInfo_; }

void Person::setContactInfo(const contactInfo& contactInfo) {
    contactInfo_ = contactInfo;
}
void Person::setName(const std::string& name){
    name_ = name;
}

}  // namespace airline

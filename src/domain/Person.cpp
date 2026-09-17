#include "domain/Person.hpp"
#include <utility>

namespace airline {

Person::Person(std::string id, std::string name, contactInfo contactInfo)
    : id_(std::move(id)), name_(std::move(name)), contactInfo_(std::move(contactInfo)) {}


void Person::setId(std::string id) { id_ = std::move(id); }
void Person::setName(std::string name) { name_ = std::move(name); }
void Person::setContactInfo(contactInfo contactInfo) { contactInfo_ = std::move(contactInfo); }

const std::string& Person::getId() const { return id_; }
const std::string& Person::getName() const { return name_; }
const contactInfo& Person::getContactInfo() const { return contactInfo_; }

void Person::setContactInfo(const contactInfo& contactInfo) {
    contactInfo_ = contactInfo;
}

}  // namespace airline

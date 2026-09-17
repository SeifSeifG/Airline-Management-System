#include "domain/Administrator.hpp"
#include <iostream>

namespace airline {

void Administrator::displayMenu() const {
    // TODO: replace with the real console menu in the UI layer.
    std::cout << "[Administrator menu placeholder]\n";
}

Role Administrator::getRole() const {
    return Role::Administrator;
}

}  // namespace airline

#pragma once
#include <string>

namespace airline {

// Forward declaration of AirlineApplication to avoid circular dependencies
class AirlineApplication; 

class Loader {
public:
    // Takes the application instance by reference to populate its private members
    static void loadFromJson(AirlineApplication& app, const std::string& filePath);
};

} // namespace airline
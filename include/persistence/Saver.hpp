#pragma once
#include <string>

namespace airline {

class AirlineApplication;

class Saver {
public:
    // Writes the current in-memory state of every repository back to a
    // JSON file, in the same schema Loader reads. See Saver.cpp for
    // exactly what is and isn't captured yet.
    static void saveToJson(const AirlineApplication& app, const std::string& filePath);
};

}  // namespace airline
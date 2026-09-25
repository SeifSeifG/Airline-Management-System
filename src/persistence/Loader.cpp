#include "persistence/Loader.hpp"
#include "app/AirlineApplication.hpp"
#include "domain/Aircraft.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/Pilot.hpp"
#include "domain/FlightAttendant.hpp"
#include "domain/Passenger.hpp"
#include "domain/Defs.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace airline {

using json = nlohmann::json;

namespace {

SeatLayout buildSeatLayout(const json& seatJson) {
    SeatLayout layout;
    int ecoWindow = seatJson.value("economyWindow", 0);
    int ecoMiddle = seatJson.value("economyMiddle", 0);
    int ecoAisle  = seatJson.value("economyAisle", 0);
    int bizWindow = seatJson.value("businessWindow", 0);
    int bizMiddle = seatJson.value("businessMiddle", 0);
    int firstCnt  = seatJson.value("first", 0);

    if (ecoWindow > 0) layout.addSeats(SeatClass::Economy, SeatPosition::Window, ecoWindow);
    if (ecoMiddle > 0) layout.addSeats(SeatClass::Economy, SeatPosition::Middle, ecoMiddle);
    if (ecoAisle  > 0) layout.addSeats(SeatClass::Economy, SeatPosition::Aisle, ecoAisle);
    if (bizWindow > 0) layout.addSeats(SeatClass::Business, SeatPosition::Window, bizWindow);
    if (bizMiddle > 0) layout.addSeats(SeatClass::Business, SeatPosition::Middle, bizMiddle);
    if (firstCnt  > 0) layout.addSeats(SeatClass::First, SeatPosition::Window, firstCnt);

    return layout;
}

contactInfo buildContactInfo(const json& entryJson) {
    return contactInfo{ entryJson.at("email").get<std::string>(),
                        entryJson.at("phone").get<std::string>() };
}

const json& requireArray(const json& root, const char* key) {
    if (!root.contains(key)) {
        throw std::runtime_error(std::string("data file is missing required section: ") + key);
    }
    if (!root.at(key).is_array()) {
        throw std::runtime_error(std::string("section '") + key + "' must be a JSON array");
    }
    return root.at(key);
}

}  // namespace

void Loader::loadFromJson(AirlineApplication& app, const std::string& filePath) {
    std::ifstream in(filePath);
    if (!in.is_open()) {
        throw std::runtime_error("could not open data file: " + filePath);
    }

    json root;
    try {
        in >> root;
    } catch (const json::parse_error& e) {
        throw std::runtime_error(std::string("malformed JSON in data file: ") + e.what());
    }

    // ---- Aircraft ----
    for (const auto& entry : requireArray(root, "aircraft")) {
        try {
            std::string tailNumber = entry.at("tailNumber").get<std::string>();
            std::string model = entry.at("model").get<std::string>();
            float maxHours = entry.at("maxRunningHours").get<float>();
            SeatLayout layout = buildSeatLayout(entry.at("seatLayout"));

            auto aircraft = std::make_shared<Aircraft>(tailNumber, model, maxHours, std::move(layout));
            app.aircraftRepo_.add(tailNumber, aircraft); // Accessing private member via friend
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed aircraft entry: " << e.what() << "\n";
        }
    }

    // ---- Pilots ----
    for (const auto& entry : requireArray(root, "pilots")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string licenseId = entry.at("licenseId").get<std::string>();
            contactInfo contact = buildContactInfo(entry);

            auto pilot = std::make_shared<Pilot>(id, name, contact, licenseId);
            app.pilots_.add(id, pilot); // Accessing private member via friend
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed pilot entry: " << e.what() << "\n";
        }
    }

    // ---- Flight attendants ----
    for (const auto& entry : requireArray(root, "flightAttendants")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string licenseId = entry.at("licenseId").get<std::string>();
            contactInfo contact = buildContactInfo(entry);

            auto fa = std::make_shared<FlightAttendant>(id, name, contact, licenseId);
            app.flightAtts_.add(id, fa); // Accessing private member via friend
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed flight attendant entry: " << e.what() << "\n";
        }
    }

    // ---- Administrators ----
    for (const auto& entry : requireArray(root, "administrators")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string username = entry.at("username").get<std::string>();
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                // Already hashed -- this is a re-load of previously saved state.
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                // Plaintext -- this is a fresh account from a seed/setup file.
                hashedPassword = hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }
            contactInfo contact = buildContactInfo(entry);

            auto admin = std::make_shared<Administrator>(id, name, contact, username,
                                                        hashedPassword, Role::Administrator);
            app.admins_.add(id, admin); // Accessing private member via friend
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed administrator entry: " << e.what() << "\n";
        }
    }

    // ---- Booking agents ----
    for (const auto& entry : requireArray(root, "bookingAgents")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string username = entry.at("username").get<std::string>();
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                // Already hashed -- this is a re-load of previously saved state.
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                // Plaintext -- this is a fresh account from a seed/setup file.
                hashedPassword = hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }
            contactInfo contact = buildContactInfo(entry);

            auto agent = std::make_shared<BookingAgent>(id, name, contact, username,
                                                          hashedPassword, Role::BookingAgent);
            app.bookingAgents_.add(id, agent); // Accessing private member via friend
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed booking agent entry: " << e.what() << "\n";
        }
    }

    // ---- Passengers ----
    for (const auto& entry : requireArray(root, "passengers")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string username = entry.at("username").get<std::string>();
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                hashedPassword = hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }
            contactInfo contact = buildContactInfo(entry);

            auto passenger = std::make_shared<Passenger>(id, name, contact, username, hashedPassword);
            app.passengerRepo_.add(id, passenger); // Accessing private member via friend
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed passenger entry: " << e.what() << "\n";
        }
    }
}

} // namespace airline
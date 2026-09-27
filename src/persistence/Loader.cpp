#include "persistence/Loader.hpp"
#include "app/AirlineApplication.hpp"
#include "services/PasswordHasher.hpp"
#include "domain/Aircraft.hpp"
#include "domain/Flight.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/Pilot.hpp"
#include "domain/FlightAttendant.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "domain/Passenger.hpp"
#include "domain/Defs.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <map>

namespace airline {

using json = nlohmann::json;

namespace {

Date parseDate(const std::string& dateStr) {
    if (dateStr.empty()) {
        return Date(0, 0, 1, 1, 1970);
    }
    
    // Attempt standard Date constructor via string_view
    try {
        return Date(dateStr);
    } catch (...) {
        // Fallback: manually parse YYYY-MM-DD format
        int year = 1970, month = 1, day = 1;
        char c1 = '-', c2 = '-';
        std::istringstream iss(dateStr);
        if (iss >> year >> c1 >> month >> c2 >> day) {
            return Date(0, 0, day, month, year); // min, hour, day, month, year
        }
    }
    return Date(0, 0, 1, 1, 1970);
}

FlightStatus parseFlightStatus(const std::string& statusStr) {
    if (statusStr == "Scheduled") return FlightStatus::Scheduled;
    if (statusStr == "Delayed")   return FlightStatus::Delayed;
    if (statusStr == "Departed")  return FlightStatus::Departed;
    if (statusStr == "Cancelled") return FlightStatus::Cancelled;
    return FlightStatus::Scheduled;
}

// Rebuilds SeatLayout from either array format (Saver.cpp) or object format (legacy seed)
SeatLayout buildSeatLayout(const json& seatJson, PassengerRepository& passengerRepo) {
    if (seatJson.is_array()) {
        // Full snapshot format (from Saver): each entry has its own id,
        // class, position, and -- if occupied -- which passenger. Use it
        // directly rather than discarding it down to tier counts.
        std::vector<PreExistingSeat> existingSeats;
        for (const auto& seatEntry : seatJson) {
            if (!seatEntry.contains("seatId")) {
                continue;  // malformed entry -- skip, don't abort the whole aircraft
            }
            std::string seatId = seatEntry.at("seatId").get<std::string>();

            // Class/position are implicit in the id itself -- no need for
            // the JSON to carry them separately.
            auto seatClass = SeatLayout::getClassById(seatId);
            auto position = SeatLayout::getPositionById(seatId);
            if (!seatClass || !position) {
                continue;  // unrecognized id format -- skip this seat
            }

            PreExistingSeat seat;
            seat.id = seatId;
            seat.seatClass = *seatClass;
            seat.position = *position;
            seat.passenger = nullptr;

            if (seatEntry.value("occupied", false) && seatEntry.contains("passengerId")) {
                std::string passengerId = seatEntry.at("passengerId").get<std::string>();
                seat.passenger = passengerRepo.get(passengerId);  // nullptr if not found --
                                                                    // treated as unoccupied,
                                                                    // not an abort condition
            }
            existingSeats.push_back(std::move(seat));
        }
        return SeatLayout(existingSeats);
    } else if (seatJson.is_object()) {
        // Legacy tier-count format (hand-authored seed data): no specific
        // ids given, just "N seats of this tier" -- generate fresh ones.
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
    return SeatLayout{};
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
            
            const json& seatData = entry.contains("seats") ? entry.at("seats") : entry.at("seatLayout");
            SeatLayout layout = buildSeatLayout(seatData, app.passengerRepo_);

            auto aircraft = std::make_shared<Aircraft>(tailNumber, model, maxHours, std::move(layout));
            app.aircraftRepo_.add(tailNumber, aircraft);
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
            float flightHours = entry.value("flightHours", 0.0f); 

            auto pilot = std::make_shared<Pilot>(id, name, contact, licenseId, flightHours);
            app.pilots_.add(id, pilot);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed pilot entry: " << e.what() << "\n";
        }
    }

    // ---- Flight Attendants ----
    for (const auto& entry : requireArray(root, "flightAttendants")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string licenseId = entry.at("licenseId").get<std::string>();
            contactInfo contact = buildContactInfo(entry);
            float flightHours = entry.value("flightHours", 0.0f); 

            auto fa = std::make_shared<FlightAttendant>(id, name, contact, licenseId, flightHours);
            app.flightAtts_.add(id, fa);
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
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                hashedPassword = PasswordHasher::hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }
            contactInfo contact = buildContactInfo(entry);

            auto admin = std::make_shared<Administrator>(id, name, contact, username,
                                                        hashedPassword, Role::Administrator);
            app.admins_.add(id, admin);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed administrator entry: " << e.what() << "\n";
        }
    }

    // ---- Booking Agents ----
    for (const auto& entry : requireArray(root, "bookingAgents")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            std::string username = entry.at("username").get<std::string>();
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                hashedPassword = PasswordHasher::hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }
            contactInfo contact = buildContactInfo(entry);

            auto agent = std::make_shared<BookingAgent>(id, name, contact, username,
                                                        hashedPassword, Role::BookingAgent);
            app.bookingAgents_.add(id, agent);
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
                hashedPassword = PasswordHasher::hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }
            contactInfo contact = buildContactInfo(entry);

            auto passenger = std::make_shared<Passenger>(id, name, contact, username, hashedPassword);
            if (entry.contains("loyaltyPoints")) {
                passenger->earnLoyaltyPoints(entry.at("loyaltyPoints").get<int>());
            }
            app.passengerRepo_.add(id, passenger);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed passenger entry: " << e.what() << "\n";
        }
    }

    // ---- Flights ----
    if (root.contains("flights") && root.at("flights").is_array()) {
        for (const auto& entry : root.at("flights")) {
            try {
                std::string flightNumber = entry.at("flightNumber").get<std::string>();
                std::string origin = entry.at("origin").get<std::string>();
                std::string destination = entry.at("destination").get<std::string>();
                Date date = parseDate(entry.at("date").get<std::string>());
                float duration = entry.value("durationHours", 0.0f);

                // Pass regulations (uses default CrewRegulations if not specified in json)
                CrewRegulations regs{0}; // any value
                if (entry.contains("minFlightHours")) {
                    regs.minFlightHours = entry.at("minFlightHours").get<float>();
                }

                auto flight = std::make_shared<Flight>(flightNumber, origin, destination, date, duration, regs);

                if (entry.contains("status")) {
                    flight->setStatus(parseFlightStatus(entry.at("status").get<std::string>()));
                }

                // Attach Aircraft
                if (entry.contains("aircraftTailNumber")) {
                    std::string tail = entry.at("aircraftTailNumber").get<std::string>();
                    if (!tail.empty()) {
                        if (auto ac = app.aircraftRepo_.get(tail)) {
                            flight->assignAircraft(ac);
                        }
                    }
                }

                // Assign Crew Members
                if (entry.contains("assignedCrewIds") && entry.at("assignedCrewIds").is_array()) {
                    for (const auto& crewIdJson : entry.at("assignedCrewIds")) {
                        std::string crewId = crewIdJson.get<std::string>();
                        if (auto pilot = app.pilots_.get(crewId)) {
                            flight->assignCrewMember(pilot);
                        } else if (auto fa = app.flightAtts_.get(crewId)) {
                            flight->assignCrewMember(fa);
                        }
                    }
                }

                // Restore Occupied Seats & Passenger Bookings
                if (entry.contains("seats") && entry.at("seats").is_array()) {
                    for (const auto& seatEntry : entry.at("seats")) {
                        if (seatEntry.value("occupied", false) && seatEntry.contains("passengerId")) {
                            std::string seatId = seatEntry.at("seatId").get<std::string>();
                            std::string passengerId = seatEntry.at("passengerId").get<std::string>();
                            if (auto passenger = app.passengerRepo_.get(passengerId)) {
                                flight->assignSeat(seatId, passenger);
                            }
                        }
                    }
                }

                app.flightRepo_.add(flightNumber, flight);
            } catch (const std::exception& e) {
                std::cerr << "Skipping malformed flight entry: " << e.what() << "\n";
            }
        }
    }
}

} // namespace airline
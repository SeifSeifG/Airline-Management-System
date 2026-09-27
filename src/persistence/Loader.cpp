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

namespace airline {

using json = nlohmann::json;

namespace {

Date parseDate(const std::string& dateStr) {
    if (dateStr.empty()) {
        return Date(0, 0, 1, 1, 1970);
    }
    return Date(std::string_view(dateStr));
}

FlightStatus parseFlightStatus(const std::string& statusStr) {
    if (statusStr == "Scheduled") return FlightStatus::Scheduled;
    if (statusStr == "Delayed")   return FlightStatus::Delayed;
    if (statusStr == "Departed")  return FlightStatus::Departed;
    if (statusStr == "Cancelled") return FlightStatus::Cancelled;
    return FlightStatus::Scheduled;
}

SeatLayout buildSeatLayout(const json& seatJson, PassengerRepository& passengerRepo) {
    if (seatJson.is_array()) {
        std::vector<PreExistingSeat> existingSeats;
        for (const auto& seatEntry : seatJson) {
            std::string seatId;
            if (seatEntry.contains("seatId")) {
                seatId = seatEntry.at("seatId").get<std::string>();
            } else {
                continue;
            }

            auto seatClass = SeatLayout::getClassById(seatId);
            if (!seatClass) continue;

            PreExistingSeat seat;
            seat.id = seatId;
            seat.seatClass = *seatClass;
            seat.passenger = nullptr;

            if (seatEntry.contains("passengerId")) {
                std::string passengerId = seatEntry.at("passengerId").get<std::string>();
                seat.passenger = passengerRepo.get(passengerId);
            }
            existingSeats.push_back(std::move(seat));
        }
        return SeatLayout(existingSeats);
    } else if (seatJson.is_object()) {
        SeatLayout layout;
        int eco = seatJson.value("economy", 0);
        int biz = seatJson.value("business", 0);
        int firstCnt  = seatJson.value("first", 0);

        if (eco > 0) layout.addSeats(SeatClass::Economy, eco);
        if (biz > 0) layout.addSeats(SeatClass::Business, biz);
        if (firstCnt  > 0) layout.addSeats(SeatClass::First, firstCnt);
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

} // namespace

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


    // ---- 1. Administrators ----
    for (const auto& entry : requireArray(root, "administrators")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            contactInfo contact = buildContactInfo(entry);
            std::string username = entry.at("username").get<std::string>();
            std::string password;
            if (entry.contains("passwordHash")) {
                std::string hashedPassword = entry.at("passwordHash").get<std::string>();
                password = PasswordHasher::deHashPassword(hashedPassword);
            } else if (entry.contains("password")) {
                password = entry.at("password").get<std::string>();
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }

            auto admin = std::make_shared<Administrator>(id, name, contact, username,
                                                        password, Role::Administrator);
            app.admins_.add(id, admin);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed administrator entry: " << e.what() << "\n";
        }
    }

    // ---- 2. Booking Agents ----
    for (const auto& entry : requireArray(root, "bookingAgents")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            contactInfo contact = buildContactInfo(entry);
            std::string username = entry.at("username").get<std::string>();
            std::string password;
            if (entry.contains("passwordHash")) {
                std::string hashedPassword = entry.at("passwordHash").get<std::string>();
                password = PasswordHasher::deHashPassword(hashedPassword);
            } else if (entry.contains("password")) {
                password = entry.at("password").get<std::string>();
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }

            auto agent = std::make_shared<BookingAgent>(id, name, contact, username,
                                                        password, Role::BookingAgent);
            app.bookingAgents_.add(id, agent);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed booking agent entry: " << e.what() << "\n";
        }
    }

    // ---- 3. Passengers (Must be loaded BEFORE Aircraft & Flights) ----
    for (const auto& entry : requireArray(root, "passengers")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            contactInfo contact = buildContactInfo(entry);
            std::string username = entry.at("username").get<std::string>();
            std::string password;
            if (entry.contains("passwordHash")) {
                std::string hashedPassword = entry.at("passwordHash").get<std::string>();
                password = PasswordHasher::deHashPassword(hashedPassword);
            } else if (entry.contains("password")) {
                password = entry.at("password").get<std::string>();
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }

            auto passenger = std::make_shared<Passenger>(id, name, contact, username, password);
            passenger->earnLoyaltyPoints(entry.at("loyaltyPoints").get<int>());

            app.passengerRepo_.add(id, passenger);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed passenger entry: " << e.what() << "\n";
        }
    }

    // ---- 4. Pilots ----
    for (const auto& entry : requireArray(root, "pilots")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            contactInfo contact = buildContactInfo(entry);
            std::string licenseId = entry.at("licenseId").get<std::string>();
            float flightHours = entry.value("flightHours", 0.0f);

            auto pilot = std::make_shared<Pilot>(id, name, contact, licenseId, flightHours);
            app.pilots_.add(id, pilot);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed pilot entry: " << e.what() << "\n";
        }
    }

    // ---- 5. Flight Attendants ----
    for (const auto& entry : requireArray(root, "flightAttendants")) {
        try {
            std::string id = entry.at("id").get<std::string>();
            std::string name = entry.at("name").get<std::string>();
            contactInfo contact = buildContactInfo(entry);
            std::string licenseId = entry.at("licenseId").get<std::string>();
            float flightHours = entry.value("flightHours", 0.0f);

            auto fa = std::make_shared<FlightAttendant>(id, name, contact, licenseId, flightHours);
            app.flightAtts_.add(id, fa);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed flight attendant entry: " << e.what() << "\n";
        }
    }

    // ---- 6. Aircraft ----
    for (const auto& entry : requireArray(root, "aircraft")) {
        try {
            std::string tailNumber = entry.at("tailNumber").get<std::string>();
            std::string model = entry.at("model").get<std::string>();
            float runningHours = entry.at("runningHours").get<float>();
            float maxRunningHours = entry.at("maxRunningHours").get<float>();


            const json& seatData = entry.contains("seats") ? entry.at("seats") : entry.at("seatLayout");
            SeatLayout layout = buildSeatLayout(seatData, app.passengerRepo_);

            auto aircraft = std::make_shared<Aircraft>(tailNumber, model, maxRunningHours, std::move(layout));
            aircraft->addRunningHours(runningHours);
            app.aircraftRepo_.add(tailNumber, aircraft);
        } catch (const std::exception& e) {
            std::cerr << "Skipping malformed aircraft entry: " << e.what() << "\n";
        }
    }

    // ---- 7. Flights ----
    if (root.contains("flights") && root.at("flights").is_array()) {
        for (const auto& entry : root.at("flights")) {
            try {
                std::string flightNumber = entry.at("flightNumber").get<std::string>();
                std::string origin = entry.at("origin").get<std::string>();
                std::string destination = entry.at("destination").get<std::string>();
                
                // Parse Date (works with parseDate or fromString<Date>(...))
                Date date = parseDate(entry.at("date").get<std::string>());
                float duration = entry.value("durationHours", 0.0f);

                // Parse crew regulations from the array format: [{"minFlightHours": 1}]
                CrewRegulations regs{0};
                if (entry.contains("crewRegulations") && entry.at("crewRegulations").is_array() && !entry.at("crewRegulations").empty()) {
                    const auto& regObj = entry.at("crewRegulations")[0];
                    if (regObj.contains("minFlightHours")) {
                        regs.minFlightHours = regObj.at("minFlightHours").get<int>();
                    }
                }

                auto flight = std::make_shared<Flight>(flightNumber, origin, destination, date, duration, regs);

                if (entry.contains("status")) {
                    flight->setStatus(parseFlightStatus(entry.at("status").get<std::string>()));
                }

                if (entry.contains("aircraftTailNumber")) {
                    std::string tail = entry.at("aircraftTailNumber").get<std::string>();
                    if (!tail.empty()) {
                        if (auto ac = app.aircraftRepo_.get(tail)) {
                            flight->setAircraft(ac);
                        }
                    }
                }

                // Parse assigned pilots from "assignedPilots"
                if (entry.contains("assignedPilots") && entry.at("assignedPilots").is_array()) {
                    for (const auto& pilotIdJson : entry.at("assignedPilots")) {
                        std::string pilotId = pilotIdJson.get<std::string>();
                        if (auto pilot = app.pilots_.get(pilotId)) {
                            flight->setCrewMember(pilot);
                        }
                    }
                }

                // Parse assigned flight attendants from "assignedFlightAttendants"
                if (entry.contains("assignedFlightAttendants") && entry.at("assignedFlightAttendants").is_array()) {
                    for (const auto& faIdJson : entry.at("assignedFlightAttendants")) {
                        std::string faId = faIdJson.get<std::string>();
                        if (auto fa = app.flightAtts_.get(faId)) {
                            flight->setCrewMember(fa);
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
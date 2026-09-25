#include "persistence/Saver.hpp"
#include "app/AirlineApplication.hpp"
#include "domain/Aircraft.hpp"
#include "domain/SeatLayout.hpp"
#include "domain/Pilot.hpp"
#include "domain/FlightAttendant.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "domain/Passenger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <stdexcept>

namespace airline {

using json = nlohmann::json;

namespace {

// Saver.cpp — replace seatLayoutToJson entirely
json seatLayoutToJson(const Aircraft& aircraft) {
    json seatsJson = json::array();
    for (const auto& [seatId, seatData] : aircraft.getAllSeats()) {
        json seatEntry = {
            {"seatId", seatId},
            {"occupied", seatData.isOccupied()}
        };
        if (seatData.isOccupied()) {
            // NOTE: this is the new consequence -- reconstructing an
            // occupied seat on load means Loader needs to look the
            // passenger up by id and call assignSeat with them, AFTER
            // passengers have already been loaded. That's an ordering
            // dependency Loader doesn't have today (seats are currently
            // built directly from tier counts, with no passenger
            // references at all). Flag if you want this wired up now
            // or left as a known gap alongside flights/reservations.
            seatEntry["passengerId"] = seatData.passenger->getId();
        }
        seatsJson.push_back(seatEntry);
    }
    return seatsJson;
}

template <typename CrewT>
json crewToJson(const std::shared_ptr<CrewT>& member) {
    return {
        {"id", member->getId()},
        {"name", member->getName()},
        {"email", member->getContactInfo().email},
        {"phone", member->getContactInfo().phone},
        {"licenseId", member->getLicenseId()}
    };
}

template <typename StaffT>
json staffToJson(const std::shared_ptr<StaffT>& user) {
    return {
        {"id", user->getId()},
        {"name", user->getName()},
        {"email", user->getContactInfo().email},
        {"phone", user->getContactInfo().phone},
        {"username", user->getUsername()},
        // NOTE: writes back whatever is currently stored as the hashed
        // password. Harmless today because hashPassword() is an identity
        // stub. The moment real hashing is implemented, this becomes a
        // real bug: Loader re-hashes whatever it reads from "password",
        // so a value that's already hashed would get hashed AGAIN on the
        // next load, silently breaking every saved account's login.
        // Fix needed at that point: either Loader gains a "this file
        // contains pre-hashed passwords" mode, or Saver and Loader agree
        // on a distinct field name (e.g. "passwordHash") that Loader
        // stores directly instead of re-hashing.
        {"password", user->getHashedPassword()}
    };
}

}  // namespace

void Saver::saveToJson(const AirlineApplication& app, const std::string& filePath) {
    json root;

    json aircraftJson = json::array();
    for (const auto& ac : app.aircraftRepo_.getAll()) {
        aircraftJson.push_back({
            {"tailNumber", ac->getTailNumber()},
            {"model", ac->getModel()},
            {"maxRunningHours", ac->getMaxRunningHours()},
            {"seats", seatLayoutToJson(*ac)}   // pass the Aircraft itself, not its seat map
        });
    }
    root["aircraft"] = aircraftJson;

    json pilotsJson = json::array();
    for (const auto& p : app.pilots_.getAll()) {
        pilotsJson.push_back(crewToJson(p));
    }
    root["pilots"] = pilotsJson;

    json flightAttsJson = json::array();
    for (const auto& fa : app.flightAtts_.getAll()) {
        flightAttsJson.push_back(crewToJson(fa));
    }
    root["flightAttendants"] = flightAttsJson;

    json adminsJson = json::array();
    for (const auto& admin : app.admins_.getAll()) {
        adminsJson.push_back(staffToJson(admin));
    }
    root["administrators"] = adminsJson;

    json agentsJson = json::array();
    for (const auto& agent : app.bookingAgents_.getAll()) {
        agentsJson.push_back(staffToJson(agent));
    }
    root["bookingAgents"] = agentsJson;

    json passengersJson = json::array();
    for (const auto& passenger : app.passengerRepo_.getAll()) {
        json entry = staffToJson(passenger);
        // NOTE: loyalty points ARE captured here, for completeness --
        // but Loader does not currently read this field back. A
        // passenger's earned points are lost on the next load unless
        // Loader is extended to call earnLoyaltyPoints() when present.
        entry["loyaltyPoints"] = passenger->getLoyaltyBalance();
        passengersJson.push_back(entry);
    }
    root["passengers"] = passengersJson;

    // NOTE: flights and reservations are not saved. Neither Loader nor
    // Saver currently has a schema for them -- FlightRepository stays
    // empty across a save/load cycle. This mirrors Loader's current
    // scope exactly; extending both together is a natural next step
    // once Flight/Reservation are further along.

    std::ofstream out(filePath);
    if (!out.is_open()) {
        throw std::runtime_error("could not open file for writing: " + filePath);
    }
    out << root.dump(4);  // pretty-printed, 4-space indent
}

}  // namespace airline
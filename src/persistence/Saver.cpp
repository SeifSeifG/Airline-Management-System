#include "persistence/Saver.hpp"
#include "app/AirlineApplication.hpp"
#include "domain/Aircraft.hpp"
#include "domain/Flight.hpp"
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

// Optional ADL serializer for Date if used directly with json assignment
template <typename T>
void to_json(json& j, const T& d) {
    j = d.toString();
}

namespace {

// Serializes a raw seats map
json seatMapToJson(const std::unordered_map<SeatId_t, SeatData>& seatsMap) {
    json seatsJson = json::array();
    for (const auto& [seatId, seatData] : seatsMap) {
        json seatEntry = {
            {"seatId", seatId},
            {"occupied", seatData.isOccupied()}
        };

        if (seatData.isOccupied() && seatData.passenger) {
            seatEntry["passengerId"] = seatData.passenger->getId();
        }
        seatsJson.push_back(seatEntry);
    }
    return seatsJson;
}

// Overload 2: Accepts std::unordered_map<SeatId_t, SeatData> directly
json seatLayoutToJson(const std::unordered_map<SeatId_t, SeatData>& seatsMap) {
    return seatMapToJson(seatsMap);
}

json flightToJson(const std::shared_ptr<Flight>& flight) {
    json j;
    j["flightNumber"] = flight->getFlightNumber();
    j["origin"] = flight->getOrigin();
    j["destination"] = flight->getDestination();

    // 1. Convert Date via std::ostringstream
    std::ostringstream oss;
    oss << flight->getDate();
    j["date"] = oss.str();

    j["durationHours"] = flight->getDuration();

    // 2. Exact enum status matching
    switch (flight->getStatus()) {
        case FlightStatus::Scheduled: j["status"] = "Scheduled"; break;
        case FlightStatus::Delayed:   j["status"] = "Delayed"; break;
        case FlightStatus::Departed:  j["status"] = "Departed"; break;
        case FlightStatus::Cancelled: j["status"] = "Cancelled"; break;
    }

    // Aircraft Tail Number
    if (auto ac = flight->getAircraft()) {
        j["aircraftTailNumber"] = ac->getTailNumber();
    } else {
        j["aircraftTailNumber"] = "";
    }

    // Regulations
    j["minFlightHours"] = flight->getRegulations().minFlightHours;

    // 3. Iterate pilots and flight attendants separately
    json crewIds = json::array();
    
    // Adjust getter method names if different in Flight.hpp (e.g. getPilots() / getFlightAtts())
    for (const auto& weakPilot : flight->getPilots()) {
        if (auto pilot = weakPilot.lock()) {
            crewIds.push_back(pilot->getId());
        }
    }
    for (const auto& weakFA : flight->getFAs()) {
        if (auto fa = weakFA.lock()) {
            crewIds.push_back(fa->getId());
        }
    }
    j["assignedCrewIds"] = crewIds;

    // Flight Seats
    j["seats"] = seatLayoutToJson(flight->getAllSeats());

    return j;
}

template <typename CrewT>
json crewToJson(const std::shared_ptr<CrewT>& member) {
    return {
        {"id", member->getId()},
        {"name", member->getName()},
        {"email", member->getContactInfo().email},
        {"phone", member->getContactInfo().phone},
        {"licenseId", member->getLicenseId()},
        {"flightHours", member->getFlightHours()}
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
        {"passwordHash", user->getHashedPassword()}
    };
}

}  // namespace

void Saver::saveToJson(const AirlineApplication& app, const std::string& filePath) {
    json root;

    // ---- Aircraft ----
    json aircraftJson = json::array();
    for (const auto& ac : app.aircraftRepo_.getAll()) {
        aircraftJson.push_back({
            {"tailNumber", ac->getTailNumber()},
            {"model", ac->getModel()},
            {"maxRunningHours", ac->getMaxRunningHours()},
            {"seats", seatLayoutToJson(ac->getAllSeats())}
        });
    }
    root["aircraft"] = aircraftJson;

    // ---- Pilots ----
    json pilotsJson = json::array();
    for (const auto& p : app.pilots_.getAll()) {
        pilotsJson.push_back(crewToJson(p));
    }
    root["pilots"] = pilotsJson;

    // ---- Flight Attendants ----
    json flightAttsJson = json::array();
    for (const auto& fa : app.flightAtts_.getAll()) {
        flightAttsJson.push_back(crewToJson(fa));
    }
    root["flightAttendants"] = flightAttsJson;

    // ---- Administrators ----
    json adminsJson = json::array();
    for (const auto& admin : app.admins_.getAll()) {
        adminsJson.push_back(staffToJson(admin));
    }
    root["administrators"] = adminsJson;

    // ---- Booking Agents ----
    json agentsJson = json::array();
    for (const auto& agent : app.bookingAgents_.getAll()) {
        agentsJson.push_back(staffToJson(agent));
    }
    root["bookingAgents"] = agentsJson;

    // ---- Passengers ----
    json passengersJson = json::array();
    for (const auto& passenger : app.passengerRepo_.getAll()) {
        json entry = staffToJson(passenger);
        entry["loyaltyPoints"] = passenger->getLoyaltyBalance();
        passengersJson.push_back(entry);
    }
    root["passengers"] = passengersJson;

    // ---- Flights ----
    json flightsJson = json::array();
    for (const auto& flight : app.flightRepo_.getAll()) {
        flightsJson.push_back(flightToJson(flight));
    }
    root["flights"] = flightsJson;

    std::ofstream out(filePath);
    if (!out.is_open()) {
        throw std::runtime_error("could not open file for writing: " + filePath);
    }
    out << root.dump(4);
}

}  // namespace airline
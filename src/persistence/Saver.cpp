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
#include <sstream>
#include <unordered_map>
#include <stdexcept>

namespace airline {

using json = nlohmann::json;

namespace {

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

template <typename StaffT> // admin and booking agent
json UserToJson(const std::shared_ptr<StaffT>& user) {
    return {
        {"id", user->getId()},
        {"name", user->getName()},
        {"email", user->getContactInfo().email},
        {"phone", user->getContactInfo().phone},
        {"username", user->getUsername()},
        {"passwordHash", user->getHashedPassword()}
    };
}

// Serializes a raw seats map
json seatMapToJson(const std::unordered_map<SeatId_t, SeatData>& seatsMap) {
    json seatsJson = json::array();
    for (const auto& [seatId, seatData] : seatsMap) {
        json seatEntry = {
            {"seatId", seatId},
            {"occupied", seatData.isOccupied()}
        };

        if (seatData.isOccupied()) {
            seatEntry["passengerId"] = seatData.passenger->getId();
        }
        seatsJson.push_back(seatEntry);
    }
    return seatsJson;
}

json aircraftToJson(const std::shared_ptr<Aircraft>& aircraft) {
    return {
        {"tailNumber", aircraft->getTailNumber()},
        {"model", aircraft->getModel()},
        {"runningHours", aircraft->getRunningHours()},
        {"maxRunningHours", aircraft->getMaxRunningHours()},
        {"maintenanceStatus", myToString(aircraft->getMaintenanceStatus())},
        {"seats", seatMapToJson(aircraft->getAllSeats())}
    };
}

template <typename T>
nlohmann::json crewIdToJson(const std::vector<std::weak_ptr<T>>& crewVector) {
    nlohmann::json jsonArray = nlohmann::json::array();
    for (const auto& weakCrew : crewVector) {
        if (auto crew = weakCrew.lock()) { // Safely promote weak_ptr to shared_ptr
            jsonArray.push_back(crew->getId());
        }
    }
    return jsonArray;
}

nlohmann::json crewRegToJson(const CrewRegulations& regs) {
    return {
        { {"minFlightHours", regs.minFlightHours} }
    };
}

nlohmann::json flightToJson(const std::shared_ptr<Flight>& flight) {
    return {
        {"flightNumber", flight->getFlightNumber()},
        {"origin", flight->getOrigin()},
        {"destination", flight->getDestination()},
        {"date", myToString(flight->getDate())},
        {"durationHours", flight->getDuration()},
        {"aircraftTailNumber", flight->getAircraft()->getTailNumber()},
        {"assignedPilots", crewIdToJson(flight->getPilots())},
        {"assignedFlightAttendants", crewIdToJson(flight->getFAs())},
        {"crewRegulations", crewRegToJson(flight->getRegulations())},
        {"status", myToString(flight->getStatus())}
    };
}

}  // namespace

void Saver::saveToJson(const AirlineApplication& app, const std::string& filePath) {
    json root;

    // ---- Administrators ----
    json adminsJson = json::array();
    for (const auto& admin : app.admins_.getAll()) {
        adminsJson.push_back(UserToJson(admin));
    }
    root["administrators"] = adminsJson;

    // ---- Booking Agents ----
    json agentsJson = json::array();
    for (const auto& agent : app.bookingAgents_.getAll()) {
        agentsJson.push_back(UserToJson(agent));
    }
    root["bookingAgents"] = agentsJson;

    // ---- Passengers ----
    json passengersJson = json::array();
    for (const auto& passenger : app.passengerRepo_.getAll()) {
        json entry = UserToJson(passenger);
        entry["loyaltyPoints"] = passenger->getLoyaltyBalance();
        passengersJson.push_back(entry);
    }
    root["passengers"] = passengersJson;

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

    // ---- Aircraft ----
    json aircraftJson = json::array();
    for (const auto& ac : app.aircraftRepo_.getAll()) {
        aircraftJson.push_back(aircraftToJson(ac));
    }
    root["aircraft"] = aircraftJson;

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
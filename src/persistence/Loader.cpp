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
    return fromString<FlightStatus>(statusStr).value();
}

ReservationStatus parseReservationStatus(const std::string& statusStr) {
    return fromString<ReservationStatus>(statusStr).value();
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

// BookingRequest and CheckInRequest have the same shape, so one helper builds either.
// The repositories come in as parameters because this free function is not a friend
// of AirlineApplication (same reason buildSeatLayout takes passengerRepo).
template <typename RequestT>
std::shared_ptr<RequestT> buildRequest(const json& entry,
                                       PassengerRepository& passengerRepo,
                                       FlightRepository& flightRepo) {
    std::string passengerId = entry.at("passengerId").get<std::string>();
    std::string flightNumber = entry.at("flightNumber").get<std::string>();

    auto passenger = passengerRepo.get(passengerId);
    if (!passenger) {
        throw std::runtime_error("unknown passenger id '" + passengerId + "'");
    }
    auto flight = flightRepo.get(flightNumber);
    if (!flight) {
        throw std::runtime_error("unknown flight number '" + flightNumber + "'");
    }

    auto req = std::make_shared<RequestT>();
    req->id = entry.at("id").get<std::string>();
    req->passenger = passenger;
    req->flight = flight;

    if (entry.contains("seatClass")) {
        req->seatClass = fromString<SeatClass>(entry.at("seatClass").get<std::string>()).value();
    }

    if (entry.contains("price")) {
        req->price = entry.at("price").get<int>();
    }

    if (entry.contains("status")) {
        req->status = parseReservationStatus(entry.at("status").get<std::string>());
    }

    return req;
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
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                hashedPassword = PasswordHasher::hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }

            auto admin = std::make_shared<Administrator>(id, name, contact, username, hashedPassword);
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
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                hashedPassword = PasswordHasher::hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }

            auto agent = std::make_shared<BookingAgent>(id, name, contact, username, hashedPassword);
            app.agentRepo_.add(id, agent);
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
            std::string hashedPassword;
            if (entry.contains("passwordHash")) {
                hashedPassword = entry.at("passwordHash").get<std::string>();
            } else if (entry.contains("password")) {
                hashedPassword = PasswordHasher::hashPassword(entry.at("password").get<std::string>());
            } else {
                throw std::runtime_error("entry missing both 'password' and 'passwordHash'");
            }

            // Parse balance (defaults to 0 if missing from JSON)
            int balance = entry.value("balance", 0);

            // Pass balance as the 6th argument to Passenger constructor
            auto passenger = std::make_shared<Passenger>(id, name, contact, username, hashedPassword, balance);

            if (entry.contains("loyaltyPoints")) {
                passenger->earnLoyaltyPoints(entry.at("loyaltyPoints").get<int>());
            }

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

                // Parse base price (default to 100 if missing from JSON)
                int basePrice = entry.value("basePrice", 100);

                // Parse crew regulations from the array format: [{"minFlightHours": 1}]
                CrewRegulations regs{0};
                if (entry.contains("crewRegulations") && entry.at("crewRegulations").is_array() && !entry.at("crewRegulations").empty()) {
                    const auto& regObj = entry.at("crewRegulations")[0];
                    if (regObj.contains("minFlightHours")) {
                        regs.minFlightHours = regObj.at("minFlightHours").get<int>();
                    }
                }

                // Pass basePrice as the 7th argument to the Flight constructor
                auto flight = std::make_shared<Flight>(flightNumber, origin, destination, date, duration, basePrice, regs);

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
    
    // ---- 8. Booking Requests (must come after passengers AND flights) ----
    // Cleared first: repositories replace entries by id, but a vector appends,
    // so loading twice would otherwise double every request.
    app.bookingRequests_.clear();
    if (root.contains("bookingRequests") && root.at("bookingRequests").is_array()) {
        for (const auto& entry : root.at("bookingRequests")) {
            try {
                auto req = buildRequest<BookingRequest>(entry, app.passengerRepo_, app.flightRepo_);
                app.bookingRequests_.push_back(req);

                // Lock the weak_ptr to safely invoke passenger methods
                if (auto passenger = req->passenger.lock()) {
                    passenger->addBookingReq(req);
                }
            } catch (const std::exception& e) {
                std::cerr << "Skipping malformed booking request entry: " << e.what() << "\n";
            }
        }
    }

    // ---- 9. Check-In Requests ----
    app.checkInRequests_.clear();
    if (root.contains("checkInRequests") && root.at("checkInRequests").is_array()) {
        for (const auto& entry : root.at("checkInRequests")) {
            try {
                auto req = buildRequest<CheckInRequest>(entry, app.passengerRepo_, app.flightRepo_);
                app.checkInRequests_.push_back(req);

                // Lock the weak_ptr to safely invoke passenger methods
                if (auto passenger = req->passenger.lock()) {
                    passenger->addCheckInReq(req);
                }
            } catch (const std::exception& e) {
                std::cerr << "Skipping malformed check-in request entry: " << e.what() << "\n";
            }
        }
    }

    // ---- 10. Finished Requests ----
    if (root.contains("finishedRequests") && root.at("finishedRequests").is_array()) {
        for (const auto& entry : root.at("finishedRequests")) {
            try {
                auto req = std::make_shared<FinishedRequest>();
                
                req->checkInId     = entry.value("checkInId", "");
                req->passengerId   = entry.value("passengerId", "");
                req->passengerName = entry.value("passengerName", "");
                req->flightNumber  = entry.value("flightNumber", "");
                req->origin        = entry.value("origin", "");
                req->destination   = entry.value("destination", "");
                req->departureDate = entry.value("departureDate", "");

                if (entry.contains("seatClass")) {
                    req->seatClass = fromString<SeatClass>(entry.at("seatClass").get<std::string>()).value();
                }

                if (entry.contains("price")) {
                    req->price = entry.at("price").get<int>();
                }

                if (entry.contains("reservationStatus")) {
                    req->reservationStatus = parseReservationStatus(entry.at("reservationStatus").get<std::string>());
                }

                if (entry.contains("paymentStatus")) {
                    req->paymentStatus = fromString<PaymentStatus>(entry.at("paymentStatus").get<std::string>()).value();
                }

                // Add to repository
                app.finishedRequests_.push_back(req);

                // Re-link to passenger's travel history for UI lookup
                if (!req->passengerId.empty()) {
                    if (auto passenger = app.passengerRepo_.get(req->passengerId)) {
                        passenger->addTravelHistory(req);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "Skipping malformed finished request entry: " << e.what() << "\n";
            }
        }
    }
}

} // namespace airline
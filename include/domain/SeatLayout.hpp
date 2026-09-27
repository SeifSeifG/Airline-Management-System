#pragma once
#include "domain/Defs.hpp"
#include <unordered_map>
#include <deque>
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include <tuple>

namespace airline {

class Passenger;

struct SeatData {
    std::shared_ptr<Passenger> passenger = nullptr;

    bool isOccupied() const { return passenger != nullptr; }
    void assignPassenger(std::shared_ptr<Passenger> newPassenger) {
        passenger = std::move(newPassenger);
    }
    std::shared_ptr<Passenger> freePassenger() {
        return passenger ? std::move(passenger) : nullptr;
    }
};

// Struct used when loading/restoring pre-existing seats
struct PreExistingSeat {
    SeatId_t id;
    SeatClass seatClass;
    std::shared_ptr<Passenger> passenger = nullptr;
};

class SeatLayout {
private:
    // Canonical storage: every seat ever created, by decorative seat id
    std::unordered_map<SeatId_t, SeatData> seats_;

    // Free seat ids per class tier. assignSeat pops front, freeSeat pushes back (O(1)).
    std::unordered_map<SeatClass, std::deque<SeatId_t>> available_;

    // Counter used to generate sequential decorative IDs per class (A1, A2... / B1, B2... / C1, C2...)
    std::unordered_map<SeatClass, int> classSeatCount_;

    int firstClassCapacity_ = 0;
    int businessClassCapacity_ = 0;
    int economyClassCapacity_ = 0;

    int occupiedFirstClass_ = 0;
    int occupiedBusinessClass_ = 0;
    int occupiedEconomyClass_ = 0;

    void bumpCapacity(SeatClass seatClass, int count);
    void bumpOccupied(SeatClass seatClass, int delta);

public:
    SeatLayout() = default;
    explicit SeatLayout(const std::vector<PreExistingSeat>& existingSeats);

    // Determines class from the starting prefix letter ('A' = First, 'B' = Business, 'C' = Economy)
    static std::optional<SeatClass> getClassById(const SeatId_t& seatId);

    // Bulk-creates `count` seats for a class, generating sequential IDs (e.g. A1, A2... B1, B2... C1, C2...)
    std::optional<std::vector<SeatId_t>> addSeats(SeatClass seatClass, int count);

    // Finds the next available seat ID for a class without assigning it
    std::shared_ptr<SeatId_t> findSeat(SeatClass seatClass) const;

    // Assigns the first available seat in the specified class
    std::optional<SeatData> assignSeat(SeatClass seatClass, std::shared_ptr<Passenger> passenger);

    // Assigns a specific seat ID to a passenger
    std::optional<SeatData> assignSeat(const SeatId_t& id, std::shared_ptr<Passenger> passenger);

    // Frees a seat by ID and returns the passenger who was in it
    std::shared_ptr<Passenger> freeSeat(const SeatId_t& seatId);

    // Read-only access for reporting / seat-map display
    const std::unordered_map<SeatId_t, SeatData>& getAllSeats() const;

    std::tuple<int, int, int> getAvailableSeatsPerClass() const;

    int getFirstClassCapacity() const;
    int getBusinessClassCapacity() const;
    int getEconomyClassCapacity() const;

    int getOccupiedFirstClass() const;
    int getOccupiedBusinessClass() const;
    int getOccupiedEconomyClass() const;

    int getTotalCapacity() const;
    int getTotalAssigned() const;
    int getTotalAvailable() const;
};

}  // namespace airline
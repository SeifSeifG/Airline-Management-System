#pragma once
#include "domain/Defs.hpp"
#include <unordered_map>
#include <deque>
#include <optional>
#include <memory>
#include <array>
#include <vector>
#include <string>

namespace airline {

constexpr std::array<char, 2> eco_window_ = {'A', 'F'};
constexpr std::array<char, 2> eco_middle_ = {'B', 'E'};
constexpr std::array<char, 2> eco_aisle_ = {'C', 'D'};

constexpr std::array<char, 2> bis_window_ = {'P', 'Q'};
constexpr std::array<char, 2> bis_middle_ = {'S', 'L'};

constexpr char firstClassId = 'X';

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

class SeatLayout {
private:
    // Canonical storage: every seat ever created, by id. mainly for ui display later
    std::unordered_map<SeatId_t, SeatData> seats_;

    // Free seat ids per (class, position) tier. assignSeat pops the front,
    // freeSeat pushes the back *** both O(1). ***
    std::unordered_map<SeatClass, std::unordered_map<SeatPosition, std::deque<SeatId_t>>> available_;

    // How many seats have ever been generated per tier. Only grows, used
    // solely to continue the P1/Q1/P2/Q2 letter-row sequence correctly if
    // addSeats is called more than once for the same tier.
    std::unordered_map<SeatClass, std::unordered_map<SeatPosition, int>> groupSeatCount_;

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

    static std::optional<SeatClass> getClassById(const SeatId_t& seatId);
    static std::optional<SeatPosition> getPositionById(const SeatId_t& seatId);

    // Bulk-creates `count` seats in the given tier, generating sequential
    // ids (business+window: P1, Q1, P2, Q2, ...). Called once per tier
    // while reading the "N seats of each tier" input file.
    // Returns the generated ids, or nullopt if (class, position) isn't a
    // real tier in this layout (e.g. Business+Aisle doesn't exist).
    std::optional<std::vector<SeatId_t>> addSeats(SeatClass seatClass, SeatPosition position, int count);

    std::shared_ptr<SeatId_t> findSeat(SeatClass seatClass, SeatPosition position) const;

    // Assigns the first available seat in (class, position). Returns the
    // assigned seat's data (id + passenger) on success, nullopt if none
    // are free. Fallback logic (try another tier, waitlist, etc.) is the
    // booking agent's responsibility, not this class's.
    std::optional<SeatData> assignSeat(SeatClass seatClass, SeatPosition position,
                                        std::shared_ptr<Passenger> passenger);

    // Frees a seat by id, returning the passenger who was in it
    // (nullptr if the id doesn't exist or wasn't occupied).
    std::shared_ptr<Passenger> freeSeat(const SeatId_t& seatId);

    // Read-only access for reporting / seat-map display.
    const std::unordered_map<SeatId_t, SeatData>& getAllSeats() const;

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
#include "domain/SeatLayout.hpp"
#include <cctype>
#include <utility>
#include <algorithm>

namespace {

template <std::size_t N>
bool contains(const std::array<char, N>& arr, char letter) {
    return (arr[0] == letter || arr[1] == letter);
}

// Which letters belong to a given (class, position) tier, in the order
// they alternate. First class has one letter, so the vector has size 1.
std::optional<std::array<char, 2>> lettersFor(airline::SeatClass seatClass, airline::SeatPosition position) {
    using airline::SeatClass;
    using airline::SeatPosition;

    if (seatClass == SeatClass::Economy) {
        if (position == SeatPosition::Window) return airline::eco_window_;
        if (position == SeatPosition::Middle) return airline::eco_middle_;
        if (position == SeatPosition::Aisle)  return airline::eco_aisle_;
    } else if (seatClass == SeatClass::Business) {
        if (position == SeatPosition::Window) return airline::bis_window_;
        if (position == SeatPosition::Middle) return airline::bis_middle_;
    } else if (seatClass == SeatClass::First) {
        if (position == SeatPosition::Window) {
            return std::array<char, 2>{airline::firstClassId, airline::firstClassId};
        }
    }
    return std::nullopt;
}

}  // namespace

namespace airline {

SeatLayout::SeatLayout(const std::vector<PreExistingSeat>& existingSeats) {
    for (const auto& seat : existingSeats) {
        seats_[seat.id] = SeatData{seat.passenger};
        bumpCapacity(seat.seatClass, 1);

        if (seat.passenger) {
            bumpOccupied(seat.seatClass, 1);
        } else {
            available_[seat.seatClass][seat.position].push_back(seat.id);
        }

        // Reconstruct groupSeatCount_ from the id itself, so a LATER
        // addSeats() call on this tier continues the sequence instead of
        // restarting from 0 and colliding with these reloaded ids.
        auto lettersOpt = lettersFor(seat.seatClass, seat.position);
        if (lettersOpt && !seat.id.empty()) {
            const auto& letters = *lettersOpt;
            char letter = std::toupper(static_cast<unsigned char>(seat.id.back()));
            std::string rowPart = seat.id.substr(0, seat.id.size() - 1);

            try {
                int rowNumber = std::stoi(rowPart);
                // Reverse of addSeats' formula. NOTE: for First class,
                // letters[0] == letters[1] (both 'X') -- see the caveat below.
                int letterIndex = (letter == letters[0]) ? 0 : 1;
                int impliedSeatsPerTier = (rowNumber - 1) * static_cast<int>(letters.size()) + letterIndex + 1;

                int& seatsPerTier = groupSeatCount_[seat.seatClass][seat.position];
                seatsPerTier = std::max(seatsPerTier, impliedSeatsPerTier);
            } catch (...) {
                // malformed id (non-numeric row part) -- skip sequence
                // tracking for this seat rather than aborting the reload
            }
        }
    }
}

std::optional<SeatClass> SeatLayout::getClassById(const SeatId_t& seatId) {
    if (seatId.empty()) return std::nullopt;
    char letter = std::toupper(static_cast<unsigned char>(seatId.back()));

    if (contains(eco_window_, letter) || contains(eco_aisle_, letter) || contains(eco_middle_, letter)) {
        return SeatClass::Economy;
    }
    if (contains(bis_window_, letter) || contains(bis_middle_, letter)) {
        return SeatClass::Business;
    }
    if (letter == firstClassId) {
        return SeatClass::First;
    }
    return std::nullopt;
}

std::optional<SeatPosition> SeatLayout::getPositionById(const SeatId_t& seatId) {
    if (seatId.empty()) return std::nullopt;
    char letter = std::toupper(static_cast<unsigned char>(seatId.back()));

    if (contains(eco_window_, letter) || contains(bis_window_, letter) || letter == firstClassId) {
        return SeatPosition::Window;
    }
    if (contains(eco_aisle_, letter)) {
        return SeatPosition::Aisle;
    }
    if (contains(eco_middle_, letter) || contains(bis_middle_, letter)) {
        return SeatPosition::Middle;
    }
    return std::nullopt;
}

void SeatLayout::bumpCapacity(SeatClass seatClass, int count) {
    switch (seatClass) {
        case SeatClass::First:    firstClassCapacity_    += count; break;
        case SeatClass::Business: businessClassCapacity_ += count; break;
        case SeatClass::Economy:  economyClassCapacity_  += count; break;
        default: break;
    }
}

void SeatLayout::bumpOccupied(SeatClass seatClass, int delta) {
    switch (seatClass) {
        case SeatClass::First:    occupiedFirstClass_    += delta; break;
        case SeatClass::Business: occupiedBusinessClass_ += delta; break;
        case SeatClass::Economy:  occupiedEconomyClass_  += delta; break;
        default: break;
    }
}


std::optional<std::vector<SeatId_t>> SeatLayout::addSeats(SeatClass seatClass, SeatPosition position, int count) {

    // Look up which letters belong to this (class, position) tier (Business+Window -> {P, Q})
    auto lettersOpt = lettersFor(seatClass, position);
    if (!lettersOpt || count <= 0) {
        return std::nullopt;
    }

    const auto& letters = lettersOpt.value();

    // groupSeatCount_[seatClass][position] default-constructs to 0 (check cppreference's value-initialization page)
    int& seatsPerTier = groupSeatCount_[seatClass][position];

    // this reference lets the loop below push newly-created seats onto the
    auto& freeSeatsQueue = available_[seatClass][position];

    // Collect every id generated in this call, to return to the caller
    // useful for logging
    std::vector<SeatId_t> generated;

    //since we already know the final size -- avoids the vector reallocatoin
    generated.reserve(count);

    // Generate exactly "count" new seats in this tier, one per iteration.
    for (int i = 0; i < count; ++i) {

        // Pick which letter this seat gets by alternating through the
        // tier's letter set. "seatsPerTier % letters.size()"
        char letter = letters[seatsPerTier % letters.size()];

        // you may think it doesn't work for first class, but it does LOL.
        int rowNumber = static_cast<int>(seatsPerTier / letters.size()) + 1;

        // Build the actual seat id string, e.g. letter='P', rowNumber=3
        // -> "P3". std::string(1, letter) constructs a one-character
        // string from a char (there's no implicit char->string
        // conversion, so this explicit constructor call is required).
        SeatId_t seatId = std::string(1, letter) + std::to_string(rowNumber);

        ++seatsPerTier;

        // Build the actual seat record.
        SeatData seat;

        // Insert the seat into the canonical storage, keyed by id.
        seats_.emplace(seatId, std::move(seat));

        // Mark this seat as available by adding its id to the tier's
        freeSeatsQueue.push_back(seatId);

        // Record this id in the method's return value too. std::move
        // here avoids one last string copy.
        generated.push_back(std::move(seatId));
    }

    bumpCapacity(seatClass, count);
    return generated;
}


std::shared_ptr<SeatId_t> SeatLayout::findSeat(SeatClass seatClass, SeatPosition position) const {

    auto classIt = available_.find(seatClass);
    if (classIt == available_.end()) {
        return nullptr;
    }
    
    auto posIt = classIt->second.find(position);
    if (posIt == classIt->second.end() || posIt->second.empty()) {
        return nullptr;  // tier exists but nothing's free right now
    }

    const SeatId_t& seatId = posIt->second.front();
    return std::make_shared<SeatId_t>(seatId);
}


std::optional<SeatData> SeatLayout::assignSeat(SeatClass seatClass, SeatPosition position, std::shared_ptr<Passenger> passenger) {
    
    // if no seat with this class is available
    auto classIt = available_.find(seatClass);
    if (classIt == available_.end()) return std::nullopt;

    // if no seat with this position is available
    auto posIt = classIt->second.find(position);
    if (posIt == classIt->second.end() /*|| posIt->second.empty()*/) return std::nullopt;

    SeatId_t seatId = posIt->second.front();
    posIt->second.pop_front();

    auto& seatData = seats_.at(seatId);
    seatData.assignPassenger(std::move(passenger));
    bumpOccupied(seatClass, +1);

    return seatData;  // copy: cheap -- one shared_ptr + one string
}

std::optional<SeatData> SeatLayout::assignSeat(const SeatId_t& id, std::shared_ptr<Passenger> passenger) {
    auto it = seats_.find(id);
    if (it == seats_.end()) {
        return std::nullopt;  // no such seat -- .at() would throw here instead; find() lets
                               // this overload fail the same way the other one does (nullopt),
                               // rather than throwing where its sibling doesn't
    }
    auto& seatData = it->second;
    if (seatData.passenger != nullptr) {
        return std::nullopt;  // already occupied -- refuse, don't overwrite
    }

    auto seatClass = getClassById(id);
    auto position = getPositionById(id);
    seatData.assignPassenger(std::move(passenger));
    if (seatClass) {
        bumpOccupied(seatClass.value(), +1);
    }

    // Keep available_ in sync -- without this, the same id could still be
    // handed out by the class/position overload above.
    if (seatClass && position) {
        auto& freeQueue = available_[seatClass.value()][position.value()];
        freeQueue.erase(std::remove(freeQueue.begin(), freeQueue.end(), id), freeQueue.end());
    }

    return seatData;
}

std::shared_ptr<Passenger> SeatLayout::freeSeat(const SeatId_t& seatId) {
    auto it = seats_.find(seatId);
    if (it == seats_.end() || !it->second.isOccupied()) {
        return nullptr;
    }

    auto passenger = std::move(it->second.passenger);

    auto seatClass = getClassById(seatId);
    auto position = getPositionById(seatId);
    if (seatClass && position) {
        available_[seatClass.value()][position.value()].push_back(seatId);
        bumpOccupied(seatClass.value(), -1);
    }
    return passenger;
}

const std::unordered_map<SeatId_t, SeatData>& SeatLayout::getAllSeats() const {
    return seats_;
}

int SeatLayout::getTierSeatCount(SeatClass seatClass, SeatPosition position) const {
    auto classIt = groupSeatCount_.find(seatClass);
    if (classIt == groupSeatCount_.end()) return 0;
    auto posIt = classIt->second.find(position);
    return posIt != classIt->second.end() ? posIt->second : 0;
}

std::tuple<int, int, int> SeatLayout::getAvailableSeatsPerTier() const {
    int availableFirst = std::max(0, getFirstClassCapacity() - getOccupiedFirstClass());
    int availableBusiness = std::max(0, getBusinessClassCapacity() - getOccupiedBusinessClass());
    int availableEconomy = std::max(0, getEconomyClassCapacity() - getOccupiedEconomyClass());

    return {availableFirst, availableBusiness, availableEconomy};
}

int SeatLayout::getFirstClassCapacity() const { return firstClassCapacity_; }
int SeatLayout::getBusinessClassCapacity() const { return businessClassCapacity_; }
int SeatLayout::getEconomyClassCapacity() const { return economyClassCapacity_; }

int SeatLayout::getOccupiedFirstClass() const { return occupiedFirstClass_; }
int SeatLayout::getOccupiedBusinessClass() const { return occupiedBusinessClass_; }
int SeatLayout::getOccupiedEconomyClass() const { return occupiedEconomyClass_; }

int SeatLayout::getTotalCapacity() const {
    return firstClassCapacity_ + businessClassCapacity_ + economyClassCapacity_;
}
int SeatLayout::getTotalAssigned() const {
    return occupiedFirstClass_ + occupiedBusinessClass_ + occupiedEconomyClass_;
}
int SeatLayout::getTotalAvailable() const {
    return getTotalCapacity() - getTotalAssigned();
}

}  // namespace airline
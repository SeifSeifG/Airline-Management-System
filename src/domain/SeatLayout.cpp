#include "domain/SeatLayout.hpp"
#include <cctype>
#include <utility>
#include <algorithm>

namespace {

char getClassPrefix(airline::SeatClass seatClass) {
    switch (seatClass) {
        case airline::SeatClass::First:    return 'A';
        case airline::SeatClass::Business: return 'B';
        case airline::SeatClass::Economy:  return 'C';
    }
    return 'C';
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
            available_[seat.seatClass].push_back(seat.id);
        }

        // Keep sequence counter in sync with restored seat IDs so future addSeats() calls don't collide
        if (!seat.id.empty()) {
            std::string numPart;
            for (char c : seat.id) {
                if (std::isdigit(static_cast<unsigned char>(c))) {
                    numPart += c;
                }
            }
            if (!numPart.empty()) {
                try {
                    int num = std::stoi(numPart);
                    classSeatCount_[seat.seatClass] = std::max(classSeatCount_[seat.seatClass], num);
                } catch (...) {
                    // Ignore malformed numeric part
                }
            }
        }
    }
}

std::optional<SeatClass> SeatLayout::getClassById(const SeatId_t& seatId) {
    if (seatId.empty()) return std::nullopt;

    char firstChar = static_cast<char>(std::toupper(static_cast<unsigned char>(seatId[0])));
    if (firstChar == 'A') return SeatClass::First;
    if (firstChar == 'B') return SeatClass::Business;
    if (firstChar == 'C') return SeatClass::Economy;

    return std::nullopt;
}

void SeatLayout::bumpCapacity(SeatClass seatClass, int count) {
    switch (seatClass) {
        case SeatClass::First:    firstClassCapacity_    += count; break;
        case SeatClass::Business: businessClassCapacity_ += count; break;
        case SeatClass::Economy:  economyClassCapacity_  += count; break;
    }
}

void SeatLayout::bumpOccupied(SeatClass seatClass, int delta) {
    switch (seatClass) {
        case SeatClass::First:    occupiedFirstClass_    += delta; break;
        case SeatClass::Business: occupiedBusinessClass_ += delta; break;
        case SeatClass::Economy:  occupiedEconomyClass_  += delta; break;
    }
}

std::optional<std::vector<SeatId_t>> SeatLayout::addSeats(SeatClass seatClass, int count) {
    if (count <= 0) return std::nullopt;

    char prefix = getClassPrefix(seatClass);
    int& currentCount = classSeatCount_[seatClass];
    auto& freeSeatsQueue = available_[seatClass];

    std::vector<SeatId_t> generated;
    generated.reserve(count);

    for (int i = 0; i < count; ++i) {
        ++currentCount;
        SeatId_t seatId = std::string(1, prefix) + std::to_string(currentCount);

        seats_.emplace(seatId, SeatData{});
        freeSeatsQueue.push_back(seatId);
        generated.push_back(seatId);
    }

    bumpCapacity(seatClass, count);
    return generated;
}

std::shared_ptr<SeatId_t> SeatLayout::findSeat(SeatClass seatClass) const {
    auto classIt = available_.find(seatClass);
    if (classIt == available_.end() || classIt->second.empty()) {
        return nullptr;
    }
    return std::make_shared<SeatId_t>(classIt->second.front());
}

std::optional<SeatData> SeatLayout::assignSeat(SeatClass seatClass, std::shared_ptr<Passenger> passenger) {
    auto classIt = available_.find(seatClass);
    if (classIt == available_.end() || classIt->second.empty()) {
        return std::nullopt;
    }

    SeatId_t seatId = classIt->second.front();
    classIt->second.pop_front();

    auto& seatData = seats_.at(seatId);
    seatData.assignPassenger(std::move(passenger));
    bumpOccupied(seatClass, +1);

    return seatData;
}

std::optional<SeatData> SeatLayout::assignSeat(const SeatId_t& id, std::shared_ptr<Passenger> passenger) {
    auto it = seats_.find(id);
    if (it == seats_.end()) {
        return std::nullopt;
    }

    auto& seatData = it->second;
    if (seatData.isOccupied()) {
        return std::nullopt;
    }

    auto seatClass = getClassById(id);
    seatData.assignPassenger(std::move(passenger));

    if (seatClass) {
        bumpOccupied(seatClass.value(), +1);
        auto& freeQueue = available_[seatClass.value()];
        freeQueue.erase(std::remove(freeQueue.begin(), freeQueue.end(), id), freeQueue.end());
    }

    return seatData;
}

std::shared_ptr<Passenger> SeatLayout::freeSeat(const SeatId_t& seatId) {
    auto it = seats_.find(seatId);
    if (it == seats_.end() || !it->second.isOccupied()) {
        return nullptr;
    }

    auto passenger = it->second.freePassenger();

    auto seatClass = getClassById(seatId);
    if (seatClass) {
        available_[seatClass.value()].push_back(seatId);
        bumpOccupied(seatClass.value(), -1);
    }
    return passenger;
}

const std::unordered_map<SeatId_t, SeatData>& SeatLayout::getAllSeats() const {
    return seats_;
}

std::tuple<int, int, int> SeatLayout::getAvailableSeatsPerClass() const {
    int availFirst = std::max(0, getFirstClassCapacity() - getOccupiedFirstClass());
    int availBusiness = std::max(0, getBusinessClassCapacity() - getOccupiedBusinessClass());
    int availEconomy = std::max(0, getEconomyClassCapacity() - getOccupiedEconomyClass());

    return {availFirst, availBusiness, availEconomy};
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
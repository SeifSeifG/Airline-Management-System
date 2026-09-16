#include "domain/SeatMap.hpp"
#include <utility>

namespace airline {

SeatMap::SeatMap(std::vector<Seat> seats) : seats_(std::move(seats)) {}

std::optional<std::size_t> SeatMap::findAvailable(SeatClass seatClass) const {
    for (std::size_t i = 0; i < seats_.size(); ++i) {
        if (seats_[i].getSeatClass() == seatClass && !seats_[i].isOccupied()) {
            return i;
        }
    }
    return std::nullopt;
}

std::vector<Seat>& SeatMap::getSeats() { return seats_; }
const std::vector<Seat>& SeatMap::getSeats() const { return seats_; }

}  // namespace airline

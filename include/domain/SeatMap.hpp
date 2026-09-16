#pragma once
#include "domain/Seat.hpp"
#include <optional>
#include <vector>

namespace airline {

class SeatMap {
public:
    explicit SeatMap(std::vector<Seat> seats);

    // Returns the index of the first available seat of the given class,
    // or nullopt if none. Index (not reference) keeps this simple to
    // serialize/deserialize later in the persistence layer.
    std::optional<std::size_t> findAvailable(SeatClass seatClass) const;

    std::vector<Seat>& getSeats();
    const std::vector<Seat>& getSeats() const;

private:
    std::vector<Seat> seats_;
};

}  // namespace airline

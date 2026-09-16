#include "domain/LoyaltyAccount.hpp"

namespace airline {

void LoyaltyAccount::earnPoints(int points) {
    points_ += points;
}

bool LoyaltyAccount::redeem(int points) {
    if (points > points_) return false;
    points_ -= points;
    return true;
}

int LoyaltyAccount::getPoints() const { return points_; }

}  // namespace airline

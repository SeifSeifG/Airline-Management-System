#pragma once

namespace airline {

class LoyaltyAccount {
public:
    void earnPoints(int points);
    bool redeem(int points);
    int getPoints() const;

private:
    int points_ = 0;
};

}  // namespace airline

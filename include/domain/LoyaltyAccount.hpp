#pragma once

namespace airline {

class LoyaltyAccount {
private:
    int points_ = 0;
public:
    void earnPoints(int points);
    bool redeem(int points);
    int getPoints() const;

};

}  // namespace airline

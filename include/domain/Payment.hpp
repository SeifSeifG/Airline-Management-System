#pragma once
#include "domain/Defs.hpp"

namespace airline {

class Payment {
public:
    Payment(float amount, PaymentMethod method);

    bool process();
    bool refund();

    float getAmount() const;
    PaymentStatus getStatus() const;

private:
    float amount_;
    PaymentMethod method_;
    PaymentStatus status_ = PaymentStatus::Pending;
};

}  // namespace airline

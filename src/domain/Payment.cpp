#include "domain/Payment.hpp"

namespace airline {

Payment::Payment(float amount, PaymentMethod method) : amount_(amount), method_(method) {}

bool Payment::process() {
    // Simulated transaction -- real gateway integration happens in the
    // services layer, this just tracks status on the domain object.
    if (status_ != PaymentStatus::Pending) return false;
    status_ = PaymentStatus::Completed;
    return true;
}

bool Payment::refund() {
    if (status_ != PaymentStatus::Completed) return false;
    status_ = PaymentStatus::Refunded;
    return true;
}

float Payment::getAmount() const { return amount_; }
PaymentStatus Payment::getStatus() const { return status_; }

}  // namespace airline

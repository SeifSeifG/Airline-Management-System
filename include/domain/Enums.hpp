#pragma once

namespace airline {

enum class Role { Administrator, BookingAgent, Passenger };
enum class FlightStatus { Scheduled, Delayed, Boarding, Departed, Cancelled, Landed };
enum class ReservationStatus { Pending, Confirmed, CheckedIn, Cancelled };
enum class PaymentStatus { Pending, Completed, Refunded, Failed };
enum class PaymentMethod { CreditCard, DebitCard, Cash };
enum class MaintenanceStatus { Airworthy, InMaintenance, Grounded };
enum class SeatClass { Economy, Business, First };

}  // namespace airline

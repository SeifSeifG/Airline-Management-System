#pragma once
#include <stdexcept>
#include <iostream>

namespace airline {

typedef std::string SeatId_t;
typedef std::string PersonId_t;


enum class Role { Administrator, BookingAgent, Passenger, Pilot, FlightAttendant};

enum class FlightStatus { Scheduled, Delayed, Boarding, Departed, Cancelled, Landed };
enum class SeatClass { Economy, Business, First };
enum class SeatPosition { Window, Aisle, Middle };
enum class MaintenanceStatus { Airworthy, InMaintenance };
enum class ReservationStatus { Pending, Confirmed, CheckedIn, Cancelled };

enum class PaymentStatus { Pending, Completed, Refunded, Failed };
enum class PaymentMethod { CreditCard, DebitCard, Cash };


class Date {
private:
    float hour;
    int day;
    int month;
    int year;
public:
    Date(float h, int d, int m, int y) : hour(h), day(d), month(m), year(y) {
        if (!isValid(h, d, m, y)) {
            throw std::invalid_argument("Invalid date supplied!");
        }
    }

    static bool isLeapYear(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    static bool isValid(float h, int d, int m, int y) {
        // Basic range checks
        if (y < 1) return false;
        if (m < 1 || m > 12) return false;
        if (d < 1) return false;
        if (h < 0 || h > 24) return false;

        // Days in each month (index 0 is unused)
        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        // Adjust for leap year in February
        if (m == 2 && isLeapYear(y)) {
            daysInMonth[2] = 29;
        }

        return d <= daysInMonth[m];
    }

    // Declare the stream insertion operator as a friend
    friend std::ostream& operator<<(std::ostream& os, const Date& dt) {
        // Convert float hour to total minutes (adding 0.5 for safe rounding)
        int totalMinutes = static_cast<int>(dt.hour * 60 + 0.5f);
        int hour24 = (totalMinutes / 60) % 24;
        int minute = totalMinutes % 60;

        // Convert 24-hour to 12-hour format
        int hour12 = hour24 % 12;
        if (hour12 == 0) hour12 = 12;

        // Formats as DD/MM/YYYY : HH:MM AM/PM
        os << (dt.day < 10 ? "0" : "") << dt.day << '/'
        << (dt.month < 10 ? "0" : "") << dt.month << '/'
        << dt.year << " : "
        << hour12 << ':'
        << (minute < 10 ? "0" : "") << minute << ' '
        << (hour24 >= 12 ? "PM" : "AM");

        return os;
    }

};

}  // namespace airline

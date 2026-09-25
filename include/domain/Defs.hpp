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

// 
class Date {
private:
    int min;
    int hour;
    int day;
    int month;
    int year;
public:
    Date(int min, int h, int d, int m, int y) : min(min), hour(h), day(d), month(m), year(y) {
        if (!isValid(min, h, d, m, y)) {
            min = hour = day = month = year = -1;
        }
    }

    static bool isLeapYear(int y) {
        return (y % 4 == 0 && y % 100 != 0) || (y % 400 == 0);
    }

    int getMin() const {return min;}
    int getHour() const { return hour; }
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    static bool isValid(int min,int h, int d, int m, int y) {
        // Basic range checks
        if (min < 0 || min > 60) return false;
        if (y < 1900) return false; // like linux does
        if (m < 1 || m > 12) return false;
        if (d < 1 || d > 31) return false;
        if (h < 0 || h > 24) return false;

        // Days in each month (index 0 is unused)
        int daysInMonth[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

        // Adjust for leap year in February
        if (m == 2 && isLeapYear(y)) {
            daysInMonth[2] = 29;
        }

        return d <= daysInMonth[m];
    }

    bool operator == (const Date& dt){
        return this->year == dt.year
        && this->month == dt.month
        && this->day == dt.day
        && this->hour == dt.hour
        && this->min == dt.min;
    }

    // Declare the stream insertion operator as a friend
    friend std::ostream& operator<<(std::ostream& os, const Date& dt) {
        // Formats as DD/MM/YYYY : HH:MM AM/PM
        
        // Calculate 12-hour format values
        int display_hour = dt.hour % 12;
        if (display_hour == 0) display_hour = 12;
        const char* am_pm = (dt.hour < 12) ? "AM" : "PM";

        os << (dt.day < 10 ? "0" : "") << dt.day << '/'
        << (dt.month < 10 ? "0" : "") << dt.month << '/'
        << dt.year << " : "
        << (display_hour < 10 ? "0" : "") << display_hour << ':'
        << (dt.min < 10 ? "0" : "") << dt.min << ' '
        << am_pm;

        return os;
    }

};

}  // namespace airline

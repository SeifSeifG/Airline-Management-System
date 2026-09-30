#pragma once
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <optional>

namespace airline {

typedef std::string SeatId_t;
typedef std::string PersonId_t;


enum class Role { Administrator, BookingAgent, Passenger, Pilot, FlightAttendant};
inline std::optional<Role> intToRole(int value) {
    switch (value) {
        case 1: return Role::Administrator;
        case 2: return Role::BookingAgent;
        case 3: return Role::Passenger;
        case 4: return Role::Pilot;
        case 5: return Role::FlightAttendant;
        default: return std::nullopt; // Invalid range
    }
}
inline std::ostream& operator<<(std::ostream& os, Role role) {
    switch (role) {
        case Role::Administrator:   return os << "Administrator";
        case Role::BookingAgent:    return os << "BookingAgent";
        case Role::Passenger:       return os << "Passenger";
        case Role::Pilot:           return os << "Pilot";
        case Role::FlightAttendant: return os << "FlightAttendant";
        default:                    return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, Role& role) {
    std::string token;
    if (is >> token) {
        if (token == "Administrator")        role = Role::Administrator;
        else if (token == "BookingAgent")    role = Role::BookingAgent;
        else if (token == "Passenger")       role = Role::Passenger;
        else if (token == "Pilot")           role = Role::Pilot;
        else if (token == "FlightAttendant") role = Role::FlightAttendant;
        else is.setstate(std::ios::failbit);
    }
    return is;
}

enum class FlightStatus { Scheduled, Delayed, Departed, Cancelled };
inline std::ostream& operator<<(std::ostream& os, FlightStatus status) {
    switch (status) {
        case FlightStatus::Scheduled: return os << "Scheduled";
        case FlightStatus::Delayed:   return os << "Delayed";
        case FlightStatus::Departed:  return os << "Departed";
        case FlightStatus::Cancelled: return os << "Cancelled";
        default:                      return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, FlightStatus& status) {
    std::string token;
    if (is >> token) {
        if (token == "Scheduled")      status = FlightStatus::Scheduled;
        else if (token == "Delayed")   status = FlightStatus::Delayed;
        else if (token == "Departed")  status = FlightStatus::Departed;
        else if (token == "Cancelled") status = FlightStatus::Cancelled;
        else is.setstate(std::ios::failbit);
    }
    return is;
}

// SeatClass
enum class SeatClass { Economy, Business, First };
inline std::ostream& operator<<(std::ostream& os, SeatClass seatClass) {
    switch (seatClass) {
        case SeatClass::Economy:  return os << "Economy";
        case SeatClass::Business: return os << "Business";
        case SeatClass::First:    return os << "First";
        default:                  return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, SeatClass& seatClass) {
    std::string token;
    if (is >> token) {
        if (token == "Economy")       seatClass = SeatClass::Economy;
        else if (token == "Business") seatClass = SeatClass::Business;
        else if (token == "First")    seatClass = SeatClass::First;
        else is.setstate(std::ios::failbit);
    }
    return is;
}

// MaintenanceStatus
enum class MaintenanceStatus { Airworthy, InMaintenance };
inline std::ostream& operator<<(std::ostream& os, MaintenanceStatus status) {
    switch (status) {
        case MaintenanceStatus::Airworthy:     return os << "Airworthy";
        case MaintenanceStatus::InMaintenance: return os << "InMaintenance";
        default:                               return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, MaintenanceStatus& status) {
    std::string token;
    if (is >> token) {
        if (token == "Airworthy")        status = MaintenanceStatus::Airworthy;
        else if (token == "InMaintenance") status = MaintenanceStatus::InMaintenance;
        else is.setstate(std::ios::failbit);
    }
    return is;
}

// ReservationStatus
enum class ReservationStatus { Missed ,PendingBook, ConfirmedBook, PendingCheckIn, Confirmed, Cancelled };
inline std::ostream& operator<<(std::ostream& os, ReservationStatus status) {
    switch (status) {
        case ReservationStatus::Missed: return os << "Missed";
        case ReservationStatus::PendingBook:   return os << "PendingBook";
        case ReservationStatus::ConfirmedBook: return os << "ConfirmedBook";
        case ReservationStatus::PendingCheckIn: return os << "PendingCheckIn";
        case ReservationStatus::Confirmed: return os << "Confirmed";
        case ReservationStatus::Cancelled: return os << "Cancelled";
        default:                           return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, ReservationStatus& status) {
    std::string token;
    if (is >> token) {
        if (token == "Missed")        status = ReservationStatus::Missed;
        else if (token == "PendingBook")  status = ReservationStatus::PendingBook;
        else if (token == "ConfirmedBook")  status = ReservationStatus::ConfirmedBook;
        else if (token == "PendingCheckIn")  status = ReservationStatus::PendingCheckIn;
        else if (token == "Confirmed")  status = ReservationStatus::Confirmed;
        else if (token == "Cancelled")  status = ReservationStatus::Cancelled;
        else is.setstate(std::ios::failbit);
    }
    return is;
}

// PaymentStatus
enum class PaymentStatus { Pending, Completed, Refunded, Failed };
inline std::ostream& operator<<(std::ostream& os, PaymentStatus status) {
    switch (status) {
        case PaymentStatus::Pending:   return os << "Pending";
        case PaymentStatus::Completed: return os << "Completed";
        case PaymentStatus::Refunded:  return os << "Refunded";
        case PaymentStatus::Failed:    return os << "Failed";
        default:                       return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, PaymentStatus& status) {
    std::string token;
    if (is >> token) {
        if (token == "Pending")        status = PaymentStatus::Pending;
        else if (token == "Completed")  status = PaymentStatus::Completed;
        else if (token == "Refunded")   status = PaymentStatus::Refunded;
        else if (token == "Failed")     status = PaymentStatus::Failed;
        else is.setstate(std::ios::failbit);
    }
    return is;
}

// PaymentMethod
enum class PaymentMethod { Card, Cash };
inline std::ostream& operator<<(std::ostream& os, PaymentMethod method) {
    switch (method) {
        case PaymentMethod::Card: return os << "Card";
        case PaymentMethod::Cash: return os << "Cash";
        default:                  return os << "Unknown";
    }
}
inline std::istream& operator>>(std::istream& is, PaymentMethod& method) {
    std::string token;
    if (is >> token) {
        if (token == "Card")      method = PaymentMethod::Card;
        else if (token == "Cash") method = PaymentMethod::Cash;
        else is.setstate(std::ios::failbit);
    }
    return is;
}


enum class DepartResult { Success, FlightNotFound, AircraftNotFound, AircraftNotAirworthy, AlreadyDeparted};

template <typename T>
std::string toString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

template <typename T>
std::optional<T> fromString(const std::string& str) {
    std::istringstream iss(str);
    T value{};
    if (iss >> value) {
        return value;
    }
    return std::nullopt; // Returns empty if parsing fails
}

// 
class Date {
private:
    int min{0};
    int hour{0};
    int day{1};
    int month{1};
    int year{2027};
public:
    Date() = default;

    Date(int min, int h, int d, int m, int y) : min(min), hour(h), day(d), month(m), year(y) {
        if (!isValid(min, h, d, m, y)) {
            min = hour = day = month = year = -1;
        }
    }

    // Conversion constructor from std::string_view
    // Supports "YYYY-MM-DD HH:MM" and "YYYY-MM-DD"
    Date(std::string_view str) {
        int y = 0, m = 0, d = 0, h = 0, mn = 0;
        std::string s(str);

        // Parse YYYY-MM-DD HH:MM (like scanf from c but safe version)
        if (std::sscanf(s.c_str(), "%d-%d-%d %d:%d", &y, &m, &d, &h, &mn) == 5) {
            // Parsed date and time
        } 
        // Parse YYYY-MM-DD
        else if (std::sscanf(s.c_str(), "%d-%d-%d", &y, &m, &d) == 3) {
            h = 0;
            mn = 0;
        } 
        // Fallback: Parse DD/MM/YYYY HH:MM
        else if (std::sscanf(s.c_str(), "%d/%d/%d %d:%d", &d, &m, &y, &h, &mn) == 5) {
            // Parsed alternate date format
        }
        // Add this branch to your Date constructor sscanf checks:
        else if (std::sscanf(s.c_str(), "%d/%d/%d : %d:%d", &d, &m, &y, &h, &mn) == 5) {
            // Parsed "DD/MM/YYYY : HH:MM"
        }
        else {
            min = hour = day = month = year = -1;
            return;
        }

        if (isValid(mn, h, d, m, y)) {
            this->min = mn;
            this->hour = h;
            this->day = d;
            this->month = m;
            this->year = y;
        } else {
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

    bool operator == (const Date& dt) const{
        return this->year == dt.year
        && this->month == dt.month
        && this->day == dt.day
        && this->hour == dt.hour
        && this->min == dt.min;
    }

    // Overload > operator for chronologically greater (later) dates
    bool operator>(const Date& rhs) const {
        if (year != rhs.year)   return year > rhs.year;
        if (month != rhs.month) return month > rhs.month;
        if (day != rhs.day)     return day > rhs.day;
        if (hour != rhs.hour)   return hour > rhs.hour;
        return min > rhs.min;
    }

    bool operator<(const Date& rhs) const {
        return rhs > *this;
    }

    // Declare the stream insertion operator as a friend
    friend std::ostream& operator<<(std::ostream& os, const Date& dt) {
        // Formats as DD/MM/YYYY : HH:MM (24-hour format)
        
        os << (dt.day < 10 ? "0" : "") << dt.day << '/'
           << (dt.month < 10 ? "0" : "") << dt.month << '/'
           << dt.year << " : "
           << (dt.hour < 10 ? "0" : "") << dt.hour << ':'
           << (dt.min < 10 ? "0" : "") << dt.min;

        return os;
    }

    // Date (Parses "DD/MM/YYYY : HH:MM")
    friend std::istream& operator>>(std::istream& is, Date& dt) {
        char slash1, slash2, colon1, colon2;
        int d, m, y, h, min;
        if (is >> d >> slash1 >> m >> slash2 >> y >> colon1 >> h >> colon2 >> min) {
            dt.day = d;
            dt.month = m;
            dt.year = y;
            dt.hour = h;
            dt.min = min;
        } else {
            is.setstate(std::ios::failbit);
        }
        return is;
    }

    static Date parseString(const std::string& dateStr) {
        Date d;
        char sep;
        std::stringstream ss(dateStr);
        ss >> d.day >> sep >> d.month >> sep >> d.year;
        if (ss >> sep >> d.hour >> sep >> d.min) {}
        return d;
    }

};

}  // namespace airline

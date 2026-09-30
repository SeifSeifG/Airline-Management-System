#include "ui/ConsoleUI.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <limits>
#include <map>

namespace airline {

using namespace std; // much easier than  everywhere

ConsoleUI::ConsoleUI(AirlineApplication& app) : app_(app) {}

int ConsoleUI::getIntInput(const string& prompt) {
    if (!prompt.empty()) {
        cout << prompt;
    }
    int choice = 0;
    while (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a valid number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return choice;
}

float ConsoleUI::getFloatInput(const string& prompt) {
    string input;
    float value = 0.0f;

    while (true) {
        cout << prompt;
        if (getline(cin, input)) {
            stringstream ss(input);
            // Parse float and ensure no trailing invalid characters exist
            if (ss >> value && (ss >> ws).eof()) {
                return value;
            }
        }
        
        // Handle stream error or invalid conversion
        if (cin.fail()) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }

        cout << "Invalid input. Please enter a valid floating-point number (e.g., 2.5).\n";
    }
}

string ConsoleUI::getStringInput(const string& prompt) {
    if (!prompt.empty()) {
        cout << prompt;
    }
    string input;
    getline(cin, input);
    return input;
}

void ConsoleUI::run() {
    while (running_) {
        showRoleMenu();
    }
}

// ==========================================
// Role Selection & Authentication
// ==========================================

void ConsoleUI::showRoleMenu() {
    cout << "Welcome to Airline Reservation and Management System\n";
    cout << "Please select your role:\n";
    cout << "1. Administrator\n";
    cout << "2. Booking Agent\n";
    cout << "3. Passenger\n";
    cout << "4. Exit\n";

    int choice = getIntInput("Enter choice: ");
    switch (choice) {
        case 1: handleAdminLogin(); break;
        case 2: handleAgentLogin(); break;
        case 3: handlePassengerLogin(); break;
        case 4: running_ = false; break;
        default: cout << "Invalid choice. Please try again.\n\n"; break;
    }
}

void ConsoleUI::handleAdminLogin() {
    cout << "\n--- Administrator Login ---\n";
    auto username = getStringInput("Username: ");
    auto plainPassword = getStringInput("Password: ");

    if (app_.login(username, plainPassword)) {
        if (dynamic_pointer_cast<Administrator>(app_.getCurrentUser())) {
            cout << "\nLogin successful!\n";
            showAdminMenu();
            app_.logout();
            return;
        }
    }

    app_.logout();
    cout << "\nLogin failed. Invalid credentials or unauthorized role access.\n\n";
}

void ConsoleUI::handleAgentLogin() {
    cout << "\n--- Booking Agent Login ---\n";
    auto username = getStringInput("Username: ");
    auto password = getStringInput("Password: ");

    if (app_.login(username, password)) {
        if (dynamic_pointer_cast<BookingAgent>(app_.getCurrentUser())) {
            cout << "\nLogin successful!\n";
            showAgentMenu();
            app_.logout();
            return;
        }
    }

    app_.logout();
    cout << "\nLogin failed. Invalid credentials or unauthorized role access.\n\n";
}

void ConsoleUI::handlePassengerLogin() {
    cout << "\n--- Passenger Login ---\n";
    auto username = getStringInput("Username: ");
    auto password = getStringInput("Password: ");

    if (app_.login(username, password)) {
        if (dynamic_pointer_cast<Passenger>(app_.getCurrentUser())) {
            cout << "\nLogin successful!\n";
            showPassengerMenu();
            app_.logout();
            return;
        }
    }

    app_.logout();
    cout << "\nLogin failed. Invalid credentials or unauthorized role access.\n\n";
}

// ==========================================
// Role Main Menus
// ==========================================

void ConsoleUI::showAdminMenu() {
    bool inMenu = true;
    while (inMenu) {
        cout << "\n--- Administrator Menu ---\n"
                  << "1. Manage Flights\n"
                  << "2. Manage Aircraft\n"
                  << "3. Manage Users\n"
                  << "4. Generate Reports\n"
                  << "5. Logout\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1: showManageFlightsMenu(); break;
            case 2: showManageAircraftMenu(); break;
            case 3: showManageUsersMenu(); break;
            case 4: showGenerateReportsMenu(); break;
            case 5: inMenu = false;  break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

void ConsoleUI::showAgentMenu() {
    bool inMenu = true;
    while (inMenu) {
        cout << "\n--- Booking Agent Menu ---\n"
                  << "1. Search Flights\n"
                  << "2. Confirm a Booking Request\n"
                  << "3. Confirm a Check-In Request\n"
                  << "4. Cancel Reservation\n"
                  << "5. Logout\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1: handleAgentSearchFlights(); break;
            case 2: handleAgentBookConfirm(); break;
            case 3: handleAgentCheckInConfirm(); break;
            case 4: handleCancelReservation(); break;
            case 5: inMenu = false; break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

void ConsoleUI::showPassengerMenu() {
    bool inMenu = true;
    while (inMenu) {
        cout << "\n--- Passenger Menu ---\n"
             << "1. Search Flights\n"
             << "2. View My Reservations\n"
             << "3. Check-In\n"
             << "4. Recharge Balance\n"  // Added option
             << "5. Logout\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1: handlePassengerSearchFlights(); break;
            case 2: handleViewMyReservations(); break;
            case 3: handleCheckIn(); break;
            case 4: handleRechargeBalance(); break; // New handler call
            case 5: inMenu = false; break;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

// ==========================================
// Administrator Handlers
// ==========================================

void ConsoleUI::showManageFlightsMenu() {
    bool managingFlights = true;
    while (managingFlights) {
        cout << "\n--- Manage Flights ---\n"
                  << "1. Add New Flight\n"
                  << "2. Update Existing Flight\n"
                  << "3. Remove Flight\n"
                  << "4. View All Flights\n"
                  << "5. Back to Main Menu\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice)
        {
        case 1: handleAddFlight(); break;
        case 2: handleUpdateFlight(); break;
        case 3: handleRemoveFlight(); break;
        case 4: handleViewAllFlights(); break;
        case 5: managingFlights = false; break;
        default: break;
        }
    }
}

void ConsoleUI::handleAddFlight() {
    cout << "\n--- Add New Flight ---\n";

    auto flightNum = getStringInput("Enter Flight Number: ");
    auto origin = getStringInput("Enter Origin: ");
    auto destination = getStringInput("Enter Destination: ");
    auto depTime = getStringInput("Enter Departure Date and Time (YYYY-MM-DD HH:MM): ");
    float flightDur = getFloatInput("Enter Flight Duration (hours): ");
    int basePrice = getIntInput("Enter base Price (economy class seat): ");

    // Get unassigned aircrafts
    auto availableAircrafts = app_.getAvailableAircrafts();
    if (availableAircrafts.empty()) {
        cout << "\nError: No available aircraft found. Please add or free an aircraft first.\n";
        return;
    }

    cout << "\nAvailable Aircraft:\n";
    int option = 1;
    for (const auto& ac : availableAircrafts) {
        cout << option++ << ". Tail Number: " << ac->getTailNumber() 
                  << " | Model: " << ac->getModel() << "\n";
    }

    int selection = 0;
    while (true) {
        selection = getIntInput("\nSelect Aircraft (1-" + to_string(availableAircrafts.size()) + "): ");
        if (selection >= 1 && selection <= static_cast<int>(availableAircrafts.size())) {
            break;
        }
        cout << "Invalid selection. Please enter a number between 1 and " << availableAircrafts.size() << ".\n";
    }

    auto selectedAircraft = availableAircrafts[selection - 1];

    if (!selectedAircraft) {
        cout << "Error: Invalid Aircraft Tail Number entered.\n";
        return;
    }

    // Prompt for Crew Regulations
    cout << "\n--- Crew Regulations ---\n";
    float minFlightHrs = getFloatInput("Enter minimum allowed flight hours per crew member: ");

    CrewRegulations reg{minFlightHrs};

    // Invoke application logic
    bool success = app_.addFlight(flightNum, origin, destination, depTime, 
        selectedAircraft, flightDur, reg, basePrice);

    if (success) {
        cout << "\nFlight " << flightNum << " has been successfully added to the schedule.\n";
    } else {
        cout << "\nError: Could not add flight.\n";
    }
}

void ConsoleUI::handleUpdateFlight() {
    cout << "\n--- Update Existing Flight ---\n";
    auto flightNum = getStringInput("Enter Flight Number to Update: ");

    auto flight = app_.getFlightById(flightNum);
    if (!flight) {
        cout << "Error: Flight " << flightNum << " not found.\n";
        return;
    }

    while (true) {
        cout << "\nSelect information to update:\n"
             << "1. Flight Details\n"
             << "2. Crew Assignments\n"
             << "3. Status\n"
             << "4. Back to Manage Flights\n";

        int choice = getIntInput("Enter choice: ");
        if (choice == 1) {
            handleflightDetailUpdate(flight);
        } else if (choice == 2) {
            handleAssignCrew(flight);
        } else if (choice == 3) { // this was not updated to support the departed functionality of the admin
            auto statusInt = getIntInput("Enter New Status (1: Scheduled, 2: Delayed, 3: Departed, 4. Cancelled): ");
            if (statusInt == 3){
                auto status = app_.departFlight(flight->getFlightNumber()); 
                switch (status)
                {
                case DepartResult::Success: cout << "\n Flight Departed successfully \n "; break;
                case DepartResult::AircraftNotFound: cout << "\n flight has no assigned aircraft \n "; break;
                case DepartResult::AircraftNotAirworthy: cout << "\n Aircraft is under maintainence \n "; break;
                case DepartResult::AlreadyDeparted: cout << "\n already departed \n "; break;
                default: break;
                }
            }
        } else if (choice == 4) {
            break;
        }
    }
}


void ConsoleUI::handleflightDetailUpdate(const shared_ptr<Flight>& flight) {
    if (!flight) {
        cout << "Invalid flight pointer.\n";
        return;
    }

    bool updating = true;
    while (updating) {
        cout << "\n--- Update Flight Details (" << flight->getFlightNumber() << ") ---\n"
                  << "1. Origin (" << flight->getOrigin() << ")\n"
                  << "2. Destination (" << flight->getDestination() << ")\n"
                  << "3. Departure Date/Time (" << toString(flight->getDate()) << ")\n"
                  << "4. Duration (" << flight->getDuration() << " hrs)\n"
                  << "5. Aircraft Tail Number (" << flight->getFlightNumber() << ")\n"
                  << "6. Base Price ($" << flight->getPriceByClass(SeatClass::Economy) << ")\n"
                  << "7. Flight Status (" << toString(flight->getStatus()) << ")\n"
                  << "8. Back to Main Menu\n";

        int choice = getIntInput("Enter choice (1-8): ");

        switch (choice) {
            case 1: {
                auto newOrigin = getStringInput("Enter new origin (press enter to leave): ");
                if (!newOrigin.empty()) {
                    flight->setOrigin(newOrigin);
                    cout << "Origin updated successfully.\n";
                } else {
                    cout << "Origin unchanged.\n";
                }
                break;
            }
            case 2: {
                auto newDestination = getStringInput("Enter new destination (press enter to leave): ");
                if (!newDestination.empty()) {
                    flight->setDestination(newDestination);
                    cout << "Destination updated successfully.\n";
                } else {
                    cout << "Destination unchanged.\n";
                }
                break;
            }
            case 3: {
                auto newDep = getStringInput("Enter New Departure Time (YYYY-MM-DD HH:MM) (press enter to leave): ");
                if (newDep.empty()) {
                    cout << "Departure time unchanged.\n";
                } else {
                    try {
                        Date date{newDep};
                        flight->setDate(date);
                        cout << "Flight departure time updated successfully.\n";
                    } catch (const exception& e) {
                        cout << "Invalid date format -- departure time NOT updated (" << e.what() << ").\n";
                    }
                }
                break;
            }
            case 4: {
                auto newDurStr = getStringInput("Enter new duration in hours (press enter to leave): ");
                if (newDurStr.empty()) {
                    cout << "Duration unchanged.\n";
                } else {
                    try {
                        float newDuration = stof(newDurStr);
                        flight->setDuration(newDuration);
                        cout << "Duration updated successfully.\n";
                    } catch (const exception& e) {
                        cout << "Invalid duration -- NOT updated (" << e.what() << ").\n";
                    }
                }
                break;
            }
            case 5: {
                auto newAircraftId = getStringInput("Enter new Aircraft Tail Number (eg. 1230056) (press enter to leave): ");
                if (newAircraftId.empty()) {
                    cout << "Aircraft unchanged.\n";
                } else {
                    auto aircraft = app_.aircraftRepo_.get(newAircraftId);
                    if (!aircraft) {
                        cout << "Error: Aircraft '" << newAircraftId << "' does not exist in fleet.\n";
                    } else {
                        flight->setFlightNumeber(newAircraftId);
                        cout << "Assigned aircraft updated to '" << newAircraftId << "'.\n";
                    }
                }
                break;
            }
            case 6: {
                auto newPriceStr = getStringInput("Enter new base price ($) (press enter to leave): ");
                if (newPriceStr.empty()) {
                    cout << "Base price unchanged.\n";
                } else {
                    try {
                        int newPrice = stoi(newPriceStr);
                        flight->setBasePrice(newPrice);
                        cout << "Base price updated successfully.\n";
                    } catch (const exception& e) {
                        cout << "Invalid price input -- NOT updated (" << e.what() << ").\n";
                    }
                }
                break;
            }
            case 7: {
                cout << "\n--- Select New Flight Status ---\n"
                          << "1. Scheduled\n"
                          << "2. Delayed\n"
                          << "3. Cancelled\n"
                          << "4. Departed (Validates airworthiness & resolves pending bookings)\n";

                int statusChoice = getIntInput("Enter status choice (1-4): ");
                if (statusChoice == 1) {
                    flight->setStatus(FlightStatus::Scheduled);
                    cout << "Status set to Scheduled.\n";
                } else if (statusChoice == 2) {
                    flight->setStatus(FlightStatus::Delayed);
                    cout << "Status set to Delayed.\n";
                } else if (statusChoice == 3) {
                    flight->setStatus(FlightStatus::Cancelled);
                    cout << "Status set to Cancelled.\n";
                } else if (statusChoice == 4) {
                    // Trigger the departure workflow with safety checks
                    DepartResult result = app_.departFlight(flight->getFlightNumber());
                    if (result == DepartResult::Success) {
                        cout << "\n[SUCCESS] Flight " << flight->getFlightNumber() << " marked as DEPARTED.\n";
                    } else if (result == DepartResult::AircraftNotAirworthy) {
                        cout << "\n[CRITICAL ERROR] Cannot depart flight! The assigned aircraft exceeds max running hours.\n";
                    } else if (result == DepartResult::AlreadyDeparted) {
                        cout << "\nFlight is already marked as DEPARTED.\n";
                    } else {
                        cout << "\n[ERROR] Departure processing failed.\n";
                    }
                } else {
                    cout << "Invalid status choice.\n";
                }
                break;
            }
            case 8: {
                updating = false;
                break;
            }
            default: {
                cout << "Invalid choice. Please enter a number between 1 and 8.\n";
                break;
            }
        }
    }
}

void ConsoleUI::handleAssignCrew(const shared_ptr<Flight>& flight) {
    if (!flight) return;

    cout << "\n--- Crew Assignments for Flight " << flight->getFlightNumber() << " ---\n";

    // 1. Pilot Assignment
    auto availablePilots = app_.getAvailablePilots();
    if (availablePilots.empty()) {
        cout << "No available pilots found.\n";
    } else {
        cout << "\nAvailable Pilots:\n";
        int option = 1;
        for (const auto& pilot : availablePilots) {
            cout << option++ << ". Pilot ID: " << pilot->getId() 
                 << " - Captain " << pilot->getName() << "\n";
        }

        int selection = getIntInput("Select Pilot (1-" + to_string(availablePilots.size()) + "): ");
        if (selection >= 1 && selection <= static_cast<int>(availablePilots.size())) {
            auto selectedPilot = availablePilots[selection - 1];
            if (app_.assignCrewMember(flight, selectedPilot)) {
                cout << "Captain " << selectedPilot->getName() << " assigned successfully.\n";
            } else {
                cout << "Error: Failed to assign pilot (does not meet flight regulations).\n";
            }
        } else {
            cout << "Invalid selection. Skipping pilot assignment.\n";
        }
    }

    // 2. Flight Attendant Assignment
    auto availableFAs = app_.getAvailableFlightAttendants();
    if (availableFAs.empty()) {
        cout << "No available flight attendants found.\n";
    } else {
        cout << "\nAvailable Flight Attendants:\n";
        int option = 1;
        for (const auto& fa : availableFAs) {
            cout << option++ << ". FA ID: " << fa->getId() 
                 << " - " << fa->getName() << "\n";
        }

        int selection = getIntInput("Select Flight Attendant (1-" + to_string(availableFAs.size()) + "): ");
        if (selection >= 1 && selection <= static_cast<int>(availableFAs.size())) {
            auto selectedFA = availableFAs[selection - 1];
            if (app_.assignCrewMember(flight, selectedFA)) {
                cout << "Flight Attendant " << selectedFA->getName() << " assigned successfully.\n";
            } else {
                cout << "Error: Failed to assign flight attendant (does not meet flight regulations).\n";
            }
        } else {
            cout << "Invalid selection. Skipping flight attendant assignment.\n";
        }
    }
}

void ConsoleUI::handleRemoveFlight() {
    // Stub
}

void ConsoleUI::handleViewAllFlights() {
    cout << "\n--- All Flights ---\n";
    const auto& flights = app_.getAllFlights();

    if (flights.empty()) {
        cout << "No flights scheduled in the system.\n";
        return;
    }

    for (const auto& flight : flights) {
        cout << "Flight: " << flight->getFlightNumber() 
                  << " | " << flight->getOrigin() << " -> " << flight->getDestination()
                  << " | Dep: " << flight->getDate()
                  << " | Status: " << flight->getStatus() << "\n";
    }
}

void ConsoleUI::showManageAircraftMenu() {
    while (true) {
        cout << "\n--- Manage Aircraft ---\n"
                  << "1. Add New Aircraft\n"
                  << "2. Update Aircraft Maintenance\n"
                  << "3. Remove Aircraft\n"
                  << "4. View All Aircraft\n"
                  << "5. Back to Administrator Menu\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1: handleAddAircraft(); break;
            case 2: handleUpdateAircraft(); break;
            case 3: handleRemoveAircraft(); break;
            case 4: handleViewAllAircraft(); break;
            case 5: return;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

void ConsoleUI::handleAddAircraft() {
    cout << "\n--- Add New Aircraft ---\n";
    string tailNum = getStringInput("Enter Tail Number: ");
    string model   = getStringInput("Enter Aircraft Model (e.g. Boeing 737): ");

    // Input capacity per class on a single line
    string capacityLine = getStringInput("Enter seating capacities (First Business Economy, e.g., 12 30 120): ");
    stringstream ss(capacityLine);
    int firstClass = 0, businessClass = 0, economyClass = 0;
    ss >> firstClass >> businessClass >> economyClass;

    // Collect max running hours until maintenance is required
    float maxRunningHours = getFloatInput("Enter Max Running Hours: ");

    // SeatLayout constructed with individual class capacities
    SeatLayout seatLayout; 
    seatLayout.addSeats(SeatClass::First, firstClass);
    seatLayout.addSeats(SeatClass::Business, businessClass);
    seatLayout.addSeats(SeatClass::Economy, economyClass);

    bool success = app_.addAircraft(tailNum, model, seatLayout, maxRunningHours);
    if (success) {
        cout << "Aircraft " << tailNum << " (" << model << ") added successfully.\n";
    } else {
        cout << "Error: Aircraft ID already exists or failed to create.\n";
    }
}

void ConsoleUI::handleUpdateAircraft() {
    cout << "\n--- Update Aircraft Maintenance ---\n";
    string tailNum = getStringInput("Enter Aircraft Tail Number: ");
    auto aircraft = app_.getAircraftByTailNumber(tailNum);

    if (!aircraft) {
        cout << "Error: Aircraft " << tailNum << " not found.\n";
        return;
    }
    cout 
            << "1. Airworthy\n"
            << "2. InMaintenance\n";
    int choice = getIntInput("Enter Maintenance Status: ");
    MaintenanceStatus status = MaintenanceStatus::Airworthy;
    switch (choice)
    {
    case 2: status = MaintenanceStatus::InMaintenance; break;
    default: break;
    }
    aircraft->setMaintenanceStatus(status);
    cout << "Aircraft " << tailNum << " status updated to " << status << ".\n";
}

void ConsoleUI::handleRemoveAircraft() {
    cout << "\n--- Remove Aircraft ---\n";
    string tailNum = getStringInput("Enter Aircraft Tail Number to remove: ");

    if (app_.removeAircraft(tailNum)) {
        cout << "Aircraft " << tailNum << " removed successfully.\n";
    } else {
        cout << "Error: Aircraft " << tailNum << " not found.\n";
    }
}

void ConsoleUI::handleViewAllAircraft() {
    cout << "\n--- All Aircraft ---\n";
    const auto& aircrafts = app_.getAllAircraft();
    if (aircrafts.empty()) {
        cout << "No aircraft found.\n";
        return;
    }

    for (const auto& ac : aircrafts) {
        if (!ac) continue;
        cout << "Tail Number: " << ac->getTailNumber()
                  << " | Model: " << ac->getModel()
                  << " | Seats: (first, bis, eco)" 
                  << ac->getFirstClassCapacity() << " "
                  << ac->getBusinessClassCapacity() << " "
                  << ac->getEconomyClassCapacity() << " "
                  << " | Status: " << ac->getMaintenanceStatus() << "\n";
    }
}

// ============================================================================
// Manage Users Menu & Handlers
// ============================================================================

void ConsoleUI::showManageUsersMenu() {
    while (true) {
        cout << "\n--- Manage Users ---\n"
                  << "1. Add New User\n"
                  << "2. Update Existing User\n"
                  << "3. Remove User\n"
                  << "4. View All Users\n"
                  << "5. Back to Administrator Menu\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1: handleAddUser(); break;
            case 2: handleUpdateUser(); break;
            case 3: handleRemoveUser(); break;
            case 4: handleViewAllUsers(); break;
            case 5: return;
            default: cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

void ConsoleUI::handleAddUser() {
    cout << "\n--- Add New User ---\n";
    cout << "Select Role:\n"
              << "1. Administrator\n"
              << "2. Booking Agent\n"
              << "3. Passenger\n";

    int roleChoice = getIntInput("Enter role choice: ");
    auto role = intToRole(roleChoice);
    if(!role){
        cout << "Invalid role choice!";
        return;
    }
    string name     = getStringInput("Enter Full Name: ");
    string username = getStringInput("Enter Username: ");
    string password = getStringInput("Enter Password: "); // hashed later in addUser
    string email    = getStringInput("Enter Email: ");
    string phone    = getStringInput("Enter Phone Number: ");

    bool success = app_.addUser(role.value(), name, username, password, email, phone);
    if (success) {
        cout << "\nUser '" << username << "' added successfully.\n";
    } else {
        cout << "\nError: Username already exists or role choice is invalid.\n";
    }
}

void ConsoleUI::handleUpdateUser() {
    cout << "\n--- Update Existing User ---\n";
    string username = getStringInput("Enter Username of User to update: ");

    auto user = app_.getUserByUsername(username);
    if (!user) {
        cout << "Error: User '" << username << "' not found.\n";
        return;
    }

    string newPassword = getStringInput("Enter New Password (or press enter to skip): ");
    string newName     = getStringInput("Enter New Name (or press enter to skip): ");

    if (!newPassword.empty()) user->setPassword(newPassword);
    if (!newName.empty())     user->setName(newName);

    cout << "User '" << username << "' updated successfully.\n";
}

void ConsoleUI::handleRemoveUser() {
    cout << "\n--- Remove User ---\n";
    string username = getStringInput("Enter Username to remove: ");

    if (app_.removeUser(username)) {
        cout << "User '" << username << "' successfully removed.\n";
    } else {
        cout << "Error: User '" << username << "' not found.\n";
    }
}

void ConsoleUI::handleViewAllUsers() {
    cout << "\n--- All Users ---\n";
    const auto& users = app_.getAllUsers();
    if (users.empty()) {
        cout << "No users found.\n";
        return;
    }

    for (const auto& user : users) {
        if (!user) continue;
        cout << "ID: " << user->getId()
                  << " | Username: " << user->getUsername()
                  << " | Name: " << user->getName()
                  << " | Role: " << toString(user->getRole()) << "\n";
    }
}

void ConsoleUI::showGenerateReportsMenu() {
    while (true) {
        cout << "\n--- Generate Reports ---\n"
                  << "1. Operational Reports\n"
                  << "2. Maintenance Reports\n"
                  << "3. User Activity Reports\n"
                  << "4. Back to Main Menu\n";

        int choice = getIntInput("Enter choice: ");
        if (choice == 1) {
            handleOperationalReport();
        } else if (choice == 2) {
            handleMaintenanceReport();
        } else if (choice == 3) {
            handleUserActivityReport();
        } else if (choice == 4) {
            break;
        }
    }
}

void ConsoleUI::handleOperationalReport() {
    using airline::SeatClass;

    cout << "\n=========================================\n"
         << "          OPERATIONAL REPORT             \n"
         << "=========================================\n";

    if (app_.finishedRequests_.empty()) {
        cout << "No finished requests/records found.\n"
             << "=========================================\n";
        return;
    }

    int totalTransactions  = 0;
    int completedPayments  = 0;
    int failedPayments     = 0;
    int cancelledCount     = 0;
    long long totalRevenue = 0;

    map<SeatClass, int> classBookings;
    map<SeatClass, long long> classRevenue;
    map<string, int> flightPopularity; // Flight Number -> Count

    for (const auto& req : app_.finishedRequests_) {
        if (!req) continue;
        totalTransactions++;

        if (req->paymentStatus == PaymentStatus::Completed) {
            completedPayments++;
            totalRevenue += req->price;

            classBookings[req->seatClass]++;
            classRevenue[req->seatClass] += req->price;
            
            if (!req->flightNumber.empty()) {
                flightPopularity[req->flightNumber]++;
            }
        } else if (req->paymentStatus == PaymentStatus::Failed) {
            failedPayments++;
        }

        if (req->reservationStatus == ReservationStatus::Cancelled) {
            cancelledCount++;
        }
    }

    cout << "Total Transactions Processed : " << totalTransactions << "\n"
         << "  - Completed Payments       : " << completedPayments << "\n"
         << "  - Failed Payments          : " << failedPayments << "\n"
         << "  - Cancelled Reservations   : " << cancelledCount << "\n"
         << "-----------------------------------------\n"
         << "Total Gross Revenue          : $" << totalRevenue << "\n"
         << "-----------------------------------------\n"
         << "Revenue & Volume by Seat Class:\n";

    for (SeatClass sc : {SeatClass::Economy, SeatClass::Business, SeatClass::First}) {
        cout << "  - " << left << setw(10) << toString(sc)
             << " : " << classBookings[sc] << " bookings | $" << classRevenue[sc] << "\n";
    }

    cout << "-----------------------------------------\n"
         << "Flight Bookings Summary:\n";

    if (flightPopularity.empty()) {
        cout << "  No completed flight bookings recorded.\n";
    } else {
        // C++11 iteration replacing C++17 structured bindings
        for (const auto& pair : flightPopularity) {
            const string& flightNum = pair.first;
            int count                    = pair.second;
            cout << "  - Flight " << flightNum << " : " << count << " completed booking(s)\n";
        }
    }

    cout << "=========================================\n";
}


void ConsoleUI::handleMaintenanceReport() {
    cout << "\n=========================================\n"
         << "          MAINTENANCE REPORT             \n"
         << "=========================================\n";

    auto aircraftList = app_.aircraftRepo_.getAll();
    if (aircraftList.empty()) {
        cout << "No aircraft registered in the system.\n"
             << "=========================================\n";
        return;
    }

    int totalFleet     = 0;
    int criticalCount  = 0;
    int warningCount   = 0;

    for (const auto& ac : aircraftList) {
        if (!ac) continue;
        totalFleet++;

        float hours    = ac->getRunningHours();
        float maxHours = ac->getMaxRunningHours();
        float ratio    = (maxHours > 0) ? (hours / maxHours) * 100.0f : 0.0f;

        cout << "Tail #: " << left << setw(8) << ac->getTailNumber()
             << " | Model: " << setw(15) << ac->getModel()
             << " | Hours: " << hours << " / " << maxHours << " hrs (" 
             << fixed << setprecision(1) << ratio << "%)\n";

        if (hours >= maxHours) {
            cout << "   >>> Status: [CRITICAL] Grounded - Exceeds max running hours!\n";
            criticalCount++;
        } else if (ratio >= 80.0f) {
            cout << "   >>> Status: [WARNING] Scheduled maintenance due soon.\n";
            warningCount++;
        } else {
            cout << "   >>> Status: [OK] Operational\n";
        }
    }

    cout << "-----------------------------------------\n"
         << "Fleet Maintenance Summary:\n"
         << "  - Total Aircraft in Fleet : " << totalFleet << "\n"
         << "  - Critical / Grounded     : " << criticalCount << "\n"
         << "  - Maintenance Warning (>80%): " << warningCount << "\n"
         << "=========================================\n";
}


void ConsoleUI::handleUserActivityReport() {
    cout << "\n=========================================\n"
         << "        USER ACTIVITY REPORT             \n"
         << "=========================================\n";

    auto passengers = app_.userPassengerService_.getAllUsers();
    auto agents     = app_.userBookingAgentService_.getAllUsers();
    auto admins     = app_.userAdminService_.getAllUsers();
    auto pilots     = app_.pilots_.getAll();
    auto flightAtts = app_.flightAtts_.getAll();

    cout << "System Accounts & Crew Breakdown:\n"
         << "  - Registered Passengers : " << passengers.size() << "\n"
         << "  - Booking Agents        : " << agents.size() << "\n"
         << "  - Administrators        : " << admins.size() << "\n"
         << "  - Active Pilots         : " << pilots.size() << "\n"
         << "  - Flight Attendants     : " << flightAtts.size() << "\n"
         << "-----------------------------------------\n";

    // Aggregate spend per passenger from finishedRequests_
    struct PassengerStats {
        string name;
        long long totalSpent{0};
        int completedBookings{0};
    };

    map<string, PassengerStats> passengerMap;

    for (const auto& req : app_.finishedRequests_) {
        if (!req) continue;
        
        if (req->paymentStatus == PaymentStatus::Completed) {
            auto& stats = passengerMap[req->passengerId];
            stats.name = req->passengerName.empty() ? req->passengerId : req->passengerName;
            stats.totalSpent += req->price;
            stats.completedBookings++;
        }
    }

    cout << "Top Passenger Activity (from Completed Requests):\n";
    if (passengerMap.empty()) {
        cout << "  No completed activity recorded.\n";
    } else {
        for (const auto& [pId, stats] : passengerMap) {
            cout << "  - [" << pId << "] " << left << setw(20) << stats.name
                 << " | Bookings: " << stats.completedBookings
                 << " | Total Spent: $" << stats.totalSpent << "\n";
        }
    }

    cout << "-----------------------------------------\n"
         << "Crew Member Logged Hours:\n";

    for (const auto& p : pilots) {
        if (p) {
            cout << "  - Pilot [" << p->getId() << "] " << left << setw(20) 
                 << p->getName() << " : " << p->getFlightHours() << " flight hrs\n";
        }
    }
    for (const auto& fa : flightAtts) {
        if (fa) {
            cout << "  - Attendant [" << fa->getId() << "] " << left << setw(18) 
                 << fa->getName() << " : " << fa->getFlightHours() << " flight hrs\n";
        }
    }

    cout << "=========================================\n";
}

// ==========================================
// Booking Agent Handlers
// ==========================================

void ConsoleUI::handleAgentSearchFlights() {
    cout << "\n--- Booking Agent: Search Flights ---\n";

    string origin      = getStringInput("Enter Origin: ");
    string destination = getStringInput("Enter Destination: ");

    // Fetch matching flights for origin and destination
    auto matchingFlights = app_.searchFlights(origin, destination);

    if (matchingFlights.empty()) {
        cout << "\nNo flights found from " << origin << " to " << destination << ".\n";
        return;
    }

    // Prompt agent for date (press Enter to skip)
    cout << "Enter Target Departure Date (DD/MM/YYYY) [Press Enter to skip]: ";
    string dateInput;
    getline(cin, dateInput);

    if (!dateInput.empty()) {
        Date targetDate = Date::parseString(dateInput);

        // Sort matching flights chronologically (ascending order)
        sort(matchingFlights.begin(), matchingFlights.end(),
            [](const shared_ptr<Flight>& a, const shared_ptr<Flight>& b) {
                Date dateA = a->getDate();
                Date dateB = b->getDate();
                return !(dateA > dateB); // Ascending order
            });

        shared_ptr<Flight> flightBefore = nullptr;
        vector<shared_ptr<Flight>> flightsAfter;

        for (const auto& flight : matchingFlights) {
            Date flightDate = flight->getDate();

            if (targetDate > flightDate) {
                // Updates continuously to the closest flight strictly before targetDate
                flightBefore = flight; 
            } else {
                // Collects ALL flights occurring on or after targetDate
                flightsAfter.push_back(flight);
            }
        }

        cout << "\n--- Search Results Near Date: " << dateInput << " ---\n";

        // 1. Single flight just before target date
        if (flightBefore) {
            cout << "\n[ Flight Just BEFORE " << dateInput << " ]\n"
                      << "Flight Number : " << flightBefore->getFlightNumber() << "\n"
                      << "Departure     : " << flightBefore->getDate() << "\n";
        } else {
            cout << "\n[ Flight Just BEFORE " << dateInput << " ] : None found.\n";
        }

        // 2. ALL flights on or after target date
        cout << "\n[ Flights On / AFTER " << dateInput << " ]\n";
        if (flightsAfter.empty()) {
            cout << "None found.\n";
        } else {
            int option = 1;
            for (const auto& flight : matchingFlights) {
                cout << option++ << ". Flight Number: " << flight->getFlightNumber() << "\n"
                        << "   Departure:     " << flight->getDate() << "\n";
            }
        }
    } else {
        // Date skipped: Show all matching flights directly
        cout << "\n--- All Available Flights (" << origin << " -> " << destination << ") ---\n";
        int option = 1;
        for (const auto& flight : matchingFlights) {
            cout << option++ << ". Flight Number: " << flight->getFlightNumber() << "\n"
                      << "   Departure:     " << flight->getDate() << "\n";
        }
    }

    // Optional flight details lookup
    cout << "\n";
    string flightChoice = getStringInput("Enter Flight Number to view details (or '0' to cancel): ");

    if (flightChoice == "0" || flightChoice.empty()) {
        return;
    }

    // Match chosen flight number
    shared_ptr<Flight> selectedFlight = nullptr;
    for (const auto& flight : matchingFlights) {
        if (flight->getFlightNumber() == flightChoice) {
            selectedFlight = flight;
            break;
        }
    }

    if (!selectedFlight) {
        cout << "Invalid Flight Number selected.\n";
        return;
    }

    // Display detailed flight information
    auto [firstAvail, bizAvail, econAvail] = selectedFlight->getAvailableSeatsPerClass();

    cout << "\n--- Flight Details (" << selectedFlight->getFlightNumber() << ") ---\n"
              << "Route           : " << selectedFlight->getOrigin() << " -> " << selectedFlight->getDestination() << "\n"
              << "Departure       : " << selectedFlight->getDate() << "\n"
              << "Available Seats : First: " << firstAvail 
              << " | Business: " << bizAvail 
              << " | Economy: " << econAvail << "\n";
}


void ConsoleUI::handleAgentBookConfirm() {
    cout << "\n--- Booking Agent: Confirm Booking Request ---\n";

    // Retrieve all booking requests via the application layer
    auto requests = app_.getAllBookingRequests();

    if (requests.empty()) {
        cout << "No booking requests available.\n";
        return;
    }

    // Display all booking requests with details
    cout << "\n--- Booking Requests List ---\n";
    for (const auto& req : requests) {
        if (!req) continue;

        auto passenger = req->passenger.lock();
        auto flight    = req->flight.lock();

        cout << "Request ID   : " << req->id << "\n"
                  << "  Passenger  : " << (passenger ? passenger->getName() + " (ID: " + passenger->getId() + ")" : "N/A") << "\n"
                  << "  Flight     : " << (flight ? flight->getFlightNumber() + " (" + flight->getOrigin() + " -> " + flight->getDestination() + ")" : "N/A") << "\n"
                  << "  Departure  : " << (flight ? toString(flight->getDate()) : "N/A") << "\n"
                  << "  Status     : " << toString(req->status) << "\n"
                  << "----------------------------------------\n";
    }

    // Prompt agent for choice
    string reqIdChoice = getStringInput("Enter Request ID to confirm (or '0' to cancel): ");

    if (reqIdChoice == "0" || reqIdChoice.empty()) {
        return;
    }

    // Call Application layer function to set status to ConfirmedBook
    const auto& requestReply = app_.confirmBookingRequest(reqIdChoice);

    if (requestReply.idFound && requestReply.payStatus == PaymentStatus::Completed) {
        cout << "\nSuccess: Booking Request '" << reqIdChoice 
                  << "' status updated to ConfirmedBook!\n"
                  << "The passenger can now convert this confirmed booking into a check-in request.\n";
    } else {
        if (requestReply.idFound == false){
            cout << "\nError: Booking Request ID '" << reqIdChoice << "' not found.\n";
        } else if (requestReply.payStatus == PaymentStatus::Failed) {
            cout << "\n Payment failed, user doesn't have enough balance at the moment. Request Cancelled!\n";
        } else if (requestReply.payStatus == PaymentStatus::Pending){
            cout << "\n No seats of requested class available on the flight \n";
        }
    }
}



void ConsoleUI::handleAgentCheckInConfirm() {
    cout << "\n--- Booking Agent: Confirm Check-In Request ---\n";

    // 1. Fetch check-in requests from Application layer
    auto requests = app_.getAllCheckInRequests();

    if (requests.empty()) {
        cout << "No check-in requests available.\n";
        return;
    }

    // 2. Display all check-in requests
    cout << "\n--- Check-In Requests List ---\n";
    for (const auto& req : requests) {
        if (!req) continue;

        auto passenger = req->passenger.lock();
        auto flight    = req->flight.lock();

        cout << "Check-In ID  : " << req->id << "\n"
                  << "  Passenger  : " << (passenger ? passenger->getName() + " (ID: " + passenger->getId() + ")" : "N/A") << "\n"
                  << "  Flight     : " << (flight ? flight->getFlightNumber() + " (" + flight->getOrigin() + " -> " + flight->getDestination() + ")" : "N/A") << "\n"
                  << "  Departure  : " << (flight ? toString(flight->getDate()) : "N/A") << "\n"
                  << "  Status     : " << toString(req->status) << "\n"
                  << "----------------------------------------\n";
    }

    // 3. Prompt agent for selection or cancel
    string reqIdChoice = getStringInput("Enter Check-In ID to confirm (or '0' to cancel): ");

    if (reqIdChoice == "0" || reqIdChoice.empty()) {
        return;
    }

    // 4. Update status via Application layer
    bool success = app_.confirmCheckInRequest(reqIdChoice);

    if (success) {
        cout << "\nSuccess: Check-In Request '" << reqIdChoice 
                  << "' has been successfully confirmed!\n";
    } else {
        cout << "\nError: Check-In Request ID '" << reqIdChoice << "' not found.\n";
    }
}

void ConsoleUI::handleCancelReservation() {
    // Stub
}

// ==========================================
// Passenger Handlers
// ==========================================

void ConsoleUI::handlePassengerSearchFlights() {
    cout << "\n--- Search Flights ---\n";

    auto origin = getStringInput("Enter Origin: ");
    auto destination = getStringInput("Enter Destination: ");

    // Fetch matching flights using system API
    auto flights = app_.searchFlights(origin, destination);

    if (flights.empty()) {
        cout << "\nNo available flights found from " << origin << " to " << destination << ".\n";
        return;
    }

    cout << "\nAvailable Flights:\n";
    int option = 1;
    for (const auto& flight : flights) {
        cout << option++ << ". Flight Number: " << flight->getFlightNumber() << "\n"
                << "   Departure: " << flight->getDate() << "\n"
                << "   Economy Seat $" << flight->getPriceByClass(SeatClass::Economy) << ", "
                << "   Business Seat $" << flight->getPriceByClass(SeatClass::Business) << ", "
                << "   First Class $" << flight->getPriceByClass(SeatClass::First) <<"\n";
    }
    option = 1;

    cout << "\n";
    string flightChoice = getStringInput("Enter the Flight Number you wish to book (or '0' to cancel): ");

    // Check for cancel / go back safely using string comparison
    if (flightChoice == "0" || flightChoice.empty()) {
        return;
    }

    // Match chosen flight number
    shared_ptr<Flight> selectedFlight = nullptr;
    for (const auto& flight : flights) {
        if (flight->getFlightNumber() == flightChoice) {
            selectedFlight = flight;
            break;
        }
    }

    if (!selectedFlight) {
        cout << "Invalid Flight Number selected.\n";
        return;
    }

    auto currentPassenger = dynamic_pointer_cast<Passenger>(app_.getCurrentUser());
    if (app_.isDuplicatedBookingReq(currentPassenger, selectedFlight)) {
        cout << "\nYou are already booked on flight " << selectedFlight->getFlightNumber() << "!\n";
        return;
    }

    // Display available seat tier summary
    auto [firstAvail, bizAvail, econAvail] = selectedFlight->getAvailableSeatsPerClass();

    cout << "\nAvailable Seats on Flight " << selectedFlight->getFlightNumber() << ":\n"
              << "1. First Class (" << firstAvail << " left)\n"
              << "2. Business Class (" << bizAvail << " left)\n"
              << "3. Economy Class (" << econAvail << " left)\n";

    int classChoice = getIntInput("Select Seat Class (1-3) (0 to go back): ");
    SeatClass chosenClass = SeatClass::Economy;
    switch (classChoice){
        case 1: chosenClass = SeatClass::First; break;
        case 2: chosenClass = SeatClass::Business; break;
        case 3: chosenClass = SeatClass::Economy; break;
        default: return; break;
    }

    // Single system call handles find seat + assign seat + create request
    auto bookingReq = app_.createBookingRequest(currentPassenger, selectedFlight, chosenClass);

    if (bookingReq) {
        cout << "\nBooking successful!\n"
                  << "Reservation ID: " << bookingReq->id << "\n"
                  << "Flight: " << selectedFlight->getFlightNumber() << "\n";
    } else {
        cout << "\nError: Selected seat option is no longer available or occupied.\n";
    }
}

void ConsoleUI::handleViewMyReservations() {
    cout << "\n--- My Reservations & Travel History ---\n";

    // 1. Retrieve the logged-in passenger
    auto currentPassenger = dynamic_pointer_cast<airline::Passenger>(app_.getCurrentUser());
    if (!currentPassenger) {
        cout << "Error: Current logged-in user is not a valid Passenger.\n";
        return;
    }

    cout << "Fetching reservations and travel history for Passenger " << currentPassenger->getName() << "...\n\n";

    // 2. Fetch booking requests, check-in requests, and travel history
    const auto& bookings      = currentPassenger->getBookingRequests();
    const auto& checkIns      = currentPassenger->getCheckInRequests(); 
    const auto& travelHistory = currentPassenger->getTravelHistory(); // or getFinishedRequests()

    if (bookings.empty() && checkIns.empty() && travelHistory.empty()) {
        cout << "No active reservations or travel history found.\n";
        return;
    }

    // 3. Display Booking Requests
    cout << "=== Booking Requests ===\n";
    if (bookings.empty()) {
        cout << "No booking requests found.\n\n";
    } else {
        for (size_t i = 0; i < bookings.size(); ++i) {
            const auto& req = bookings[i];
            if (!req) continue;

            cout << (i + 1) << ". Booking ID : " << req->id << "\n";
            if (auto flight = req->flight.lock()) {
                cout << "   Flight     : " << flight->getFlightNumber() 
                          << " (" << flight->getOrigin() << " -> " << flight->getDestination() << ")\n"
                          << "   Departure  : " << flight->getDate() << "\n";
            } else {
                cout << "   Flight     : N/A\n";
            }
            cout << "   Seat Class : " << airline::toString(req->seatClass) << "\n";
            cout << "   Price      : $" << req->price << "\n";
            cout << "   Status     : " << airline::toString(req->status) << "\n\n";
        }
    }

    // 4. Display Check-In Requests
    cout << "=== Check-In Requests ===\n";
    if (checkIns.empty()) {
        cout << "No check-in requests found.\n\n";
    } else {
        for (size_t i = 0; i < checkIns.size(); ++i) {
            const auto& req = checkIns[i];
            if (!req) continue;

            cout << (i + 1) << ". Check-In ID: " << req->id << "\n";
            if (auto flight = req->flight.lock()) {
                cout << "   Flight     : " << flight->getFlightNumber() 
                          << " (" << flight->getOrigin() << " -> " << flight->getDestination() << ")\n"
                          << "   Departure  : " << flight->getDate() << "\n";
            } else {
                cout << "   Flight     : N/A\n";
            }
            cout << "   Seat Class : " << airline::toString(req->seatClass) << "\n";
            cout << "   Price      : $" << req->price << "\n";
            cout << "   Status     : " << airline::toString(req->status) << "\n\n";
        }
    }

    // 5. Display Travel History (Completed / Confirmed Requests)
    cout << "=== Travel History ===\n";
    if (travelHistory.empty()) {
        cout << "No past travel history found.\n\n";
    } else {
        for (size_t i = 0; i < travelHistory.size(); ++i) {
            const auto& record = travelHistory[i];

            cout << (i + 1)  << "   Flight      : " << record.flightNumber 
                      << " (" << record.origin << " -> " << record.destination << ")\n"
                      << "   Departure   : " << record.date << "\n"
                      << "   Seat Class  : " << airline::toString(record.seatClass) << "\n"
                      << "   Price       : $" << record.price << "\n";
        }
    }
}

void ConsoleUI::handleCheckIn() {
    cout << "\n--- Check-In ---\n";

    auto currentPassenger = dynamic_pointer_cast<airline::Passenger>(app_.getCurrentUser());
    if (!currentPassenger) {
        cout << "Error: Current logged-in user is not a valid Passenger.\n";
        return;
    }

    string resId = getStringInput("Enter Reservation ID (if you don't know it, invoke '2. View My Reservations' ): ");

    // Verify reservation ownership directly from passenger's booking list
    shared_ptr<airline::BookingRequest> bookReq = nullptr;
    for (const auto& booking : currentPassenger->getBookingRequests()) {
        if (booking && booking->id == resId) {
            bookReq = booking;
            break;
        }
    }

    if (!bookReq) {
        cout << "\nError: Reservation ID " << resId << " not found or does not belong to you.\n";
        return;
    }

    if (bookReq->status != airline::ReservationStatus::ConfirmedBook) {
        cout << "\nCheck-In failed: Your reservation is currently " 
                  << toString(bookReq->status) 
                  << ". It must be Confirmed by an agent first.\n";
        return;
    }

    // Submit check-in request
    auto checkInReq = app_.createCheckInRequest(currentPassenger, bookReq);
    if (!checkInReq) {
        cout << "\nCheck-In failed: System could not process check-in request.\n";
        return;
    }

    // Lock weak_ptr<Flight> to print boarding pass details
    auto flight = bookReq->flight.lock();

    cout << "\nCheck-In Request Submitted!\n"
              << "Request Details:\n"
              << "-----------------------------\n"
              << "Reservation ID: " << checkInReq->id << "\n"
              << "Passenger: " << currentPassenger->getName() << "\n";

    if (flight) {
        cout << "Flight: " << flight->getFlightNumber() << "\n"
                  << "Origin: " << flight->getOrigin() << "\n"
                  << "Destination: " << flight->getDestination() << "\n"
                  << "Departure: " << flight->getDate() << "\n";
    } else {
        cout << "Flight: N/A\n";
    }

    cout << "-----------------------------\n";
}

void ConsoleUI::handleRechargeBalance() {
    cout << "\n--- Recharge Balance ---\n";

    // 1. Retrieve the logged-in passenger
    auto passenger = dynamic_pointer_cast<Passenger>(app_.getCurrentUser());

    // 2. Display current balance & loyalty points
    cout << "Current Balance       : $" << passenger->getBalance() << "\n"
        << "Available Loyalty Pts : " << passenger->getLoyaltyBalance() << " pts (" 
        << app_.loyaltyPointsDivisor << " pts = $1)\n\n";
    // 3. Prompt for Recharge / Funding Source
    cout << "Select Funding Method:\n"
         << "1. Cash\n"
         << "2. Card\n"
         << "3. Redeem Loyalty Points\n";
    
    auto methodChoice = getStringInput("Enter choice (press enter to return): ");

    if (methodChoice.empty()){
        return;
    }

    if (methodChoice == "1" || methodChoice == "2") {
        // same thing so far
        int amount = getIntInput("Enter recharge amount: $");
        if (amount <= 0) {
            cout << "Error: Recharge amount must be greater than zero.\n";
            return;
        }

        passenger->rechargeBalance(amount);
        cout << "\nSuccess: Added $" << amount << " to your balance via "
                << (methodChoice == "1" ? "Cash" : "Card") << "!\n"
                << "New Balance: $" << passenger->getBalance() << "\n";

    } else if (methodChoice == "3") {
        // --- Redeem Loyalty Points ---
        int currentPts = passenger->getLoyaltyBalance();
        if (currentPts < app_.loyaltyPointsDivisor) {
            cout << "\nError: You need at least "<< app_.loyaltyPointsDivisor 
                << " loyalty points to redeem ($1 value).\n";
            return;
        }

        cout << "Enter points to redeem (multiple of " 
                << AirlineApplication::loyaltyPointsDivisor <<" ): ";
        int ptsToRedeem = getIntInput("");

        if (ptsToRedeem <= 0) {
            cout << "Error: Points to redeem must be greater than zero.\n";
            return;
        }

        if (ptsToRedeem > currentPts) {
            cout << "Error: Insufficient loyalty points. You have " << currentPts << " pts.\n";
            return;
        }

        int dollarValue = ptsToRedeem / AirlineApplication::loyaltyPointsDivisor;
        if (dollarValue == 0) {
            cout << "Error: Minimum redemption amount is 10 points ($1).\n";
            return;
        }

        if (passenger->redeemLoyaltyPoints(ptsToRedeem)) {
            passenger->rechargeBalance(dollarValue);
            cout << "\nSuccess: Redeemed " << (dollarValue * AirlineApplication::loyaltyPointsDivisor) 
                << " points for $" << dollarValue << "!\n"
                << "New Balance       : $" << passenger->getBalance() << "\n"
                << "Remaining Points  : " << passenger->getLoyaltyBalance() << " pts\n";
        } else {
            cout << "\nError: Failed to redeem loyalty points.\n";
        }
    }
}

} // namespace airline
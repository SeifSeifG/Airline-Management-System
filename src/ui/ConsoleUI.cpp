#include "ui/ConsoleUI.hpp"
#include "app/AirlineApplication.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <limits>

namespace airline {

using namespace std; // much easier than std:: everywhere

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
    auto password = getStringInput("Password: ");

    if (app_.login(username, password)) {
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
                  << "4. Logout\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1: handlePassengerSearchFlights(); break;
            case 2: handleViewMyReservations(); break;
            case 3: handleCheckIn(); break;
            case 4: inMenu = false;  break;
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
        } else if (choice == 3) {
            auto newStatusStr = getStringInput("Enter New Status (0: Scheduled, 1: Delayed, 2: Canceled): ");
            int statusInt = stoi(newStatusStr);
            flight->setStatus(static_cast<FlightStatus>(statusInt));
            cout << "Flight status updated successfully.\n";
        } else if (choice == 4) {
            break;
        }
    }
}

void ConsoleUI::handleflightDetailUpdate(shared_ptr<Flight> flight) {
    cout << "\n--- Update Flight Details (" << flight->getFlightNumber() << ") ---\n"
         << "1. Origin\n"
         << "2. Destination\n"
         << "3. Departure Date/Time\n"
         << "4. Duration\n"
         << "5. Back\n";

    int choice = getIntInput("Enter choice: ");

    if (choice == 1) {
        auto newOrigin = getStringInput("Enter new origin, or leave blank to keep current: ");
        if (!newOrigin.empty()) {
            flight->setOrigin(newOrigin);
            cout << "Origin updated successfully.\n";
        } else {
            cout << "Origin unchanged.\n";
        }
    } else if (choice == 2) {
        auto newDestination = getStringInput("Enter new destination, or leave blank to keep current: ");
        if (!newDestination.empty()) {
            flight->setDestination(newDestination);
            cout << "Destination updated successfully.\n";
        } else {
            cout << "Destination unchanged.\n";
        }
    } else if (choice == 3) {
        auto newDep = getStringInput("Enter New Departure Time (YYYY-MM-DD HH:MM), or leave blank to keep current: ");
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
    } else if (choice == 4) {
        auto newDurStr = getStringInput("Enter new duration in hours, or leave blank to keep current: ");
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
    } else if (choice == 5) {
        return;
    } else {
        cout << "Invalid choice.\n";
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
        std::cout << "\n--- Manage Aircraft ---\n"
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
            default: std::cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

void ConsoleUI::handleAddAircraft() {
    std::cout << "\n--- Add New Aircraft ---\n";
    std::string tailNum = getStringInput("Enter Tail Number: ");
    std::string model   = getStringInput("Enter Aircraft Model (e.g. Boeing 737): ");

    // Input capacity per class on a single line
    std::string capacityLine = getStringInput("Enter seating capacities (First Business Economy, e.g., 12 30 120): ");
    std::stringstream ss(capacityLine);
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
        std::cout << "Aircraft " << tailNum << " (" << model << ") added successfully.\n";
    } else {
        std::cout << "Error: Aircraft ID already exists or failed to create.\n";
    }
}

void ConsoleUI::handleUpdateAircraft() {
    std::cout << "\n--- Update Aircraft Maintenance ---\n";
    std::string tailNum = getStringInput("Enter Aircraft Tail Number: ");
    auto aircraft = app_.getAircraftByTailNumber(tailNum);

    if (!aircraft) {
        std::cout << "Error: Aircraft " << tailNum << " not found.\n";
        return;
    }
    std::cout 
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
    std::cout << "Aircraft " << tailNum << " status updated to " << status << ".\n";
}

void ConsoleUI::handleRemoveAircraft() {
    std::cout << "\n--- Remove Aircraft ---\n";
    std::string tailNum = getStringInput("Enter Aircraft Tail Number to remove: ");

    if (app_.removeAircraft(tailNum)) {
        std::cout << "Aircraft " << tailNum << " removed successfully.\n";
    } else {
        std::cout << "Error: Aircraft " << tailNum << " not found.\n";
    }
}

void ConsoleUI::handleViewAllAircraft() {
    std::cout << "\n--- All Aircraft ---\n";
    const auto& aircrafts = app_.getAllAircraft();
    if (aircrafts.empty()) {
        std::cout << "No aircraft found.\n";
        return;
    }

    for (const auto& ac : aircrafts) {
        if (!ac) continue;
        std::cout << "Tail Number: " << ac->getTailNumber()
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
        std::cout << "\n--- Manage Users ---\n"
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
            default: std::cout << "Invalid choice. Please try again.\n"; break;
        }
    }
}

void ConsoleUI::handleAddUser() {
    std::cout << "\n--- Add New User ---\n";
    std::cout << "Select Role:\n"
              << "1. Administrator\n"
              << "2. Booking Agent\n"
              << "3. Passenger\n";

    int roleChoice = getIntInput("Enter role choice: ");
    auto role = intToRole(roleChoice);
    if(!role){
        std::cout << "Invalid role choice!";
        return;
    }
    std::string name     = getStringInput("Enter Full Name: ");
    std::string username = getStringInput("Enter Username: ");
    std::string password = getStringInput("Enter Password: ");
    std::string email    = getStringInput("Enter Email: ");
    std::string phone    = getStringInput("Enter Phone Number: ");

    bool success = app_.addUser(role.value(), name, username, password, email, phone);
    if (success) {
        std::cout << "\nUser '" << username << "' added successfully.\n";
    } else {
        std::cout << "\nError: Username already exists or role choice is invalid.\n";
    }
}

void ConsoleUI::handleUpdateUser() {
    std::cout << "\n--- Update Existing User ---\n";
    std::string username = getStringInput("Enter Username of User to update: ");

    auto user = app_.getUserByUsername(username);
    if (!user) {
        std::cout << "Error: User '" << username << "' not found.\n";
        return;
    }

    std::string newPassword = getStringInput("Enter New Password (or press enter to skip): ");
    std::string newName     = getStringInput("Enter New Name (or press enter to skip): ");

    if (!newPassword.empty()) user->setPassword(newPassword);
    if (!newName.empty())     user->setName(newName);

    std::cout << "User '" << username << "' updated successfully.\n";
}

void ConsoleUI::handleRemoveUser() {
    std::cout << "\n--- Remove User ---\n";
    std::string username = getStringInput("Enter Username to remove: ");

    if (app_.removeUser(username)) {
        std::cout << "User '" << username << "' successfully removed.\n";
    } else {
        std::cout << "Error: User '" << username << "' not found.\n";
    }
}

void ConsoleUI::handleViewAllUsers() {
    std::cout << "\n--- All Users ---\n";
    const auto& users = app_.getAllUsers();
    if (users.empty()) {
        std::cout << "No users found.\n";
        return;
    }

    for (const auto& user : users) {
        if (!user) continue;
        std::cout << "ID: " << user->getId()
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
    // Stub
}

void ConsoleUI::handleMaintenanceReport() {
    // Stub
}

void ConsoleUI::handleUserActivityReport() {
    // Stub
}

// ==========================================
// Booking Agent Handlers
// ==========================================

void ConsoleUI::handleAgentSearchFlights() {
    std::cout << "\n--- Booking Agent: Search Flights ---\n";

    std::string origin      = getStringInput("Enter Origin: ");
    std::string destination = getStringInput("Enter Destination: ");

    // Fetch matching flights for origin and destination
    auto matchingFlights = app_.searchFlights(origin, destination);

    if (matchingFlights.empty()) {
        std::cout << "\nNo flights found from " << origin << " to " << destination << ".\n";
        return;
    }

    // Prompt agent for date (press Enter to skip)
    std::cout << "Enter Target Departure Date (DD/MM/YYYY) [Press Enter to skip]: ";
    std::string dateInput;
    std::getline(std::cin, dateInput);

    if (!dateInput.empty()) {
        Date targetDate = Date::parseString(dateInput);

        // Sort matching flights chronologically (ascending order)
        std::sort(matchingFlights.begin(), matchingFlights.end(),
            [](const std::shared_ptr<Flight>& a, const std::shared_ptr<Flight>& b) {
                Date dateA = a->getDate();
                Date dateB = b->getDate();
                return !(dateA > dateB); // Ascending order
            });

        std::shared_ptr<Flight> flightBefore = nullptr;
        std::vector<std::shared_ptr<Flight>> flightsAfter;

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

        std::cout << "\n--- Search Results Near Date: " << dateInput << " ---\n";

        // 1. Single flight just before target date
        if (flightBefore) {
            std::cout << "\n[ Flight Just BEFORE " << dateInput << " ]\n"
                      << "Flight Number : " << flightBefore->getFlightNumber() << "\n"
                      << "Departure     : " << flightBefore->getDate() << "\n";
        } else {
            std::cout << "\n[ Flight Just BEFORE " << dateInput << " ] : None found.\n";
        }

        // 2. ALL flights on or after target date
        std::cout << "\n[ Flights On / AFTER " << dateInput << " ]\n";
        if (flightsAfter.empty()) {
            std::cout << "None found.\n";
        } else {
            int option = 1;
            for (const auto& flight : matchingFlights) {
                std::cout << option++ << ". Flight Number: " << flight->getFlightNumber() << "\n"
                        << "   Departure:     " << flight->getDate() << "\n";
            }
        }
    } else {
        // Date skipped: Show all matching flights directly
        std::cout << "\n--- All Available Flights (" << origin << " -> " << destination << ") ---\n";
        int option = 1;
        for (const auto& flight : matchingFlights) {
            std::cout << option++ << ". Flight Number: " << flight->getFlightNumber() << "\n"
                      << "   Departure:     " << flight->getDate() << "\n";
        }
    }

    // Optional flight details lookup
    std::cout << "\n";
    std::string flightChoice = getStringInput("Enter Flight Number to view details (or '0' to cancel): ");

    if (flightChoice == "0" || flightChoice.empty()) {
        return;
    }

    // Match chosen flight number
    std::shared_ptr<Flight> selectedFlight = nullptr;
    for (const auto& flight : matchingFlights) {
        if (flight->getFlightNumber() == flightChoice) {
            selectedFlight = flight;
            break;
        }
    }

    if (!selectedFlight) {
        std::cout << "Invalid Flight Number selected.\n";
        return;
    }

    // Display detailed flight information
    auto [firstAvail, bizAvail, econAvail] = selectedFlight->getAvailableSeatsPerClass();

    std::cout << "\n--- Flight Details (" << selectedFlight->getFlightNumber() << ") ---\n"
              << "Route           : " << selectedFlight->getOrigin() << " -> " << selectedFlight->getDestination() << "\n"
              << "Departure       : " << selectedFlight->getDate() << "\n"
              << "Available Seats : First: " << firstAvail 
              << " | Business: " << bizAvail 
              << " | Economy: " << econAvail << "\n";
}


void ConsoleUI::handleAgentBookConfirm() {
    std::cout << "\n--- Booking Agent: Confirm Booking Request ---\n";

    // Retrieve all booking requests via the application layer
    auto requests = app_.getAllBookingRequests();

    if (requests.empty()) {
        std::cout << "No booking requests available.\n";
        return;
    }

    // Display all booking requests with details
    std::cout << "\n--- Booking Requests List ---\n";
    for (const auto& req : requests) {
        if (!req) continue;

        auto passenger = req->passenger.lock();
        auto flight    = req->flight.lock();

        std::cout << "Request ID   : " << req->id << "\n"
                  << "  Passenger  : " << (passenger ? passenger->getName() + " (ID: " + passenger->getId() + ")" : "N/A") << "\n"
                  << "  Flight     : " << (flight ? flight->getFlightNumber() + " (" + flight->getOrigin() + " -> " + flight->getDestination() + ")" : "N/A") << "\n"
                  << "  Departure  : " << (flight ? toString(flight->getDate()) : "N/A") << "\n"
                  << "  Status     : " << toString(req->status) << "\n"
                  << "----------------------------------------\n";
    }

    // Prompt agent for choice
    std::string reqIdChoice = getStringInput("Enter Request ID to confirm (or '0' to cancel): ");

    if (reqIdChoice == "0" || reqIdChoice.empty()) {
        return;
    }

    // Call Application layer function to set status to ConfirmedBook
    bool success = app_.confirmBookingRequest(reqIdChoice);

    if (success) {
        std::cout << "\nSuccess: Booking Request '" << reqIdChoice 
                  << "' status updated to ConfirmedBook!\n"
                  << "The passenger can now convert this confirmed booking into a check-in request.\n";
    } else {
        std::cout << "\nError: Booking Request ID '" << reqIdChoice << "' not found.\n";
    }
}



void ConsoleUI::handleAgentCheckInConfirm() {
    std::cout << "\n--- Booking Agent: Confirm Check-In Request ---\n";

    // 1. Fetch check-in requests from Application layer
    auto requests = app_.getAllCheckInRequests();

    if (requests.empty()) {
        std::cout << "No check-in requests available.\n";
        return;
    }

    // 2. Display all check-in requests
    std::cout << "\n--- Check-In Requests List ---\n";
    for (const auto& req : requests) {
        if (!req) continue;

        auto passenger = req->passenger.lock();
        auto flight    = req->flight.lock();

        std::cout << "Check-In ID  : " << req->id << "\n"
                  << "  Passenger  : " << (passenger ? passenger->getName() + " (ID: " + passenger->getId() + ")" : "N/A") << "\n"
                  << "  Flight     : " << (flight ? flight->getFlightNumber() + " (" + flight->getOrigin() + " -> " + flight->getDestination() + ")" : "N/A") << "\n"
                  << "  Departure  : " << (flight ? toString(flight->getDate()) : "N/A") << "\n"
                  << "  Status     : " << toString(req->status) << "\n"
                  << "----------------------------------------\n";
    }

    // 3. Prompt agent for selection or cancel
    std::string reqIdChoice = getStringInput("Enter Check-In ID to confirm (or '0' to cancel): ");

    if (reqIdChoice == "0" || reqIdChoice.empty()) {
        return;
    }

    // 4. Update status via Application layer
    bool success = app_.confirmCheckInRequest(reqIdChoice);

    if (success) {
        std::cout << "\nSuccess: Check-In Request '" << reqIdChoice 
                  << "' has been successfully confirmed!\n";
    } else {
        std::cout << "\nError: Check-In Request ID '" << reqIdChoice << "' not found.\n";
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
    for (size_t i = 0; i < flights.size(); ++i) {
        cout << (i + 1) << ". Flight Number: " << flights[i]->getFlightNumber() << "\n"
                << "   Departure: " << flights[i]->getDate() << "\n";
    }

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

    int classChoice = getIntInput("Select Seat Class (1-3): ");
    SeatClass chosenClass = SeatClass::Economy;
    if (classChoice == 1) chosenClass = SeatClass::First;
    else if (classChoice == 2) chosenClass = SeatClass::Business;

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
    std::cout << "\n--- My Reservations & Travel History ---\n";

    // 1. Retrieve the logged-in passenger
    auto currentPassenger = std::dynamic_pointer_cast<airline::Passenger>(app_.getCurrentUser());
    if (!currentPassenger) {
        std::cout << "Error: Current logged-in user is not a valid Passenger.\n";
        return;
    }

    std::cout << "Fetching reservations and travel history for Passenger " << currentPassenger->getName() << "...\n\n";

    // 2. Fetch booking requests, check-in requests, and travel history
    const auto& bookings      = currentPassenger->getBookingRequests();
    const auto& checkIns      = currentPassenger->getCheckInRequests(); 
    const auto& travelHistory = currentPassenger->getTravelHistory(); // or getFinishedRequests()

    if (bookings.empty() && checkIns.empty() && travelHistory.empty()) {
        std::cout << "No active reservations or travel history found.\n";
        return;
    }

    // 3. Display Booking Requests
    std::cout << "=== Booking Requests ===\n";
    if (bookings.empty()) {
        std::cout << "No booking requests found.\n\n";
    } else {
        for (size_t i = 0; i < bookings.size(); ++i) {
            const auto& req = bookings[i];
            if (!req) continue;

            std::cout << (i + 1) << ". Booking ID : " << req->id << "\n";
            if (auto flight = req->flight.lock()) {
                std::cout << "   Flight     : " << flight->getFlightNumber() 
                          << " (" << flight->getOrigin() << " -> " << flight->getDestination() << ")\n"
                          << "   Departure  : " << flight->getDate() << "\n";
            } else {
                std::cout << "   Flight     : N/A\n";
            }
            std::cout << "   Seat Class : " << airline::toString(req->seatClass) << "\n";
            std::cout << "   Price      : $" << req->price << "\n";
            std::cout << "   Status     : " << airline::toString(req->status) << "\n\n";
        }
    }

    // 4. Display Check-In Requests
    std::cout << "=== Check-In Requests ===\n";
    if (checkIns.empty()) {
        std::cout << "No check-in requests found.\n\n";
    } else {
        for (size_t i = 0; i < checkIns.size(); ++i) {
            const auto& req = checkIns[i];
            if (!req) continue;

            std::cout << (i + 1) << ". Check-In ID: " << req->id << "\n";
            if (auto flight = req->flight.lock()) {
                std::cout << "   Flight     : " << flight->getFlightNumber() 
                          << " (" << flight->getOrigin() << " -> " << flight->getDestination() << ")\n"
                          << "   Departure  : " << flight->getDate() << "\n";
            } else {
                std::cout << "   Flight     : N/A\n";
            }
            std::cout << "   Seat Class : " << airline::toString(req->seatClass) << "\n";
            std::cout << "   Price      : $" << req->price << "\n";
            std::cout << "   Status     : " << airline::toString(req->status) << "\n\n";
        }
    }

    // 5. Display Travel History (Completed / Confirmed Requests)
    std::cout << "=== Travel History ===\n";
    if (travelHistory.empty()) {
        std::cout << "No past travel history found.\n\n";
    } else {
        for (size_t i = 0; i < travelHistory.size(); ++i) {
            const auto& record = travelHistory[i];

            std::cout << (i + 1)  << "   Flight      : " << record.flightNumber 
                      << " (" << record.origin << " -> " << record.destination << ")\n"
                      << "   Departure   : " << record.date << "\n"
                      << "   Seat Class  : " << airline::toString(record.seatClass) << "\n"
                      << "   Price       : $" << record.price << "\n";
        }
    }
}

void ConsoleUI::handleCheckIn() {
    std::cout << "\n--- Check-In ---\n";

    auto currentPassenger = std::dynamic_pointer_cast<airline::Passenger>(app_.getCurrentUser());
    if (!currentPassenger) {
        std::cout << "Error: Current logged-in user is not a valid Passenger.\n";
        return;
    }

    std::string resId = getStringInput("Enter Reservation ID (if you don't know it, invoke '2. View My Reservations' ): ");

    // Verify reservation ownership directly from passenger's booking list
    std::shared_ptr<airline::BookingRequest> bookReq = nullptr;
    for (const auto& booking : currentPassenger->getBookingRequests()) {
        if (booking && booking->id == resId) {
            bookReq = booking;
            break;
        }
    }

    if (!bookReq) {
        std::cout << "\nError: Reservation ID " << resId << " not found or does not belong to you.\n";
        return;
    }

    if (bookReq->status != airline::ReservationStatus::ConfirmedBook) {
        std::cout << "\nCheck-In failed: Your reservation is currently " 
                  << toString(bookReq->status) 
                  << ". It must be Confirmed by an agent first.\n";
        return;
    }

    // Submit check-in request
    auto checkInReq = app_.createCheckInRequest(currentPassenger, bookReq);
    if (!checkInReq) {
        std::cout << "\nCheck-In failed: System could not process check-in request.\n";
        return;
    }

    // Lock weak_ptr<Flight> to print boarding pass details
    auto flight = bookReq->flight.lock();

    std::cout << "\nCheck-In Successful!\n"
              << "Boarding Pass:\n"
              << "-----------------------------\n"
              << "Reservation ID: " << checkInReq->id << "\n"
              << "Passenger: " << currentPassenger->getName() << "\n";

    if (flight) {
        std::cout << "Flight: " << flight->getFlightNumber() << "\n"
                  << "Origin: " << flight->getOrigin() << "\n"
                  << "Destination: " << flight->getDestination() << "\n"
                  << "Departure: " << flight->getDate() << "\n";
    } else {
        std::cout << "Flight: N/A\n";
    }

    std::cout << "-----------------------------\n";
}

} // namespace airline
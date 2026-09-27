#include "ui/ConsoleUI.hpp"
#include "app/AirlineApplication.hpp"
#include <iostream>
#include <sstream>
#include <limits>

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

float ConsoleUI::getFloatInput(const std::string& prompt) {
    std::string input;
    float value = 0.0f;

    while (true) {
        std::cout << prompt;
        if (std::getline(std::cin, input)) {
            std::stringstream ss(input);
            // Parse float and ensure no trailing invalid characters exist
            if (ss >> value && (ss >> std::ws).eof()) {
                return value;
            }
        }
        
        // Handle stream error or invalid conversion
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }

        std::cout << "Invalid input. Please enter a valid floating-point number (e.g., 2.5).\n";
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
            case 1:
                showManageFlightsMenu();
                break;
            case 2:
                showManageAircraftMenu();
                break;
            case 3:
                showManageUsersMenu();
                break;
            case 4:
                showGenerateReportsMenu();
                break;
            case 5:
                inMenu = false;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
        }
    }
}

void ConsoleUI::showAgentMenu() {
    bool inMenu = true;
    while (inMenu) {
        cout << "\n--- Booking Agent Menu ---\n"
                  << "1. Search Flights\n"
                  << "2. Book a Flight\n"
                  << "3. Modify Reservation\n"
                  << "4. Cancel Reservation\n"
                  << "5. Logout\n";

        int choice = getIntInput("Enter choice: ");
        switch (choice) {
            case 1:
                handleAgentSearchFlights();
                break;
            case 2:
                handleAgentBookFlight();
                break;
            case 3:
                handleModifyReservation();
                break;
            case 4:
                handleCancelReservation();
                break;
            case 5:
                inMenu = false;
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
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
        selection = getIntInput("\nSelect Aircraft (1-" + std::to_string(availableAircrafts.size()) + "): ");
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
    bool success = app_.addFlight(flightNum, origin, destination, depTime, selectedAircraft, flightDur, reg);

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
            int statusInt = std::stoi(newStatusStr);
            flight->setStatus(static_cast<FlightStatus>(statusInt));
            cout << "Flight status updated successfully.\n";
        } else if (choice == 4) {
            break;
        }
    }
}

void ConsoleUI::handleflightDetailUpdate(std::shared_ptr<Flight> flight) {
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
            } catch (const std::exception& e) {
                cout << "Invalid date format -- departure time NOT updated (" << e.what() << ").\n";
            }
        }
    } else if (choice == 4) {
        auto newDurStr = getStringInput("Enter new duration in hours, or leave blank to keep current: ");
        if (newDurStr.empty()) {
            cout << "Duration unchanged.\n";
        } else {
            try {
                float newDuration = std::stof(newDurStr);
                flight->setDuration(newDuration);
                cout << "Duration updated successfully.\n";
            } catch (const std::exception& e) {
                cout << "Invalid duration -- NOT updated (" << e.what() << ").\n";
            }
        }
    } else if (choice == 5) {
        return;
    } else {
        cout << "Invalid choice.\n";
    }
}

void ConsoleUI::handleAssignCrew(const std::shared_ptr<Flight>& flight) {
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

        int selection = getIntInput("Select Pilot (1-" + std::to_string(availablePilots.size()) + "): ");
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

        int selection = getIntInput("Select Flight Attendant (1-" + std::to_string(availableFAs.size()) + "): ");
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
    // Stub
}

void ConsoleUI::showManageUsersMenu() {
    // Stub
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
    // Stub
}

void ConsoleUI::handleAgentBookFlight() {
    // Stub
}

void ConsoleUI::handleModifyReservation() {
    // Stub
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

    // Display available seat tier summary
    auto [firstAvail, bizAvail, econAvail] = selectedFlight->getAvailableSeatsPerTier();

    cout << "\nAvailable Seats on Flight " << selectedFlight->getFlightNumber() << ":\n"
              << "1. First Class (" << firstAvail << " left)\n"
              << "2. Business Class (" << bizAvail << " left)\n"
              << "3. Economy Class (" << econAvail << " left)\n";

    int classChoice = getIntInput("Select Seat Class (1-3): ");
    SeatClass chosenClass = SeatClass::Economy;
    if (classChoice == 1) chosenClass = SeatClass::First;
    else if (classChoice == 2) chosenClass = SeatClass::Business;

    cout << "\nSelect Seat Position:\n"
              << "1. Window\n"
              << "2. Aisle\n"
              << "3. Middle\n";

    int posChoice = getIntInput("Select Position (1-3): ");
    SeatPosition chosenPos = SeatPosition::Aisle;
    if (posChoice == 1) chosenPos = SeatPosition::Window;
    else if (posChoice == 3) chosenPos = SeatPosition::Middle;

    // Single system call handles find seat + assign seat + create request
    auto currentPassenger = dynamic_pointer_cast<Passenger>(app_.getCurrentUser());
    auto bookingReq = app_.createBookingRequest(currentPassenger, selectedFlight, chosenClass, chosenPos);

    if (bookingReq) {
        cout << "\nBooking successful!\n"
                  << "Reservation ID: " << bookingReq->id << "\n"
                  << "Flight: " << selectedFlight->getFlightNumber() << "\n";
    } else {
        cout << "\nError: Selected seat option is no longer available or occupied.\n";
    }
}

void ConsoleUI::handleViewMyReservations() {
    cout << "\n--- My Reservations ---\n";

    auto currentPassenger = dynamic_pointer_cast<Passenger>(app_.getCurrentUser());
    if (!currentPassenger) {
        cout << "Error: Current logged-in user is not a valid Passenger.\n";
        return;
    }

    cout << "Fetching reservations for Passenger " << currentPassenger->getUsername() << "...\n\n";

    auto bookings = app_.getBookingRequestsForPassenger(currentPassenger);
    if (bookings.empty()) {
        cout << "No reservations found.\n";
        return;
    }

    for (size_t i = 0; i < bookings.size(); ++i) {
        const auto& req = bookings[i];
        cout << (i + 1) << ". Reservation ID: " << req->id << "\n"
                  << "   Flight: " << req->flight->getFlightNumber() 
                  << " from " << req->flight->getOrigin() 
                  << " to " << req->flight->getDestination() << "\n"
                  << "   Departure: " << req->flight->getDate() << "\n"
                  << "   Status: " << statusToString(req->status) << "\n\n";
    }
}

void ConsoleUI::handleCheckIn() {
    cout << "\n--- Check-In ---\n";

    auto currentPassenger = dynamic_pointer_cast<Passenger>(app_.getCurrentUser());
    if (!currentPassenger) {
        cout << "Error: Current logged-in user is not a valid Passenger.\n";
        return;
    }

    string resId = getStringInput("Enter Reservation ID: ");

    // Verify reservation ownership
    auto myBookings = app_.getBookingRequestsForPassenger(currentPassenger);
    shared_ptr<BookingRequest> targetBooking = nullptr;

    for (const auto& booking : myBookings) {
        if (booking->id == resId) {
            targetBooking = booking;
            break;
        }
    }

    if (!targetBooking) {
        cout << "\nError: Reservation ID " << resId << " not found or does not belong to you.\n";
        return;
    }

    if (targetBooking->status != RequestStatus::Confirmed) {
        cout << "\nCheck-In failed: Your reservation is currently " 
                  << statusToString(targetBooking->status) 
                  << ". It must be Confirmed by an agent first.\n";
        return;
    }

    // Submit check-in request if not already submitted
    auto checkInReq = app_.getCheckInRequest(currentPassenger, targetBooking->flight);
    if (!checkInReq) {
        checkInReq = app_.createCheckInRequest(currentPassenger, targetBooking->flight);
    }

    cout << "\nCheck-In Successful!\n"
              << "Boarding Pass:\n"
              << "-----------------------------\n"
              << "Reservation ID: " << targetBooking->id << "\n"
              << "Passenger: " << currentPassenger->getName() << "\n"
              << "Flight: " << targetBooking->flight->getFlightNumber() << "\n"
              << "Origin: " << targetBooking->flight->getOrigin() << "\n"
              << "Destination: " << targetBooking->flight->getDestination() << "\n"
              << "Departure: " << targetBooking->flight->getDate() << "\n"
              << "-----------------------------\n";
}

} // namespace airline
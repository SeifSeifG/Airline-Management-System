#pragma once
#include "app/AirlineApplication.hpp"

namespace airline {

class ConsoleUI {
private:
    AirlineApplication& app_;
    bool running_{true};

    // Utility Helpers
    int getIntInput(const std::string& prompt);
    float getFloatInput(const std::string& prompt);
    std::string getStringInput(const std::string& prompt);

    // Role Selection & Login Routines
    void showRoleMenu();
    void handleAdminLogin();
    void handleAgentLogin();
    void handlePassengerLogin();

    // Role Main Menus
    void showAdminMenu();
    void showAgentMenu();
    void showPassengerMenu();

    // --- Administrator Handlers & Sub-Menus ---
    void showManageFlightsMenu();
    void handleAddFlight();
    void handleUpdateFlight();
    void handleflightDetailUpdate(std::shared_ptr<Flight> flight);
    void handleAssignCrew(const std::shared_ptr<Flight>& flight);
    void handleRemoveFlight();
    void handleViewAllFlights();

    void showManageAircraftMenu();
    void showManageUsersMenu();

    void showGenerateReportsMenu();
    void handleOperationalReport();
    void handleMaintenanceReport();
    void handleUserActivityReport();

    // --- Booking Agent Handlers ---
    void handleAgentSearchFlights();
    void handleAgentBookFlight();
    void handleModifyReservation();
    void handleCancelReservation();

    // --- Passenger Handlers ---
    void handlePassengerSearchFlights();
    void handleViewMyReservations();
    void handleCheckIn();
public:
    explicit ConsoleUI(AirlineApplication& app);
    void run();
};

} // namespace airline
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

    // entry points to any role account
    void showRoleMenu();
        void handleAdminLogin();
        void handleAgentLogin();
        void handlePassengerLogin();
    
    // --- Administrator Handlers ---
    void showAdminMenu();
        void showManageFlightsMenu();
            void handleAddFlight();
            void handleUpdateFlight();
                void handleflightDetailUpdate(const std::shared_ptr<Flight>& flight);
                void handleAssignCrew(const std::shared_ptr<Flight>& flight);
            void handleRemoveFlight();
            void handleViewAllFlights();

        void showManageAircraftMenu();
            void handleAddAircraft();
            void handleUpdateAircraft();
            void handleRemoveAircraft();
            void handleViewAllAircraft();

        void showManageUsersMenu();
            void handleAddUser();
            void handleUpdateUser();
            void handleRemoveUser();
            void handleViewAllUsers();

        void showGenerateReportsMenu();
            void handleOperationalReport();
            void handleMaintenanceReport();
            void handleUserActivityReport();

    // --- Booking Agent Handlers ---
    void showAgentMenu();
        void handleAgentSearchFlights();
        void handleAgentBookConfirm();
        void handleAgentCheckInConfirm();
        void handleCancelReservation();

    // --- Passenger Handlers ---
    void showPassengerMenu();
        void handlePassengerSearchFlights();
        void handleViewMyReservations();
        void handleCheckIn();
        void handleRechargeBalance();
public:
    explicit ConsoleUI(AirlineApplication& app);
    void run();
};

} // namespace airline
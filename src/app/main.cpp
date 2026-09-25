#include "app/AirlineApplication.hpp"
#include "domain/Defs.hpp"
#include <iostream>

int main() {
    using namespace airline;

    AirlineApplication app;

    // ---- Step 1: load the seed file (plaintext passwords) ----
    std::cout << ">>> Loading seed data (plaintext passwords)...\n";
    try {
        app.initialize("data/airline_data.json");
    } catch (const std::exception& e) {
        std::cerr << "Failed to load seed data: " << e.what() << "\n";
        return 1;
    }
    app.printSummary();

    // ---- Step 1.5: Demo registration of a new passenger ----
    std::cout << "\n>>> Registering a new passenger...\n";
    contactInfo newContact{"alice.smith@example.com", "555-0199"};
    std::string newUsername = "alicesmith";
    std::string newPassword = "Password123!";

    if (app.registerNewPassenger("Alice Smith", newContact, newUsername, newPassword)) {
        std::cout << "Successfully registered passenger: " << newUsername << "\n";
    } else {
        std::cerr << "Failed to register new passenger.\n";
        return 1;
    }

    // Verify login on the active instance
    if (app.login(newUsername, newPassword)) {
        std::cout << "Login test successful on current instance.\n";
        app.logout();
    } else {
        std::cerr << "Login test failed on current instance!\n";
        return 1;
    }

    // ---- Step 2: save what was loaded and modified to a separate file ----
    std::cout << "\n>>> Saving current state to JSON...\n";
    try {
        app.saveToFile("data/airline_data_saved.json");
    } catch (const std::exception& e) {
        std::cerr << "Failed to save: " << e.what() << "\n";
        return 1;
    }
    std::cout << "Saved to data/airline_data_saved.json\n";

    // ---- Step 3: load the SAVED file into a fresh AirlineApplication ----
    std::cout << "\n>>> Reloading the saved file into a fresh instance...\n";
    AirlineApplication reloaded;
    try {
        reloaded.initialize("data/airline_data_saved.json");
    } catch (const std::exception& e) {
        std::cerr << "Failed to reload saved data: " << e.what() << "\n";
        return 1;
    }
    reloaded.printSummary();

    // ---- Step 4: Verify the newly created passenger exists in the reloaded data ----
    std::cout << "\n>>> Testing login for the new passenger on the reloaded instance...\n";
    if (reloaded.login(newUsername, newPassword)) {
        std::cout << "SUCCESS: Logged in as '" << reloaded.getCurrentUser()->getName() 
                  << "' (" << newUsername << ") from reloaded JSON file!\n";
        reloaded.logout();
    } else {
        std::cerr << "FAILURE: Could not log in with saved passenger credentials on reloaded instance.\n";
        return 1;
    }

    std::cout << "\n>>> Round trip complete.\n";
    return 0;
}
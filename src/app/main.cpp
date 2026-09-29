#include "app/AirlineApplication.hpp"
#include "domain/Passenger.hpp"
#include "domain/Administrator.hpp"
#include "domain/BookingAgent.hpp"
#include "ui/ConsoleUI.hpp"
#include <iostream>

int main() {
    using namespace airline;
    try {
        AirlineApplication app("data/airline_data.json");
        ConsoleUI ui(app);
        ui.run();
    } catch (const std::exception& e) {
        std::cerr << "Failed to start application: " << e.what() << "\n";
        return 1;
    }
}




// using namespace airline;
// int main() {
//     std::vector<std::shared_ptr<User>> users;

//     // Creating objects without passing an ID
//     users.push_back(std::make_shared<Administrator>("Alice Smith", "alice@airline.com", "555-0100", "admin_alice", "pass123"));
//     users.push_back(std::make_shared<BookingAgent>("Bob Jones", "bob@airline.com", "555-0200", "agent_bob", "pass456"));
//     users.push_back(std::make_shared<Passenger>("Charlie Brown", "charlie@gmail.com", "555-0300", "cbrown", "pass789"));
//     users.push_back(std::make_shared<Passenger>("Diana Prince", "diana@gmail.com", "555-0400", "dprince", "pass999"));
//     users.push_back(std::make_shared<Administrator>("Eve Adams", "eve@airline.com", "555-0500", "admin_eve", "pass000"));

//     std::cout << "--- Generated User Records ---\n";
//     for (const auto& user : users) {
//         std::cout << user->getId() << "\n";
//     }

//     return 0;
// }
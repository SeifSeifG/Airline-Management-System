#include "app/AirlineApplication.hpp"
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
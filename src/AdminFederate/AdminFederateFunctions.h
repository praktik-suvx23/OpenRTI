#include <iostream>
#include <limits>

bool userInteraction() {
    std::string input = "temp";
    while(true){
        std::getline(std::cin, input);
        if (input == "") {
            break;
        }
    }
    return true;
}

int getValidIntInput() {
    int input = 0;
    while (true) {
        std::cin >> input;
        if (std::cin.fail() || input <= 0) {
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            std::wcout << "Invalid input. Please enter a positive whole number: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            return input;
        }
    }
}

int getValidDoubleInput() {
    double input = 0.0;
    while (true) {
        std::cin >> input;
        if (std::cin.fail() || input <= 0) {
            std::cin.clear(); // Clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Ignore invalid input
            std::wcout << "Invalid input. Please enter a positive number: ";
        } else {
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Clear the buffer
            return input;
        }
    }
}
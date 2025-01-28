#include "../include/Federate.h"
#include "../include/fedAmbassador.h"
#include <string>
#include <iostream>

std::wstring toWideString(const std::string& str) {
    std::wstring wstr(str.begin(), str.end());
    return wstr;
}

int main(int argc, char* argv[]) {
    // Check if we have a federate name
    std::wstring federateName = L"testFederate";
    if (argc > 1) {
        federateName = toWideString(std::string(argv[1]));
    }

    // Create and run the federate
    Federate federate;
    federate.runFederate(federateName);

    std::cout << "Federate execution completed." << std::endl;
    return 0;
}
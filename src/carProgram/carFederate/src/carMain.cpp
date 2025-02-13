#include "../include/carFederate.h"
#include "../include/carFedAmb.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])  {
    std::wstring federateName = L"carFederate";  // Set your federate name

    try {
        carFederate myFederate;
        myFederate.runFederate(federateName);
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}
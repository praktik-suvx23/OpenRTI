#include "../include/scenarioFederate.h"
#include "../include/scenarioFedAmb.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])  {
    std::wstring federateName = L"printFederate";  // Set your federate name

    try {
        scenarioFederate myFederate;
        myFederate.runFederate(federateName);
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}
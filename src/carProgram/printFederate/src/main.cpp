#include "../include/printFederate.h"
#include "../include/printFedAmb.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])  {
    std::wstring federateName = L"printFederate";  // Set your federate name

    try {
        mastFederate myFederate;
        myFederate.runFederate(federateName);
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}
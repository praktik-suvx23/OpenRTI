#include "../include/masterFederate.h"
#include "../include/masterFedAmbassador.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])  {
    std::wstring federateName = L"MyFederate";  // Set your federate name

    try {
        masterFederate myFederate;
        myFederate.runFederate(L"MyFederate");
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}
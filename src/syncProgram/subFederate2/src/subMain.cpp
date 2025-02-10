#include "../include/subFederate.h"
#include "../include/subFedAmb.h"
#include <string>
#include <iostream>

int main(int argc, char* argv[])  {
    std::wstring federateName = L"MyFederate";  // Set your federate name

    try {
        subFederate myFederate;
        myFederate.runFederate(L"MyFederate");
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }

    return 0;
}
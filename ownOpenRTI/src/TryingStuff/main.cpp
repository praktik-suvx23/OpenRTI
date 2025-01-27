#include "testFederate.h"
#include "testFedAmb.h"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    // check if we have a federate name
    std::wstring federateName = L"testFederate";
    if(argc > 1){
        federateName = toWideString(std::string(argv[1]));
    }

    // create and run the federate
    testFederate *federate;
    federate = new testFederate();
    federate->runFederate(federateName);

    // clean up
    delete federate;
    return 0;
}
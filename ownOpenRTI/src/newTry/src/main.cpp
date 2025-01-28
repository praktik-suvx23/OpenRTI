#include "../include/Federate.h"
#include "../include/fedAmbassador.h"
#include <string>
#include <iostream>

std::wstring toWideString(const std::string& str) {
    std::wstring wstr(str.begin(), str.end());
    return wstr;
}

int main(int argc, char* argv[]) {
    // check if we have a federate name
    std::wstring federateName = L"testFederate";
    if(argc > 1){
        federateName = toWideString(std::string(argv[1]));
    }
    
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
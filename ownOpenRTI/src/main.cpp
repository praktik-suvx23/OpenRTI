#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <cstring>
#include <fstream>
#include <cstdlib>
#include <thread>
#include <chrono>

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    void federationCreated(const std::wstring& federationName)  {
        std::wcout << L"Federation created: " << federationName << std::endl;
    }

    void federationDestroyed(const std::wstring& federationName)  {
        std::wcout << L"Federation destroyed: " << federationName << std::endl;
    }
};

void openFileCheck(const std::string& fomFilePath) {
    // Attempt to open the FOM file
    std::cout << "FOM file path: " << fomFilePath << std::endl;

    // Attempt to open the FOM file
    std::ifstream file(fomFilePath);

    if (file.is_open()) {
        std::cout << "FOM.xml file successfully opened!" << std::endl;

        // Read and print the content of the FOM file
        /*
        std::string line;
        while (std::getline(file, line)) {
            std::cout << line << std::endl;
        }
        */
    } else {
        std::cout << "Error: Unable to open FOM.xml file at path: " << fomFilePath << std::endl;
    }
}

int main(int argc, char* argv[]) {
    try {
        rti1516e::RTIambassadorFactory factory;
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador();

        MyFederateAmbassador ambassador;

        rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

        std::wstring federationName = L"MyFederation";
        std::wstring federateName = L"MyFederate";
        std::wstring fromFomFile = L"../src/FOM.xml";

        std::string fomFilePath = "../src/FOM.xml";
        openFileCheck(fomFilePath);

        try{
            rtiAmbassador->createFederationExecution(federationName, fromFomFile);
            std::wcout << L"Federation created successfully." << std::endl;
        } catch (rti1516e::FederationExecutionAlreadyExists&) {
            std::wcout << L"Federation already exists." << std::endl;
        }

        // Join federation
        rtiAmbassador->joinFederationExecution(federateName, federationName);;
        std::wcout << L"Joined federation." << std::endl;

        try {
            rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
            std::wcout << L"Vehicle class handle: " << vehicleClassHandle << std::endl;
        } catch (const rti1516e::NameNotFound& e) {
            std::cerr << "Exception: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
            // Handle the error, possibly by checking the FOM file or initialization steps 
        }

        // Check if still connected to the federation
        try {
            rtiAmbassador->queryFederationSaveStatus();
            std::wcout << L"Still connected to the federation." << std::endl;
        } catch (const rti1516e::Exception& e) {
            std::cerr << "Not connected to the federation: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
        }

        // Resign from federation
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;

        // Destroy federation
        try {
            rtiAmbassador->destroyFederationExecution(federationName);
            std::wcout << L"Federation destroyed successfully." << std::endl;
        } catch (rti1516e::FederationExecutionDoesNotExist&) {
            std::wcout << L"Federation does not exist." << std::endl;
        }

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
    }

    return 0;
}
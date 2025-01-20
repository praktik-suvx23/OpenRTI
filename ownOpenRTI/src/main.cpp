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
    void discoverObjectInstance(rti1516e::ObjectInstanceHandle theObject, 
                                rti1516e::ObjectClassHandle theObjectClass, 
                                std::wstring const& theObjectName) override {
        // This method is called when an object instance is discovered
        std::wcout << L"Discovered object: " << theObjectName << std::endl;
        
        // You can also retrieve attributes associated with the object, if needed
        // For example, if the object class represents "Vehicle":
        if (theObjectName == L"Vehicle") {
            std::wcout << L"Vehicle object discovered: " << theObject << std::endl;
            // You can subscribe to this object or perform other actions as needed
        }
    }

    void reflectAttributeValues(rti1516e::ObjectInstanceHandle theObject,
                                rti1516e::AttributeHandleValueMap const& theAttributes,
                                rti1516e::VariableLengthData const& theTag,
                                rti1516e::OrderType sentOrder,
                                rti1516e::TransportationType theType,
                                rti1516e::LogicalTime const& theTime,
                                rti1516e::OrderType receivedOrder,
                                rti1516e::MessageRetractionHandle theHandle,
                                rti1516e::SupplementalReflectInfo theReflectInfo) {
        for (const auto& attribute : theAttributes) {
            if (attribute.first == positionHandle) {
                std::string positionValue(reinterpret_cast<const char*>(attribute.second.data()), attribute.second.size());
                std::wcout << L"Received Position: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(positionValue) << std::endl;
            } else if (attribute.first == speedHandle) {
                double speedValue;
                std::memcpy(&speedValue, attribute.second.data(), attribute.second.size());
                std::wcout << L"Received Speed: " << speedValue << std::endl;
            }
        }
    }

    rti1516e::AttributeHandle positionHandle;
    rti1516e::AttributeHandle speedHandle;
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

void updateAttributes(rti1516e::RTIambassador* rtiAmbassador, rti1516e::ObjectInstanceHandle vehicleHandle, MyFederateAmbassador& ambassador, int myNumber) {
    // Create an empty VariableLengthData for the tag
    rti1516e::VariableLengthData tag;

    // Set values for the Vehicle attributes
    rti1516e::AttributeHandleValueMap attributes;

    // Update position and speed values
    std::string positionValue = std::to_string(100.0 + myNumber * 10);  // Example update logic for Position
    rti1516e::VariableLengthData positionData(reinterpret_cast<const void*>(positionValue.data()), positionValue.size());
    attributes[ambassador.positionHandle] = positionData;

    double speedValue = 50.0 + myNumber * 5;  // Example update logic for Speed
    rti1516e::VariableLengthData speedData(reinterpret_cast<const void*>(&speedValue), sizeof(speedValue));
    attributes[ambassador.speedHandle] = speedData;

    // Update the attribute values
    rtiAmbassador->updateAttributeValues(vehicleHandle, attributes, tag);

    // Output the updated values
    std::cout << "Updated Position Value: " << positionValue << std::endl;
    std::cout << "Updated Speed Value: " << speedValue << std::endl;
}

int main() {
    try {
        const char* sourceDirectory = std::getenv("Source_Directory");
        if (!sourceDirectory) {
            std::cerr << "Error: Source_Directory environment variable is not set." << std::endl;
            return 1;
        }
        //std::string(sourceDirectory) + "/src/FOM.xml"
        std::string fomFilePath = "../src/FOM.xml";
        openFileCheck(fomFilePath);

        // Create RTI Ambassador and Federate Ambassador
        rti1516e::RTIambassadorFactory factory;
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador();
        
        MyFederateAmbassador ambassador;
        rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

        // Federation and federate setup
        rtiAmbassador->createFederationExecution(L"MyFederation", std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(fomFilePath));
        rtiAmbassador->joinFederationExecution(L"MyFederate", L"MyFederation");

        std::cout << "Federate joined the federation." << std::endl;

        // Discover handles for the object class and attributes
        rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        ambassador.positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        ambassador.speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        // Subscribe to the Vehicle object class and its attributes
        rtiAmbassador->subscribeObjectClassAttributes(vehicleClassHandle, {ambassador.positionHandle, ambassador.speedHandle});

        // Create an object instance of the Vehicle class
        rti1516e::ObjectInstanceHandle vehicleHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);

        // Set initial values for the Vehicle attributes
        updateAttributes(rtiAmbassador.get(), vehicleHandle, ambassador, 0);

        // Run simulation loop or additional logic here...
        int myNumber = 1;
        while (myNumber < 10) {
            updateAttributes(rtiAmbassador.get(), vehicleHandle, ambassador, myNumber);

            // Sleep for a short duration to simulate time passing (optional)
            std::this_thread::sleep_for(std::chrono::seconds(1));

            myNumber++;
        }

        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        rtiAmbassador->destroyFederationExecution(L"MyFederation");

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
    }
    return 0;
}
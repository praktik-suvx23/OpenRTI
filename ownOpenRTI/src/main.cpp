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

class MyFederateAmbassador : public rti1516e::NullFederateAmbassador {
public:
    void discoverObjectInstance(const rti1516e::ObjectClassHandle& objectHandle, 
                                const rti1516e::FederateHandle& federateHandle, 
                                const char* objectName) {
        // Log or handle object instance discovery here
        std::cout << "Discovered object: " << objectName << std::endl;
    }
};

void openFileCheck(){
        const std::string fomFilePath = "/root/GitHub_temp/apa/OpenRTI/ownOpenRTI/src/FOM.xml";  // Replace with your actual file path

        // Attempt to open the FOM file
        std::ifstream file(fomFilePath);

        if (file.is_open()) {
            std::cout << "FOM.xml file successfully opened!" << std::endl;
            // Print out content of the file
            /*
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;
            }
            */
            // You can add further debugging actions here, like reading the file content if needed
        } else {
            std::cout << "Error: Unable to open FOM.xml file at path: " << fomFilePath << std::endl;
        }
    }

int main() {
    try {
        openFileCheck();
        
        // Create RTI Ambassador and Federate Ambassador
        rti1516e::RTIambassadorFactory factory;
        std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador();

        std::cout << "RTI ambassador created." << std::endl;
        
        MyFederateAmbassador ambassador;
        rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

        std::cout << "Federate ambassador connected." << std::endl;

        // Federation and federate setup
        rtiAmbassador->createFederationExecution(L"MyFederation", L"/root/GitHub_temp/apa/OpenRTI/ownOpenRTI/src/FOM.xml");
        std::cout << "Federation execution created." << std::endl;
        rtiAmbassador->joinFederationExecution(L"MyFederate", L"MyFederation");

        std::cout << "Federate joined the federation." << std::endl;

        // Discover handles for the object class and attributes
        rti1516e::ObjectClassHandle vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
        rti1516e::AttributeHandle positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        rti1516e::AttributeHandle speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        // Register the object instance
        rti1516e::ObjectInstanceHandle vehicleHandle = rtiAmbassador->registerObjectInstance(vehicleClassHandle);

        // Publish object class and its attributes using the appropriate method
        rti1516e::AttributeHandleSet attributesToPublish;
        attributesToPublish.insert(positionHandle);  // Use insert instead of push_back
        attributesToPublish.insert(speedHandle);
        
        rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, attributesToPublish);  // Publish object class and attributes

        std::cout << "Object class and attributes published." << std::endl;

        // Create an empty VariableLengthData for the tag
        rti1516e::VariableLengthData tag;

        // Set values for the Vehicle attributes
        rti1516e::AttributeHandleValueMap attributes;

        // Position is a String attribute
        std::string positionValue = "100.0";  // Example string value for Position
        rti1516e::VariableLengthData positionData(reinterpret_cast<const void*>(positionValue.data()), positionValue.size());
        attributes[positionHandle] = positionData;

        // Speed is a Float attribute
        double speedValue = 50.0;  // Example float value for Speed
        rti1516e::VariableLengthData speedData(reinterpret_cast<const void*>(&speedValue), sizeof(speedValue));
        attributes[speedHandle] = speedData;

        // Send the update to the RTI
        rtiAmbassador->updateAttributeValues(vehicleHandle, attributes, tag);

        std::cout << "Vehicle object updated with Position and Speed." << std::endl;

        // (Optional) Retrieve and print the updated attribute values for confirmation
        rti1516e::AttributeHandleValueMap receivedValues;
        // Retrieve values using an appropriate method
        // rtiAmbassador->getAttributeValues(vehicleHandle, receivedValues);

        std::cout << "112" << std::endl;

        // Print out the received values
        for (const auto& entry : attributes) {
            if (entry.first == positionHandle) {
                std::string receivedPosition(reinterpret_cast<const char*>(entry.second.data()), entry.second.size());  // Convert to string
                std::cout << "Vehicle Position: " << receivedPosition << std::endl;
            } else if (entry.first == speedHandle) {
                double receivedSpeed;
                std::memcpy(&receivedSpeed, entry.second.data(), sizeof(receivedSpeed));  // Decode float
                std::cout << "Vehicle Speed: " << receivedSpeed << std::endl;
            }
        }

        std::cout << "Pappa polis" << std::endl;

        // Clean up by resigning and destroying the federation execution
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        rtiAmbassador->destroyFederationExecution(L"MyFederation");

    } catch (const rti1516e::Exception& e) {
        std::cerr << "Exception: " << std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(e.what()) << std::endl;
    }

    return 0;
}

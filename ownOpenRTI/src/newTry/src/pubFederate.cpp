#include "../include/pubFederate.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <fstream> // For openFileCheck

// ================================
// This function is temporary / for debugging purposes
void openFileCheck() {
    std::cout << "Debugg-Message 1" << std::endl;

    std::ifstream file("foms/FOM.xml");

    /*
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    */

    if (file.is_open()) {
        std::cout << "FOM.xml file successfully opened!" << std::endl;
    } else {
        std::cout << "Error: Unable to open FOM.xml file at path: foms/FOM.xml" << std::endl;
    }

    file.close();

    std::cout << "End Debugg-Message 1" << std::endl;
}
// ================================

Federate::Federate() : _state(0), _federateAmbassador(std::make_unique<FederateAmbassador>()) {
    _rtiAmbassador = std::make_unique<rti1516e::RTIambassadorFactory>()->createRTIambassador();
}

Federate::~Federate() {
    finalize();
}

void Federate::runFederate(const std::wstring& federateName) {
    this->fedAmb = new FederateAmbassador();
    //openFileCheck();

    std::cout << "Calling connectToRTI" << std::endl;
    connectToRTI();

    std::cout << "Calling initializeFederation" << std::endl;
    initializeFederation();


    std::cout << "Calling joinFederation" << std::endl;
    joinFederation();

    std::cout << "Calling publishOnly" << std::endl;
    publishOnly();

    std::cout << "Calling run" << std::endl;
    run();
}

void Federate::connectToRTI(){
    try{
        std::cout << "Message01: For some *Unknown reason* this message..." << std::endl;
        _rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED);
        std::wcout << L"Connected to the RTI" << std::endl;
        std::wcout << "Message02: ... and this message seems requered for the program to progress..?" << std::endl;

        if (!_rtiAmbassador) {
            std::wcout << L"RTI Ambassador is null after connection attempt!" << std::endl;
            return;
        }

    } catch (rti1516e::ConnectionFailed& e){
        std::wcout << L"Connection failed: " << e.what() << std::endl;
    } catch (rti1516e::InvalidLocalSettingsDesignator& e){
        std::wcout << L"Invalid local settings designator: " << e.what() << std::endl;
    } catch (rti1516e::UnsupportedCallbackModel& e){
        std::wcout << L"Unsupported callback model: " << e.what() << std::endl;
    } catch (rti1516e::AlreadyConnected& e){
        std::wcout << L"Already connected: " << e.what() << std::endl;
    } catch (rti1516e::CallNotAllowedFromWithinCallback& e){
        std::wcout << L"Call not allowed from within callback: " << e.what() << std::endl;
    }
    catch (const rti1516e::Exception& e) {
        std::wcout << L"RTI Exception in runFederate: " << e.what() << std::endl;
    } 
    catch (const std::exception& e) {
        std::wcout << L"Standard Exception in runFederate: " << e.what() << std::endl;
    } 
    catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void Federate::initializeFederation() {
    try{
        std::vector<std::wstring> fomModules = {L"foms/FOM.xml"};
        _rtiAmbassador->createFederationExecution(L"ExampleFederation", fomModules);
    } catch (rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists." << std::endl;
    } catch(rti1516e::Exception& e ){
        std::wcout << L"Something else happened: " << e.what() << std::endl;
    }
}

void Federate::joinFederation() {
    try {
        _rtiAmbassador->joinFederationExecution(L"ExampleFederate", L"ExampleFederation");
        std::wcout << "Joined federation as ExampleFederate." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error joining federation: " << e.what() << std::endl;
    }

    /* ================================
    // This code is temporary / for debugging purposes
    try{
        // After joining, you can perform synchronization points or other operations
        _rtiAmbassador->registerFederationSynchronizationPoint(L"VehicleReady", rti1516e::VariableLengthData());
        std::wcout << L"Synchronization point announced. RTI connection is functional." << std::endl;
    } catch (const rti1516e::Exception& e) {
            std::wcout << L"Error verifying RTI connection: " << e.what() << std::endl;
    }
     ================================*/
}

// ================================
// When this code works, clean up this function
// ================================
void Federate::publishOnly() {
    try {
        vehicleClassHandle = _rtiAmbassador->getObjectClassHandle(L"Vehicle");
        positionHandle = _rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
        speedHandle = _rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");

        rti1516e::AttributeHandleSet attributes;
        attributes.insert(positionHandle);
        attributes.insert(speedHandle);

        _rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, {positionHandle, speedHandle});
        std::wcout << "Published vehicleClassHandle: " << vehicleClassHandle << std::endl;

        // 🔹 ANNOUNCE SYNCHRONIZATION POINT
        std::this_thread::sleep_for(std::chrono::seconds(6));
        rti1516e::FederateHandleSet emptyVector;    // Should send to *all* federates in the federation
        _rtiAmbassador->registerFederationSynchronizationPoint(L"VehicleReady", rti1516e::VariableLengthData(), emptyVector);
        std::wcout << "Synchronization point 'VehicleReady' announced." << std::endl;

        // 🔹 WAIT FOR ALL FEDERATES TO ACHIEVE SYNC
        //waitForSyncPoint();
        //std::wcout << "All federates synchronized. Continuing execution." << std::endl;

        // 🔹 PUBLISH INTERACTION CLASS "VehicleReady"
        vehicleReadyHandle = _rtiAmbassador->getInteractionClassHandle(L"VehicleReady");
        _rtiAmbassador->publishInteractionClass(vehicleReadyHandle);
        std::wcout << "Published interaction class: VehicleReady." << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error publishing attributes: " << e.what() << std::endl;
    }
}

void Federate::waitForSyncPoint() {
    _rtiAmbassador->enableCallbacks();
    while (!fedAmb->isSyncPointAchieved()) {
        bool processed = _rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        std::cout << "[DEBUG] evokeCallback returned: " << (processed ? "true" : "false") << std::endl;
    }
    std::cout << "[DEBUG] Sync achieved! Exiting wait loop." << std::endl;
}


void Federate::run() {
    std::wcout << "Federate running." << std::endl;

    double speed = 0.0;   // Initial speed
    double position = 0.0; // Initial position

    // Step 1: Register the vehicle object in the federation
    registerVehicleObject(position, speed);

    for (int i = 0; i < 20; i++) {
        speed += 1.0;
        position += speed * 2.0;

        std::wcout << L"Iteration " << i+1 << L": Speed = " << speed << L", Position = " << position << std::endl;

        // Step 2: Update the federation with new values
        updateVehicleAttributes(position, speed);

        // Step 3: Wait 2 seconds before next update
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    std::wcout << "Finished updating speed and position." << std::endl;
}

void Federate::registerVehicleObject(double position, double speed) {
    try {
        // Register an instance of the "Vehicle" object
        //_rtiAmbassador->publishObjectClassAttributes(vehicleClassHandle, {positionHandle, speedHandle});
        vehicleInstanceHandle = _rtiAmbassador->registerObjectInstance(vehicleClassHandle);
        std::wcout << "Published Vehicle attributes for: " << vehicleInstanceHandle << std::endl;

        // Set initial values for attributes
        updateVehicleAttributes(position, speed); // Example: position = 0.0, speed = 0.0

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error registering Vehicle object: " << e.what() << std::endl;
    }
}

void Federate::updateVehicleAttributes(double position, double speed) {
    try {
        rti1516e::AttributeHandleValueMap attributes;

        // Encode attributes as HLA variable-length data
        rti1516e::HLAfloat64Time positionValue(position);
        rti1516e::HLAfloat64Time speedValue(speed);

        attributes[positionHandle] = positionHandle.encode();
        attributes[speedHandle] = speedHandle.encode();

        // Send the attribute update to the federation
        _rtiAmbassador->updateAttributeValues(vehicleInstanceHandle, attributes, rti1516e::VariableLengthData());

        std::wcout << "Updated Vehicle attributes: Position = " << position << ", Speed = " << speed << std::endl;

    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error updating Vehicle attributes: " << e.what() << std::endl;
    }
}

void Federate::finalize() {
    try {
        resignFederation();
        std::wcout << "Federate finalized." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error during finalization: " << e.what() << std::endl;
    }
}

void Federate::resignFederation() {
    try {
        _rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        _rtiAmbassador->destroyFederationExecution(L"ExampleFederation");
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}
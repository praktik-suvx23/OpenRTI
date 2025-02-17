#include "syncFederate.h"
#include "syncFederateAmbassador.h"

bool userInteraction() {
    std::string input = "temp";
    while(true){
        std::getline(std::cin, input);
        if (input == "") {
            break;
        }
    }
    return true;
}

syncFederate::syncFederate() {
    rti1516e::RTIambassadorFactory factory;
    rtiAmbassador = factory.createRTIambassador();
}

syncFederate::~syncFederate() {
    resignFederation();
}

void syncFederate::runFederate() {
    fedAmb = std::make_unique<syncFederateAmbassador>(rtiAmbassador.get());

    std::wcout << "Federate connecting to RTI using rti protocol with synchronous callback model..." << std::endl;
    connectToRTI();

    std::wcout << "Creating federation..." << std::endl;
    initializeFederation();

    std::wcout << "Joining federation..." << std::endl;
    joinFederation();

    std::wcout << "Registering sync point..." << std::endl;
    registerSyncPoint();

    std::wcout << "Achieving sync point..." << std::endl;
    achiveSyncPoint();
}

void syncFederate::connectToRTI() {
    try{
        if(!rtiAmbassador){
            std::cout << "RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*fedAmb, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    }catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void syncFederate::initializeFederation() {
    std::vector<std::wstring> fomModules = {L"foms/robot.xml"};
    try {
        rtiAmbassador->createFederationExecutionWithMIM(L"robotFederation", fomModules, L"foms/MIM.xml");
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void syncFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(L"syncFederate", L"robotFederation");
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void syncFederate::registerSyncPoint() {
    std::wcout << "Press 'Enter' to register synchronization point..." << std::endl;
    userInteraction();

    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"InitialSync", rti1516e::VariableLengthData());
        std::wcout << L"Sync Federate waiting for synchronization..." << std::endl;
    
        // TODO: Add timeout
        while (fedAmb->syncLabel != L"InitialSync") { 
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"Sync Federate has announced synchronization point: InitialSync" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while registering synchronization point: " << e.what() << std::endl;
    }
}

void syncFederate::achiveSyncPoint() {    
    try {
        rtiAmbassador->synchronizationPointAchieved(L"InitialSync", true);
        std::wcout << "Synchronization point 'InitialSync' achieved!" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while achieving synchronization point: " << e.what() << std::endl;
    }
}

void syncFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    try {
        syncFederate myFederate;
        myFederate.runFederate();
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
#include "AdminFederate.h"
#include "AdminFederateAmbassadorHelper.h"
#include "AdminFederateFunctions.h"

#include <limits>

AdminFederate::AdminFederate() {
    createRTIambassador();
}

AdminFederate::~AdminFederate() {
    resignFederation();
}

void AdminFederate::runFederate() {
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

    std::wcout << "Initializing handles..." << std::endl;
    initializeHandles();

    std::wcout << "Publishing interactions..." << std::endl;
    publishInteractions();

    std::wcout << "Setup simulation..." << std::endl;
    setupSimulation();

    std::wcout << "Registering sync point for simulation setup complete..." << std::endl;
    registerSyncSimulationSetupComplete();

    std::wcout << "Admin loop..." << std::endl;
    adminLoop();
}

void AdminFederate::createRTIambassador() {
    rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
    federateAmbassador = std::make_unique<AdminFederateAmbassador>(rtiAmbassador.get());
}

void AdminFederate::connectToRTI() {
    try{
        if(!rtiAmbassador){
            std::cout << "RTIambassador is null" << std::endl;
            exit(1);
        }
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
    }catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void AdminFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, minModule);
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void AdminFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
    } catch (...) {
        std::wcout << L"Unknown Exception in runFederate!" << std::endl;
    }
}

void AdminFederate::registerSyncPoint() {
    std::wcout << "Press 'Enter' to register synchronization point..." << std::endl;
    userInteraction();

    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"InitialSync", rti1516e::VariableLengthData());
        std::wcout << L"Sync Federate waiting for synchronization..." << std::endl;
    
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"InitialSync") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }

        std::wcout << L"Sync Federate has announced synchronization point: InitialSync" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while registering synchronization point: " << e.what() << std::endl;
    }
}

void AdminFederate::achiveSyncPoint() {    
    try {
        rtiAmbassador->synchronizationPointAchieved(L"InitialSync", true);
        std::wcout << "Synchronization point 'InitialSync' achieved!" << std::endl;
    } catch (const rti1516e::RTIinternalError& e) {
        std::wcout << L"Error while achieving synchronization point: " << e.what() << std::endl;
    }
}

void AdminFederate::initializeHandles() {
    try {
        AmbassadorSetter::setSetupSimulationHandle(*federateAmbassador, rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.SetupSimulation"));
        AmbassadorSetter::setBlueShipsParam(*federateAmbassador, rtiAmbassador->getParameterHandle(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), L"NumberOfBlueShips"));
        AmbassadorSetter::setRedShipsParam(*federateAmbassador, rtiAmbassador->getParameterHandle(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), L"NumberOfRedShips"));
        AmbassadorSetter::setTimeScaleFactorParam(*federateAmbassador, rtiAmbassador->getParameterHandle(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), L"TimeScaleFactor"));
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error initializing handles: " << e.what() << std::endl;
    }
}

void AdminFederate::publishInteractions() {
    try {
        rtiAmbassador->publishInteractionClass(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador));
        std::wcout << "Published SetupSimulation interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error publishing SetupSimulation interaction: " << e.what() << std::endl;
    }
}

void AdminFederate::setupSimulation() {
    /* Improve this. This isn't OK. Just POC. Need better 'wcin' handling.
    Also: Actually use this in the different ship federates. */
    int redShips = 0;
    int blueShips = 0;
    double timeScaleFactor = 0.0;

    std::wcout << "Enter initial amount of sips on team 'Blue': " << std::endl;
    blueShips = getValidIntInput();

    std::wcout << "Enter initial amount of sips on team 'Red': " << std::endl;
    redShips = getValidIntInput();

    std::wcout << "Enter time scale factor: " << std::endl;
    timeScaleFactor = getValidDoubleInput();

    publishSetupSimulationInteraction(blueShips, redShips, timeScaleFactor);
}

void AdminFederate::publishSetupSimulationInteraction(int teamA, int teamB, double timeScaleFactor) {
    rti1516e::ParameterHandleValueMap parameters;
    parameters[AmbassadorGetter::getBlueShipsParam(*federateAmbassador)] = rti1516e::HLAinteger32BE(teamA).encode();
    parameters[AmbassadorGetter::getRedShipsParam(*federateAmbassador)] = rti1516e::HLAinteger32BE(teamB).encode();
    parameters[AmbassadorGetter::getTimeScaleFactorParam(*federateAmbassador)] = rti1516e::HLAfloat64BE(timeScaleFactor).encode();

    try {
        rtiAmbassador->sendInteraction(AmbassadorGetter::getSetupSimulationHandle(*federateAmbassador), parameters, rti1516e::VariableLengthData());
        std::wcout << "Sent SetupSimulation interaction." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error sending SetupSimulation interaction: " << e.what() << std::endl;
    }
}

void AdminFederate::registerSyncSimulationSetupComplete() {
    try {
        rtiAmbassador->registerFederationSynchronizationPoint(L"SimulationSetupComplete", rti1516e::VariableLengthData());
        std::wcout << L"Registered sync point: SimulationSetupComplete" << std::endl;
        
        while (AmbassadorGetter::getSyncLabel(*federateAmbassador) != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }    
    } catch (const rti1516e::Exception& e) {
        std::wcout << L"Error registering SimulationSetupComplete sync point: " << e.what() << std::endl;
    }
}

void AdminFederate::adminLoop() {
    std::wcout << "~ Bye bye ~\nTODO: Do something with AdminFederate.cpp - adminLoop" << std::endl;
    /* TODO: Implement admin loop functionality
    For example:
    - Determine when to end the simulation
    - Send interactions to control the simulation
    - Manage removal of entities
    - Manage communication between ships
    - Manage which ships fire at which ships
    - Manage which ships are hit
    - Manage adding new ships
    etc.
    */
}

void AdminFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << "Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcout << "Error resigning from federation: " << e.what() << std::endl;
    }
}

int main(int argc, char* argv[]) {

    try {
        AdminFederate myFederate;
        myFederate.runFederate();
    } catch (const std::exception& e) {
        std::wcerr << L"Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
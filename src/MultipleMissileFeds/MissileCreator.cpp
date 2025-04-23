#include "MissileCreator.h"

void MissileCreatorFederate::startMissileCreator() {
    connectToRTI();
    initializeFederation();
    joinFederation();
    waitForSyncPoint();
    initializeHandles();
    subscribeAttributes();
    publishAttributes();
    subscribeInteractions();
    publishInteractions();
    waitForSetupSync();
    initializeTimeFactory();
    enableTimeManagement();
    std::wcout << L"[DEBUG] Starting \"readyCheck\"..." << std::endl;
    readyCheck();
    runSimulationLoop();
}

void MissileCreatorFederate::connectToRTI(){
    try {
        rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();
        federateAmbassador = std::make_unique<MissileCreatorFederateAmbassador>(rtiAmbassador.get());
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createRTIAmbassador - Exception" << e.what() << std::endl;
    } 
}

void MissileCreatorFederate::initializeFederation() {
    try {
        rtiAmbassador->createFederationExecutionWithMIM(federationName, fomModules, mimModule);
        std::wcout << L"[INFO] Federation created: " << federationName << std::endl;
    } catch (const rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"[INFO] Federation already exists: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"[DEBUG - initializeFederation] Exception: " << e.what() << std::endl;
    }
}
void MissileCreatorFederate::joinFederation() {
    try {
        rtiAmbassador->joinFederationExecution(federateName, federationName);
        std::wcout << L"[INFO] Joined federation: " << federationName << std::endl;
    } catch (const rti1516e::FederateAlreadyExecutionMember&) {
        std::wcout << L"[INFO] Already a member of the federation: " << federationName << std::endl;
    } catch (const std::exception& e) {
        std::wcerr << L"[DEBUG - joinFederation] Exception: " << e.what() << std::endl;
    }
}
void MissileCreatorFederate::waitForSyncPoint() {
    std::wcout << L"Waiting for setup sync point..." << std::endl;
    try {
        while (federateAmbassador->getSyncLabel() != L"SimulationSetupComplete") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
        std::wcout << L"Sync point achieved: " << federateAmbassador->getSyncLabel() << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] waitForSetupSync - \"SimulationSetupComplete\" - Exception: " << e.what() << std::endl;
    }
    std::wcout << L"Waiting for all ships to be created..." << std::endl;
    try {
        while (federateAmbassador->getRedSyncLabel() != L"RedShipFederate" || federateAmbassador->getBlueSyncLabel() != L"BlueShipFederate") {
            rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        }
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] waitForSetupSync - \"RedShipFederate||BlueShipFederate\" - Exception: " << e.what() << std::endl;
    }
}

void MissileCreatorFederate::initializeHandles() {

}

void MissileCreatorFederate::subscribeAttributes() {

}
void MissileCreatorFederate::publishAttributes() {

}

int main() {
    try {
        MissileCreatorFederate missileCreatorFederate;
        missileCreatorFederate.startMissileCreator();
    } catch (const std::exception& e) {
        std::wcerr << L"[ERROR - main] Exception: " << e.what() << std::endl;
    }

    return 0;
}

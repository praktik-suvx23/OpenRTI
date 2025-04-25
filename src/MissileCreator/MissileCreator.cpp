#include "MissileCreator.h"

MissileCreatorFederate::MissileCreatorFederate() {
    createRTIAmbassador();
}
MissileCreatorFederate::~MissileCreatorFederate() {
    resignFederation();
}

void MissileCreatorFederate::createRTIAmbassador() {
    try {
        rtiAmbassador = rti1516e::RTIambassadorFactory().createRTIambassador();        
        federateAmbassador = std::make_unique<MissileCreatorFederateAmbassador>(rtiAmbassador.get());
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] createRTIAmbassador - Exception" << e.what() << std::endl;
    }
}

void MissileCreatorFederate::resignFederation() {
    try {
        rtiAmbassador->resignFederationExecution(rti1516e::NO_ACTION);
        std::wcout << L"Resigned from federation." << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[ERROR - resignFederation] Exception: " << e.what() << std::endl;
    }
}

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

    std::wcout << L"[DEBUG] Starting \"readyCheck\"..." << std::endl;
    readyCheck();
    runSimulationLoop();
}

void MissileCreatorFederate::connectToRTI(){
    try {
        rtiAmbassador->connect(*federateAmbassador, rti1516e::HLA_EVOKED, L"rti://localhost:14321");
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

}

void MissileCreatorFederate::initializeHandles() {
    try {    
        //FireMissile
        federateAmbassador->setInteractioClassFireMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.FireMissile"));
        federateAmbassador->setParamShooterID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassFireMissile(), L"ShooterID"));
        federateAmbassador->setParamMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassFireMissile(), L"Team"));
        federateAmbassador->setParamMissileStartPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassFireMissile(), L"ShooterPosition"));
        federateAmbassador->setParamMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassFireMissile(), L"TargetPosition"));
        federateAmbassador->setParamNumberOfMissilesFired(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassFireMissile(), L"NumberOfMissilesFired"));

        //CreateMissile
        federateAmbassador->setInteractioClassCreateMissile(rtiAmbassador->getInteractionClassHandle(L"HLAinteractionRoot.CreateMissile"));
        federateAmbassador->setParamCreateMissileID(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassCreateMissile(), L"MissileID"));
        federateAmbassador->setParamCreateMissileTeam(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassCreateMissile(), L"MissileTeam"));
        federateAmbassador->setParamCreateMissilePosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassCreateMissile(), L"MissilePosition"));
        federateAmbassador->setParamCreateMissileTargetPosition(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassCreateMissile(), L"MissileTargetPosition"));
        federateAmbassador->setParamCreateMissileAltitude(rtiAmbassador->getParameterHandle(federateAmbassador->getInteractioClassCreateMissile(), L"MissileAltitude"));
   
    } catch  (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG] initializeHandles - Exception: " << e.what() << std::endl;
    }
}

void MissileCreatorFederate::subscribeAttributes() {
    // Might not be needed here because attributes are not used
}

void MissileCreatorFederate::publishAttributes() {
    // Might not be needed here because attributes are not used
}

void MissileCreatorFederate::subscribeInteractions() {
    try {

        rtiAmbassador->subscribeInteractionClass(federateAmbassador->getInteractioClassFireMissile());
        std::wcout << L"Subscribed to interaction: " 
                   << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractioClassFireMissile()) 
                   << std::endl;
    } catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - subscribeInteractions] Exception: " << e.what() << std::endl;
    }
}

void MissileCreatorFederate::publishInteractions() {
    try  {  
        rtiAmbassador->publishInteractionClass(federateAmbassador->getInteractioClassCreateMissile());
        std::wcout << L"Published interaction: " 
                    << rtiAmbassador->getInteractionClassName(federateAmbassador->getInteractioClassCreateMissile()) 
                    << std::endl;
    }   catch (const rti1516e::Exception& e) {
        std::wcerr << L"[DEBUG - publishInteractions] Exception: " << e.what() << std::endl;
    }
}

void MissileCreatorFederate::waitForSetupSync() {
    //Might not be needed here because time is not used
}

void MissileCreatorFederate::readyCheck() {
    //Might not be needed here because time is not used
}

void MissileCreatorFederate::runSimulationLoop(){
    while(true){
        std::wcout << L"Waiting to create missiles.." << std::endl;
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
        // Add logic to create missiles and send interactions
        // This is where the missile creation logic would go
    }
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

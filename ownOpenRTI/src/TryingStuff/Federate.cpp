#include "Federate.h"

testFederate::testFederate(){

}

testFederate::~testFederate(){

}

void testFederate::runFederate(std::wstring federateName){
    
    // 1. create the RTIambassador
    rti1516e::RTIambassadorFactory factory;
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador = factory.createRTIambassador(); // factory.createRTIambassador().release()?

    // 2. connect to the RTI
    MyFederateAmbassador ambassador;
    rtiAmbassador->connect(ambassador, rti1516e::HLA_EVOKED);

    // 3. create and join the federation
    try{
        // add FOM files
        vector<wstring> foms;
        foms.push_back(L"foms/FOM.xml");

        std::wstring federationName = L"testFederation";
        rtiAmbassador->createFederationExecution(federationName, foms);
        std::wcout << L"Federation created successfully." << std::endl;
    } catch (rti1516e::FederationExecutionAlreadyExists&) {
        std::wcout << L"Federation already exists." << std::endl;
    } catch( Exception& e ){
        std::wcout << L"Something else happened: " << e.what() << std::endl;
    }

    // 4. join the federation
    rtiAmbassador->joinFederationExecution(federateName, L"testFederate", L"testFederation");
    std::wcout << L"Joined federation as " << federateName << std::endl;

    initializeHandles();

    // 5. announce the sync point
    VariableLengthData tag((void*)"", 1);
    
}

void testFederate::initializeHandles(){
    this->vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
    this->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
    this->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
}
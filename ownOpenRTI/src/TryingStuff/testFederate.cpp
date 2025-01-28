#include "testFederate.h"
#include "testFedAmb.h"
#include <iostream>
#include <vector>

testFederate::testFederate(){

}

testFederate::~testFederate(){

}

void testFederate::runFederate(const std::wstring& federateName){
    
    // 1. create the RTIambassador
    rti1516e::RTIambassadorFactory factory;
    this->rtiAmbassador = factory.createRTIambassador().release(); // factory.createRTIambassador().release()?

    // 2. connect to the RTI
    this->fedAmbassador = new testFedAmb();
    try{
        rtiAmbassador->connect(*fedAmbassador, HLA_EVOKED);
        std::wcout << L"Connected to the RTI" << std::endl;
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

    // 3. create and join the federation
    try{
        // add FOM files
        std::vector<std::wstring> foms;
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
    rti1516e::VariableLengthData tag((void*)"", 1);
    rtiAmbassador->registerFederationSynchronizationPoint(READY_TO_RUN, tag);
    while(rtiAmbassador->isAnnounced == false){
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    waitForUser();
    
    // 6. achieve the sync point
    rtiAmbassador->synchronizationPointAchieved(READY_TO_RUN);
    std::wcout << L"Achieved sync point: " << READY_TO_RUN << ", waiting for federation..." << std::endl;
    while( fedAmbassador->isReadyToRun == false ){
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    // 7. enable time regulation
    enableTimePolicy();
    std::wcout << L"Time Policy Enabled" << std::endl;

    // 8. publish and subscribe
    publishAndSubscribe();
    std::wcout << L"Published and Subscribed" << std::endl;

    // 9. register an object to update
    rti1516e::ObjectInstanceHandle objectHandle = registerObject();
    std::wcout << L"Registered Object, handle=" << objectHandle << std::endl;

    // 10. main simulation loop
    for(int i = 0; i < 20; i++){
        // 10.1 update the attribute values of the object
        updateAttributeValues( objectHandle );

        // 10.2 send an update
        sendInteraction();

        // 10.3 request a time advance and wait until it is granted
        advanceTime(1.0);
        std::wcout << L"Time Advanced to " << fedAmbassador->federateTime << std::endl;
    }
}

void testFederate::initializeHandles(){
    this->vehicleClassHandle = rtiAmbassador->getObjectClassHandle(L"Vehicle");
    this->positionHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Position");
    this->speedHandle = rtiAmbassador->getAttributeHandle(vehicleClassHandle, L"Speed");
}

void testFederate::waitForUser(){
    std::wcout << L" >>>>>>>>>> Press Enter to Continue <<<<<<<<<<";
    std::wstring line;
    std::getline(std::wcin, line);
}

void testFederate::enableTimePolicy(){
    double lookahead = fedAmbassador->federateLookahead;
    std::auto_ptr<HLAfloat64Interval> lookaheadInterval(new HLAfloat64Interval(lookahead));
    rtiAmbassador->enableTimeRegulation(*lookaheadInterval);

    while(fedAmbassador->isRegulating == false){
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }

    rtiAmbassador->enableTimeConstrained();

    while(fedAmbassador->isConstrained == false){
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }
}

void testFederate::publishAndSubscribe(){
    AttributeHandleSet attributes;
    attributes.insert(this->positionHandle);
    attributes.insert(this->speedHandle);

    rtiAmbassador->publishObjectClassAttributes(this->vehicleClassHandle, attributes);

    rtiAmbassador->subscribeObjectClassAttributes(this->vehicleClassHandle, attributes);
}

rti1516e::ObjectInstanceHandle testFederate::registerObject(){
    rti1516e::ObjectInstanceHandle objectHandle = rtiAmbassador->registerObjectInstance(this->vehicleClassHandle);
    return objectHandle;
}

void testFederate::updateAttributeValues(rti1516e::ObjectInstanceHandle objectHandle){
    AttributeHandleValueMap attributes;

    VariableLengthData position((void*)"Position", 8);
    VariableLengthData speed((void*)"Speed", 5);

    attributes[this->positionHandle] = position;
    attributes[this->speedHandle] = speed;
}

void testFederate::sendInteraction(){
    InteractionClassHandle interactionHandle = rtiAmbassador->getInteractionClassHandle(L"InteractionRoot.InteractionOne");
    ParameterHandleValueMap parameters;

    VariableLengthData param((void*)"Parameter", 9);
    parameters[this->positionHandle] = param;

    VariableLengthData tag((void*)"", 1);
    rtiAmbassador->sendInteraction(interactionHandle, parameters, tag);
}

void testFederate::advanceTime(double timestep){
    fedAmbassador->isAdvancing = true;
    std::auto_ptr<HLAfloat64Time> newTime( new HLAfloat64Time(fedAmbassador->federateTime + timestep));
    rtiAmbassador->timeAdvanceRequest(*newTime);

    while(fedAmbassador->isAdvancing){
        rtiAmbassador->evokeMultipleCallbacks(0.1, 1.0);
    }
}
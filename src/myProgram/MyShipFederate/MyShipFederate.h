#ifndef MYSHIPFEDERATE_H
#define MYSHIPFEDERATE_H

#include "MyShipFederateAmbassador.h"

class MyShipFederate {
public:
    MyShipFederate();
    ~MyShipFederate();
    void runFederate(const std::wstring& federateName);

    // Make these private?
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<MyShipFederateAmbassador> federateAmbassador;

    void readJsonFile(int i);
    void createRTIAmbassador();
    void connectToRTI();
    void initializeFederation();
    void joinFederation();
    void waitForSyncPoint();
    void initializeHandles();
    void publishAttributes();
    void registerShipObject();
    void subscribeInteractions();
    void publishInteractions();
    void runSimulationLoop();
    void sendHitEvent();
    void updateShipAttributes(
        const std::wstring& shipLocation, 
        const std::wstring& futureShipLocation, 
        double shipSpeed, 
        const rti1516e::LogicalTime& logicalTimePtr
    );
    void resignFederation();

    void enableTimeManegement();

    rti1516e::HLAfloat64TimeFactory* logicalTimeFactory = nullptr;
    void initializeTimeFactory();
    
};

#endif 
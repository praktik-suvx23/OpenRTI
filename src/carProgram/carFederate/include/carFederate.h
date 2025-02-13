#ifndef CARFEDERATE_H
#define CARFEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

#include "carFedAmb.h"

class carFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<carFedAmb> fedAmb;

    rti1516e::VariableLengthData tag;
    rti1516e::AttributeHandleSet attributeHandleSet;
    rti1516e::AttributeHandleValueMap attributeValues;

    rti1516e::ParameterHandle parameterHandle;
    rti1516e::InteractionClassHandle interactionClassHandle;

    std::wstring federationName;
    std::wstring federateName;
    std::wstring fomModule;
    std::vector<std::wstring> fomModules;
    std::wstring mimModule;

    std::string carName = "";
    std::string licensePlate = "";
    std::string fuelType = "";
    double normalSpeed = 0.0;
    double fuelConsumption1 = 0.0;
    double fuelConsumption2 = 0.0;
    double fuelConsumption3 = 0.0;

    double topLeftLat = 0.0;
    double topLeftLong = 0.0;
    double bottomRightLat = 0.0;
    double bottomRightLong = 0.0;
    double startLat = 0.0;
    double startLong = 0.0;
    double goalLat = 0.0;
    double goalLong = 0.0;

    double fuelLevel = 0.0;
    
    carFederate();
    ~carFederate();
    void runFederate(const std::wstring& federateName);
    
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation(std::wstring federateName);
    void achieveSyncPoint();
    void initializeHandles();
    void run();
    void loadScenario();
    void loadCarConfig(std::string carConfig);
    void runSimulation();
    void finalize();
    void resignFederation();
};

#endif
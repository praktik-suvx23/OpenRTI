#ifndef CARFEDERATE_H
#define CARFEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

#include "carFedAmb.h"

class subFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<carFedAmb> fedAmb;

    rti1516e::VariableLengthData tag;
    rti1516e::AttributeHandleSet attributeHandleSet;
    rti1516e::AttributeHandleValueMap attributeValues;

    rti1516e::ParameterHandle parameterHandle;
    rti1516e::InteractionClassHandle interactionClassHandle;

    std::wstring federationName;
    std::wstring fomModule;
    std::vector<std::wstring> fomModules;
    std::wstring mimModule;
    
    subFederate();
    ~subFederate();
    void runFederate(const std::wstring& federateName);
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation(std::wstring federateName);
    void achieveSyncPoint();
    void initializeHandles();
    void run();
    void loadCarConfig(std::string carConfig);
    void registerCarObject();
    void updateCarState();
    void finalize();
    void resignFederation();
};

#endif
#ifndef SCENARIOFEDERATE_H
#define SCENARIOFEDERATE_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAinteger64TimeFactory.h>
#include <memory>

#include "scenarioFedAmb.h"

class scenarioFederate {

public:
    std::unique_ptr<rti1516e::RTIambassador> rtiAmbassador;
    std::unique_ptr<scenarioFedAmb> fedAmb;

    std::wstring federationName;
    std::wstring fomModule;
    std::vector<std::wstring> fomModules;
    std::wstring mimModule;

    rti1516e::InteractionClassHandle scenarioDataHandle;
    rti1516e::ParameterHandle topLeftLatParamHandle;
    rti1516e::ParameterHandle topLeftLongParamHandle;
    rti1516e::ParameterHandle bottomRightLatParamHandle;
    rti1516e::ParameterHandle bottomRightLongParamHandle;
    rti1516e::ParameterHandle startLatParamHandle;
    rti1516e::ParameterHandle startLongParamHandle;
    rti1516e::ParameterHandle stopLatParamHandle;
    rti1516e::ParameterHandle stopLongParamHandle;

    double topLeftLat = 0;
    double topLeftLong = 0;
    double bottomRightLat = 0;
    double bottomRightLong = 0;
    double startLat = 0;
    double startLong = 0;
    double stopLat = 0;
    double stopLong = 0;

    scenarioFederate();
    ~scenarioFederate();
    void runFederate(const std::wstring& federateName);
private:
    void connectToRTI();
    void initializeFederation();
    void joinFederation(std::wstring federateName);
    void achiveSyncPoint();
    void initializeHandles();
    void run();    
    void loadScenario(std::string scenarioConfig);
    void publishScenario(std::wstring scenarioName);
    void finalize();
    void resignFederation();
};

#endif
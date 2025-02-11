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

    double topLeftLat, topLeftLong, bottomRightLat, bottomRightLong, startLat, startLong, stopLat, stopLong, initialFuelAmount;

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
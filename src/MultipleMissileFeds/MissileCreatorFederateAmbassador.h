#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>

#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTimeInterval.h>
#include <RTI/LogicalTime.h>

#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <numeric>  
#include <sstream> 

#include "../include/MissileCalculator.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "../include/decodePosition.h"

class MissileCreatorFederateAmbassador : public rti1516e::NullFederateAmbassador {  
private:
    rti1516e::RTIambassador* _rtiAmbassador;

    // Variables used in: announceSynchronizationPoint
    std::wstring syncLabel = L"";
    std::wstring redSyncLabel = L"";
    std::wstring blueSyncLabel = L"";

    // Variables used in: receiveInteraction
    // interactionClassFireMissile
    std::wstring shooterID;
    std::pair<double, double> missilePosition;
    std::pair<double, double> missileTargetPosition;
    double initialBearing;
    int numberOfMissilesFired;
public: 
};
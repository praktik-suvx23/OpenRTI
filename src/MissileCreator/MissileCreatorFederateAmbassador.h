#ifndef MISSILECREATORFEDERATEAMBASSADOR_H
#define MISSILECREATORFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>

#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include <cmath>

#include <iomanip>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include "../include/MissileCalculator.h"
#include "../include/ObjectInstanceHandleHash.h"
#include "../include/decodePosition.h"
#include "../include/loggingFunctions.h"

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

    // Parameters and handle for interaction class FireMissile (Subscribe)
    rti1516e::InteractionClassHandle interactionClassFireMissile;
    rti1516e::ParameterHandle parameterHandleShooterID;
    rti1516e::ParameterHandle parameterHandleTargetID;
    rti1516e::ParameterHandle parameterHandleMissileTeam;
    rti1516e::ParameterHandle parameterHandleMissileStartPosition;
    rti1516e::ParameterHandle parameterHandleMissileTargetPosition;
    rti1516e::ParameterHandle parameterHandleNumberOfMissilesFired;
    rti1516e::ParameterHandle parameterHandleMissileSpeed;

public: 
int testForInteraction = false;

    void StartMissile(
    const std::wstring& shooterID,
    const std::wstring& targetID,
    const std::wstring& missileTeam,
    const std::pair<double, double>& missileStartPosition,
    const std::pair<double, double>& missileTargetPosition,
    const int& numberOfMissilesFired,
    const double& initialBearing);

    MissileCreatorFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~MissileCreatorFederateAmbassador();
    // Function to create missile objects

    void announceSynchronizationPoint(
        std::wstring const& label,
        rti1516e::VariableLengthData const& theUserSuppliedTag) override;       
    
    void discoverObjectInstance(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::ObjectClassHandle theObjectClass,
        std::wstring const &theObjectName) override;

    void receiveInteraction(//Receive interaction without time
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    std::wstring getSyncLabel() const;
    std::wstring getRedSyncLabel() const;
    std::wstring getBlueSyncLabel() const;

    //Set get for interaction class FireMissile
    void setInteractioClassFireMissile(rti1516e::InteractionClassHandle interactionClassHandle);
    rti1516e::InteractionClassHandle getInteractioClassFireMissile() const;

    void setParamShooterID(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamShooterID() const;

    void setParamTargetID(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamTargetID() const;

    void setParamMissileTeam(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamMissileTeam() const;

    void setParamMissileStartPosition(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamMissileStartPosition() const;

    void setParamMissileTargetPosition(rti1516e::ParameterHandle parameterHandle);  
    rti1516e::ParameterHandle getParamMissileTargetPosition() const;

    void setParamNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamNumberOfMissilesFired() const;
};

#endif // MISSILECREATORFEDERATEAMBASSADOR_H
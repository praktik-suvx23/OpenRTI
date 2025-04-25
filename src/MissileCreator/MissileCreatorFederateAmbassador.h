#ifndef MISSILECREATORFEDERATEAMBASSADOR_H
#define MISSILECREATORFEDERATEAMBASSADOR_H

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


    // Parameters and handle for interaction class FireMissile (Subscribe)
    rti1516e::InteractionClassHandle interactionClassFireMissile;
    rti1516e::ParameterHandle parameterHandleShooterID;
    rti1516e::ParameterHandle parameterHandleMissileTeam;
    rti1516e::ParameterHandle parameterHandleMissileStartPosition;
    rti1516e::ParameterHandle parameterHandleMissileTargetPosition;
    rti1516e::ParameterHandle parameterHandleNumberOfMissilesFired;
    rti1516e::ParameterHandle parameterHandleMissileSpeed;


    // Parameters and handle for interaction class CreateMissile (Publish)
    rti1516e::InteractionClassHandle interactionClassCreateMissile;
    rti1516e::ParameterHandle parameterHandleCreateMissileID;
    rti1516e::ParameterHandle parameterHandleCreateMissileTeam;
    rti1516e::ParameterHandle parameterHandleCreateMissilePosition;
    rti1516e::ParameterHandle parameterHandleCreateMissileTargetPosition;
    rti1516e::ParameterHandle parameterHandleCreateMissileAltitude;
    rti1516e::ParameterHandle parameterHandleCreateMissileNumberOfMissilesFired;
    rti1516e::ParameterHandle parameterHandleCreateMissileBearing;

public: 
int testForInteraction = false;

    void sendStartMissileInteraction(
    std::wstring shooterID,
    std::wstring missileTeam,
    std::pair<double, double> missileStartPosition,
    std::pair<double, double> missileTargetPosition,
    double missileAltitude,
    double missileSpeed,
    int numberOfMissilesFired,
    double initialBearing);

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

    void reflectAttributeValues(
        rti1516e::ObjectInstanceHandle theObject,
        rti1516e::AttributeHandleValueMap const &theAttributes,
        rti1516e::VariableLengthData const &theTag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType theType,
        rti1516e::SupplementalReflectInfo theReflectInfo) override;

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

    void setParamMissileTeam(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamMissileTeam() const;

    void setParamMissileStartPosition(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamMissileStartPosition() const;

    void setParamMissileTargetPosition(rti1516e::ParameterHandle parameterHandle);  
    rti1516e::ParameterHandle getParamMissileTargetPosition() const;

    void setParamNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamNumberOfMissilesFired() const;

    void setParamMissileSpeed(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamMissileSpeed() const;

    //Set get for interaction class CreateMissile
    void setInteractioClassCreateMissile(rti1516e::InteractionClassHandle interactionClassHandle);
    rti1516e::InteractionClassHandle getInteractioClassCreateMissile() const;

    void setParamCreateMissileID(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamCreateMissileID() const;

    void setParamCreateMissileTeam(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamCreateMissileTeam() const;

    void setParamCreateMissilePosition(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamCreateMissilePosition() const;

    void setParamCreateMissileTargetPosition(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamCreateMissileTargetPosition() const;

    void setParamCreateMissileAltitude(rti1516e::ParameterHandle parameterHandle); //Might not be needed
    rti1516e::ParameterHandle getParamCreateMissileAltitude() const;//---

    void setParamCreateMissileNumberOfMissilesFired(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamCreateMissileNumberOfMissilesFired() const;

    void setParamCreateMissileBearing(rti1516e::ParameterHandle parameterHandle);
    rti1516e::ParameterHandle getParamCreateMissileBearing() const;

};

#endif // MISSILECREATORFEDERATEAMBASSADOR_H
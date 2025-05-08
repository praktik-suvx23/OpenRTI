#ifndef ADMINFEDERATEAMBASSADOR_H
#define ADMINFEDERATEAMBASSADOR_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <unordered_map>

#include <RTI/LogicalTimeFactory.h>
#include <RTI/LogicalTimeInterval.h>
#include <RTI/LogicalTime.h>

#include <RTI/time/HLAfloat64Interval.h>
#include <RTI/time/HLAfloat64Time.h>
#include <RTI/time/HLAfloat64TimeFactory.h>
#include <iostream>
#include <thread>

#include "tempStruct.h"

class AmbassadorGetter;
class AmbassadorSetter;

class AdminFederateAmbassador : public rti1516e::NullFederateAmbassador {
    friend class AmbassadorGetter;
    friend class AmbassadorSetter;

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    rti1516e::RTIambassador* _rtiAmbassador;

    std::wstring syncLabel = L"";

    std::unordered_map<TargetInfo*, uint8_t> targetLockMap;

    rti1516e::InteractionClassHandle setupSimulationHandle; // Interaction class handle for setupSimulation interaction
    rti1516e::ParameterHandle blueShipsParam;               // Parameter handle for blueShips parameter
    rti1516e::ParameterHandle redShipsParam;                // Parameter handle for redShips parameter
    rti1516e::ParameterHandle timeScaleFactorParam;         // Parameter handle for timeScaleFactor parameter

    // Initiate handshake - RED
    rti1516e::InteractionClassHandle interactionClassInitiateRedHandshake;
    rti1516e::ParameterHandle parameterHandleInitiateRedShooterID;
    rti1516e::ParameterHandle parameterHandleInitiateRedMissilesAvailable;
    rti1516e::ParameterHandle parameterHandleInitiateRedTargetID;
    rti1516e::ParameterHandle parameterHandleInitiateRedMaxMissilesRequired;
    rti1516e::ParameterHandle parameterHandleInitiateRedMissilesCurrentlyTargeting;

    // Initiate handshake - BLUE
    rti1516e::InteractionClassHandle interactionClassInitiateBlueHandshake;
    rti1516e::ParameterHandle parameterHandleInitiateBlueShooterID;
    rti1516e::ParameterHandle parameterHandleInitiateBlueMissilesAvailable;
    rti1516e::ParameterHandle parameterHandleInitiateBlueTargetID;
    rti1516e::ParameterHandle parameterHandleInitiateBlueMaxMissilesRequired;
    rti1516e::ParameterHandle parameterHandleInitiateBlueMissilesCurrentlyTargeting;

    // Confirm handshake - RED
    rti1516e::InteractionClassHandle interactionClassConfirmRedHandshake;
    rti1516e::ParameterHandle parameterHandleConfirmRedShooterID;
    rti1516e::ParameterHandle parameterHandleConfirmRedMissilesLoaded;
    rti1516e::ParameterHandle parameterHandleConfirmRedTargetID;
    rti1516e::ParameterHandle parameterHandleConfirmRedCurrentlyTargeting;

    // Confirm handshake - BLUE
    rti1516e::InteractionClassHandle interactionClassConfirmBlueHandshake;
    rti1516e::ParameterHandle parameterHandleConfirmBlueShooterID;
    rti1516e::ParameterHandle parameterHandleConfirmBlueMissilesLoaded;
    rti1516e::ParameterHandle parameterHandleConfirmBlueTargetID;
    rti1516e::ParameterHandle parameterHandleConfirmBlueCurrentlyTargeting;
public:
    AdminFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~AdminFederateAmbassador();

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    std::chrono::time_point<std::chrono::high_resolution_clock> getStartTime() const;
    void setStartTime(const std::chrono::time_point<std::chrono::high_resolution_clock>& time);

    void timeRegulationEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeConstrainedEnabled(const rti1516e::LogicalTime& theFederateTime) override;
    void timeAdvanceGrant(const rti1516e::LogicalTime& theTime) override;

    // Initiate handshake - RED
    rti1516e::InteractionClassHandle getInteractionClassInitiateRedHandshake() const;
    void setInteractionClassInitiateRedHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamInitiateRedShooterID() const;
    void setParamInitiateRedShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateRedMissilesAvailable() const;
    void setParamInitiateRedMissilesAvailable(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateRedTargetID() const;
    void setParamInitiateRedTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateRedMaxMissilesRequired() const;
    void setParamInitiateRedMaxMissilesRequired(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateRedMissilesCurrentlyTargeting() const;
    void setParamInitiateRedMissilesCurrentlyTargeting(const rti1516e::ParameterHandle& handle);

    // Initiate handshake - BLUE
    rti1516e::InteractionClassHandle getInteractionClassInitiateBlueHandshake() const;
    void setInteractionClassInitiateBlueHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamInitiateBlueShooterID() const;
    void setParamInitiateBlueShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateBlueMissilesAvailable() const;
    void setParamInitiateBlueMissilesAvailable(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateBlueTargetID() const;
    void setParamInitiateBlueTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateBlueMaxMissilesRequired() const;
    void setParamInitiateBlueMaxMissilesRequired(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamInitiateBlueMissilesCurrentlyTargeting() const;
    void setParamInitiateBlueMissilesCurrentlyTargeting(const rti1516e::ParameterHandle& handle);

    // Confirm handshake - RED
    rti1516e::InteractionClassHandle getInteractionClassConfirmRedHandshake() const;
    void setInteractionClassConfirmRedHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamConfirmRedShooterID() const;
    void setParamConfirmRedShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmRedMissilesLoaded() const;
    void setParamConfirmRedMissilesLoaded(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmRedTargetID() const;
    void setParamConfirmRedTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmRedCurrentlyTargeting() const;
    void setParamConfirmRedCurrentlyTargeting(const rti1516e::ParameterHandle& handle);

    // Confirm handshake - BLUE
    rti1516e::InteractionClassHandle getInteractionClassConfirmBlueHandshake() const;
    void setInteractionClassConfirmBlueHandshake(const rti1516e::InteractionClassHandle& handle);

    rti1516e::ParameterHandle getParamConfirmBlueShooterID() const;
    void setParamConfirmBlueShooterID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmBlueMissilesLoaded() const;
    void setParamConfirmBlueMissilesLoaded(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmBlueTargetID() const;
    void setParamConfirmBlueTargetID(const rti1516e::ParameterHandle& handle);

    rti1516e::ParameterHandle getParamConfirmBlueCurrentlyTargeting() const;
    void setParamConfirmBlueCurrentlyTargeting(const rti1516e::ParameterHandle& handle);
};

#endif
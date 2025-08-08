#ifndef ADMINFEDERATEAMBASSADOR_H
#define ADMINFEDERATEAMBASSADOR_H

//Many of the includes that are being used here are from the RTI library
//Also includes such as Vector and string are inside the RTI library
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
#include <optional>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <limits>
#include <algorithm>
#include <chrono>
#include <vector>
#include <map>

#include "structAdmin.h"
#include "../include/loggingFunctions.h"

class AdminFederateAmbassador : public rti1516e::NullFederateAmbassador {

    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;

    rti1516e::RTIambassador* _rtiAmbassador;

    std::wstring syncLabel = L"";
    loggingType logType = loggingType::LOGGING_DEFAULT;

    rti1516e::InteractionClassHandle interactionClassFireMissile; // Interaction class handle for fireMissile interaction

    std::vector<InitialHandshake> blueInitialHandshake;
    std::vector<InitialHandshake> redInitialHandshake;

    std::unordered_map<std::wstring, std::optional<int32_t>> missilesLeftToTargetBlue;
    std::unordered_map<std::wstring, std::optional<int32_t>> missilesLeftToTargetRed;

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
    rti1516e::ParameterHandle parameterHandleInitiateRedDistanceToTarget;

    // Initiate handshake - BLUE
    rti1516e::InteractionClassHandle interactionClassInitiateBlueHandshake;
    rti1516e::ParameterHandle parameterHandleInitiateBlueShooterID;
    rti1516e::ParameterHandle parameterHandleInitiateBlueMissilesAvailable;
    rti1516e::ParameterHandle parameterHandleInitiateBlueTargetID;
    rti1516e::ParameterHandle parameterHandleInitiateBlueMaxMissilesRequired;
    rti1516e::ParameterHandle parameterHandleInitiateBlueMissilesCurrentlyTargeting;
    rti1516e::ParameterHandle parameterHandleInitiateBlueDistanceToTarget;

    // Confirm handshake - RED
    rti1516e::InteractionClassHandle interactionClassConfirmRedHandshake;
    rti1516e::ParameterHandle parameterHandleConfirmRedShooterID;
    rti1516e::ParameterHandle parameterHandleConfirmRedMissilesLoaded;
    rti1516e::ParameterHandle parameterHandleConfirmRedTargetID;
    rti1516e::ParameterHandle parameterHandleConfirmRedBoolean;

    // Confirm handshake - BLUE
    rti1516e::InteractionClassHandle interactionClassConfirmBlueHandshake;
    rti1516e::ParameterHandle parameterHandleConfirmBlueShooterID;
    rti1516e::ParameterHandle parameterHandleConfirmBlueMissilesLoaded;
    rti1516e::ParameterHandle parameterHandleConfirmBlueTargetID;
    rti1516e::ParameterHandle parameterHandleConfirmBlueBoolean;
public:
    bool redShipReady = false;
    bool blueShipReady = false;

    AdminFederateAmbassador(rti1516e::RTIambassador* rtiAmbassador);
    ~AdminFederateAmbassador();

    void receiveInteraction(
    rti1516e::InteractionClassHandle interactionClassHandle,
    const rti1516e::ParameterHandleValueMap& parameterValues,
    const rti1516e::VariableLengthData& tag,
    rti1516e::OrderType sentOrder,
    rti1516e::TransportationType transportationType,
    rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        const rti1516e::LogicalTime& theTime,
        rti1516e::OrderType receivedOrder,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    bool isRegulating = false;
    bool isConstrained = false;
    bool isAdvancing = false;

    int missilesBeingCreated = 0; // Flag to indicate if missiles are being created

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

    rti1516e::ParameterHandle getParamInitiateRedDistanceToTarget() const;
    void setParamInitiateRedDistanceToTarget(const rti1516e::ParameterHandle& handle);

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

    rti1516e::ParameterHandle getParamInitiateBlueDistanceToTarget() const;
    void setParamInitiateBlueDistanceToTarget(const rti1516e::ParameterHandle& handle);

    // Getters using Team (dynamic)
    rti1516e::InteractionClassHandle getInteractionClassConfirmHandshake(Team side) const;
    rti1516e::ParameterHandle getParamConfirmShooterID(Team side) const;
    rti1516e::ParameterHandle getParamConfirmMissilesLoaded(Team side) const;
    rti1516e::ParameterHandle getParamConfirmTargetID(Team side) const;
    rti1516e::ParameterHandle getParamConfirmBoolean(Team side) const;

    // Setters using Team (dynamic)
    void setInteractionClassConfirmHandshake(Team side, const rti1516e::InteractionClassHandle& handle);
    void setParamConfirmShooterID(Team side, const rti1516e::ParameterHandle& handle);
    void setParamConfirmMissilesLoaded(Team side, const rti1516e::ParameterHandle& handle);
    void setParamConfirmTargetID(Team side, const rti1516e::ParameterHandle& handle);
    void setParamConfirmBoolean(Team side, const rti1516e::ParameterHandle& handle);

    loggingType getLogType() const;
    void setLogType(loggingType newType);

    //Get and clear - initialHandshakeMap
    std::vector<InitialHandshake>& getInitialHandshakeBlue();
    std::vector<InitialHandshake>& getInitialHandshakeRed();

    //Get, set and clear - missilesLeftToTarget
    std::unordered_map<std::wstring, std::optional<int32_t>>& getMissilesLeftToTargetBlue();
    std::unordered_map<std::wstring, std::optional<int32_t>>& getMissilesLeftToTargetRed();

    void setSyncLabel(std::wstring label);
    void setSetupSimulationHandle(rti1516e::InteractionClassHandle handle);
    void setBlueShipsParam(rti1516e::ParameterHandle handle);
    void setRedShipsParam(rti1516e::ParameterHandle handle);
    void setTimeScaleFactorParam(rti1516e::ParameterHandle handle);

    std::wstring getSyncLabel();
    rti1516e::InteractionClassHandle getSetupSimulationHandle();
    rti1516e::ParameterHandle getBlueShipsParam();
    rti1516e::ParameterHandle getRedShipsParam();
    rti1516e::ParameterHandle getTimeScaleFactorParam();
};

#endif
#ifndef CARFEDAMB_H
#define CARFEDAMB_H

#include <RTI/RTIambassadorFactory.h>
#include <RTI/RTIambassador.h>
#include <RTI/NullFederateAmbassador.h>
#include <RTI/encoding/BasicDataElements.h>
#include <RTI/encoding/EncodingExceptions.h>
#include <RTI/encoding/DataElement.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <unordered_map>

class carFedAmb : public rti1516e::NullFederateAmbassador {
public:
    carFedAmb(rti1516e::RTIambassador* rtiAmbassador);
    ~carFedAmb();

    void receiveInteraction(
        rti1516e::InteractionClassHandle interactionClassHandle,
        const rti1516e::ParameterHandleValueMap& parameterValues,
        const rti1516e::VariableLengthData& tag,
        rti1516e::OrderType sentOrder,
        rti1516e::TransportationType transportationType,
        rti1516e::SupplementalReceiveInfo receiveInfo) override;

    void announceSynchronizationPoint (
         std::wstring  const & label,
         rti1516e::VariableLengthData const & theUserSuppliedTag) override;

    std::wstring syncLabel = L"";

    rti1516e::ObjectClassHandle carObjectClassHandle;
    rti1516e::ObjectInstanceHandle carObjectInstanceHandle;
    rti1516e::AttributeHandle carNameHandle;
    rti1516e::AttributeHandle licensePlateHandle;
    rti1516e::AttributeHandle fuelLevelHandle;
    rti1516e::AttributeHandle fuelTypeHandle;
    rti1516e::AttributeHandle positionHandle;

    // 🚗 New Parameters (Handles for scenario data)
    rti1516e::ParameterHandle startLatHandle;
    rti1516e::ParameterHandle startLongHandle;
    rti1516e::ParameterHandle stopLatHandle;
    rti1516e::ParameterHandle stopLongHandle;
    rti1516e::ParameterHandle initFuelHandle;

    // 🚗 Vehicle-specific data
    std::string carName = "";
    std::string licensePlate = "";
    std::string fuelType = "";
    double fuelConsumption1 = 0;
    double fuelConsumption2 = 0;
    double fuelConsumption3 = 0;
    double normalSpeed = 0;

    // 🚗 Scenario-related attributes
    double goalLat = -1;
    double goalLong = -1;
    double startLat = -1;
    double startLong = -1;
    double fuelLevel = -1;
    bool simulationRunning = true;

    int savedData = 0;

    // 🚗 Handles for interactions
    rti1516e::InteractionClassHandle loadScenarioHandle;
    rti1516e::InteractionClassHandle startHandle;
    rti1516e::InteractionClassHandle stopHandle;

private:
    rti1516e::RTIambassador* _rtiAmbassador;
};

#endif